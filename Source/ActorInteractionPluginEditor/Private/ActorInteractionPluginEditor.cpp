// All rights reserved Dominik Morse (Pavlicek) 2021

#include "ActorInteractionPluginEditor.h"

#include "Interfaces/IPluginManager.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"

#include "Modules/ModuleManager.h"

#include "AssetActions/InteractorComponentAssetActions.h"
#include "AssetActions/InteractableComponentAssetActions.h"

#include "AssetToolsModule.h"
#include "GameplayTagsManager.h"
#include "HttpModule.h"
#include "HelpButton/AIntPCommands.h"
#include "HelpButton/AIntPHelpStyle.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Popup/AIntPPopup.h"
#include "Utilities/ActorInteractionEditorUtilities.h"

#include "ToolMenus.h"
#include "AssetActions/InteractionSettingsConfig.h"
#include "DetailsPanel/MounteaInteractableBase_DetailsPanel.h"
#include "Helpers/MounteaInteractionSystemEditorLog.h"
#include "Interfaces/IHttpResponse.h"

#include "Interfaces/IMainFrameModule.h"
#include "Serialization/JsonReader.h"
#include "Settings/MounteaInteractionEditorSettings.h"

DEFINE_LOG_CATEGORY(ActorInteractionPluginEditor);

const FString ChangelogURL = FString("https://raw.githubusercontent.com/Mountea-Framework/MounteaInteractionSystem/5.1/CHANGELOG.md");

#define LOCTEXT_NAMESPACE "FActorInteractionPluginEditor"

static const FName MenuName("LevelEditor.LevelEditorToolBar.PlayToolBar");

void FActorInteractionPluginEditor::StartupModule()
{
	// Try to request Changelog from GitHub & GameplayTags
	{
		Http = &FHttpModule::Get();
		SendHTTPGet();
		SendHTTPGet_Tags();
	}

	// Register Category
	{
		FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(FName("MounteaInteraction"), FText::FromString(TEXT("\U0001F539 Mountea Interaction")));
	}

	// Thumbnails and Icons
	{
		InteractionSet = MakeShareable(new FSlateStyleSet("MounteaInteractionClassesSet"));

		const TSharedPtr<IPlugin> PluginPtr = IPluginManager::Get().FindPlugin("ActorInteractionPlugin");

		if (PluginPtr.IsValid())
		{
			const FString ContentDir = IPluginManager::Get().FindPlugin("ActorInteractionPlugin")->GetBaseDir();

			InteractionSet->SetContentRoot(ContentDir);

			// Interactor
			{
				FSlateImageBrush* InteractorComponentClassThumb = new FSlateImageBrush(InteractionSet->RootToContentDir(TEXT("Resources/InteractorIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* InteractorComponentClassIcon = new FSlateImageBrush(InteractionSet->RootToContentDir(TEXT("Resources/InteractorIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (InteractorComponentClassThumb && InteractorComponentClassIcon)
				{
					InteractionSet->Set("ClassThumbnail.ActorInteractorComponentBase", InteractorComponentClassThumb);
					InteractionSet->Set("ClassIcon.ActorInteractorComponentBase", InteractorComponentClassIcon);
				}
			}

			// Interactable
			{
				FSlateImageBrush* InteractableComponentClassThumb = new FSlateImageBrush(InteractionSet->RootToContentDir(TEXT("Resources/InteractableIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* InteractableComponentClassIcon = new FSlateImageBrush(InteractionSet->RootToContentDir(TEXT("Resources/InteractableIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (InteractableComponentClassThumb && InteractableComponentClassIcon)
				{
					InteractionSet->Set("ClassThumbnail.ActorInteractableComponentBase", InteractableComponentClassThumb);
					InteractionSet->Set("ClassIcon.ActorInteractableComponentBase", InteractableComponentClassIcon);
				}
			}
			
			// Config
			{
				FSlateImageBrush* InteractableComponentClassThumb = new FSlateImageBrush(InteractionSet->RootToContentDir(TEXT("Resources/InteractionConfigIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* InteractableComponentClassIcon = new FSlateImageBrush(InteractionSet->RootToContentDir(TEXT("Resources/InteractionConfigIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (InteractableComponentClassThumb && InteractableComponentClassIcon)
				{
					InteractionSet->Set("ClassThumbnail.MounteaInteractionSettingsConfig", InteractableComponentClassThumb);
					InteractionSet->Set("ClassIcon.MounteaInteractionSettingsConfig", InteractableComponentClassIcon);
				}
			}
			
			FSlateStyleRegistry::RegisterSlateStyle(*InteractionSet.Get());
		}
	}

	// Asset Types
	{
		// Register Interactor Component Base
		{
			InteractorComponentAssetActions = MakeShared<FInteractorComponentAssetActions>();
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InteractorComponentAssetActions.ToSharedRef());
		}

		// Register Interactable Component Base
		{
			InteractableComponentAssetActions = MakeShared<FInteractableComponentAssetActions>();
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InteractableComponentAssetActions.ToSharedRef());
		}

		// Register Interaction Config
		{
			InteractionConfigSettingsAssetAction = MakeShared<FInteractionSettingsConfigAssetActions>();
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InteractionConfigSettingsAssetAction.ToSharedRef());
		}
	}

	// Register pre-made Events
	{
		// New Interactable
		FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback
		(
			this,
			UActorInteractableComponentBase::StaticClass(),
			FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw(this, &FActorInteractionPluginEditor::HandleNewInteractableBlueprintCreated)
		);

		// New Interactor
		FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback
		(
			this,
			UActorInteractorComponentBase::StaticClass(),
			FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw(this, &FActorInteractionPluginEditor::HandleNewInteractorBlueprintCreated)
		);
	}

	// Register Custom Detail Panels
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		{
			TArray<FOnGetDetailCustomizationInstance> CustomClassLayouts =
			{
				FOnGetDetailCustomizationInstance::CreateStatic(&MounteaInteractableBase_DetailsPanel::MakeInstance),
			};
			RegisteredCustomClassLayouts =
			{
				UActorInteractableComponentBase::StaticClass()->GetFName(),
			};
			for (int32 i = 0; i < RegisteredCustomClassLayouts.Num(); i++)
			{
				PropertyModule.RegisterCustomClassLayout(RegisteredCustomClassLayouts[i], CustomClassLayouts[i]);
			}
		}
	}

	// Register Menu Button
	{
		FAIntPHelpStyle::Initialize();
		FAIntPHelpStyle::ReloadTextures();

		FAIntPCommands::Register();

		PluginCommands = MakeShareable(new FUICommandList);

		PluginCommands->MapAction(
			FAIntPCommands::Get().PluginAction,
			FExecuteAction::CreateRaw(this, &FActorInteractionPluginEditor::PluginButtonClicked), 
			FCanExecuteAction());

		IMainFrameModule& mainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
		mainFrame.GetMainFrameCommandBindings()->Append(PluginCommands.ToSharedRef());

		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FActorInteractionPluginEditor::RegisterMenus));
	}
}

void FActorInteractionPluginEditor::ShutdownModule()
{
	// Thumbnails and Icons
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(InteractionSet->GetStyleSetName());
	}

	// Asset Types Cleanup
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		{
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InteractorComponentAssetActions.ToSharedRef());
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InteractableComponentAssetActions.ToSharedRef());
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InteractionConfigSettingsAssetAction.ToSharedRef());
		}
	}

	// Help Button Cleanup
	{
		UToolMenus::UnRegisterStartupCallback(this);

		UToolMenus::UnregisterOwner(this);

		FAIntPHelpStyle::Shutdown();

		FAIntPCommands::Unregister();
	}

	UE_LOG(ActorInteractionPluginEditor, Warning, TEXT("ActorInteractionPluginEditor module has been unloaded"));
}

void FActorInteractionPluginEditor::HandleNewInteractorBlueprintCreated(UBlueprint* Blueprint)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return;
	}

	Blueprint->bForceFullEditor = true;
	UEdGraph* FunctionGraph = FActorInteractionEditorUtilities::BlueprintGetOrAddFunction
	(
		Blueprint,
		GET_FUNCTION_NAME_CHECKED(UActorInteractorComponentBase, CanInteract),
		UActorInteractorComponentBase::StaticClass()
	);
	if (FunctionGraph)
	{
		Blueprint->LastEditedDocuments.Add(FunctionGraph);
	}
	
	Blueprint->BlueprintCategory = FString("Mountea");
	Blueprint->BroadcastChanged();
}

void FActorInteractionPluginEditor::HandleNewInteractableBlueprintCreated(UBlueprint* Blueprint)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return;
	}

	Blueprint->bForceFullEditor = true;
	UEdGraph* FunctionGraph = FActorInteractionEditorUtilities::BlueprintGetOrAddFunction
	(
		Blueprint,
		GET_FUNCTION_NAME_CHECKED(UActorInteractableComponentBase, CanInteract),
		UActorInteractableComponentBase::StaticClass()
	);
	
	if (FunctionGraph)
	{
		Blueprint->LastEditedDocuments.Add(FunctionGraph);
	}

	Blueprint->BlueprintCategory = FString("Mountea");
	Blueprint->BroadcastChanged();
}

bool FActorInteractionPluginEditor::DoesHaveValidTags() const
{
	if (!GConfig) return false;
	
	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("ActorInteractionPlugin"))->GetBaseDir();
	const FString ConfigFilePath = PluginDirectory + "/Config/Tags/MounteaInteractionSystemTags.ini";
	FString NormalizedConfigFilePath = FConfigCacheIni::NormalizeConfigIniPath(ConfigFilePath);
	
	if (FPaths::FileExists(ConfigFilePath))
	{
		return GConfig->Find(NormalizedConfigFilePath) != nullptr;
	}
	
	return false;
}

void FActorInteractionPluginEditor::RefreshGameplayTags()
{
	TSharedPtr<IPlugin> ThisPlugin = IPluginManager::Get().FindPlugin(TEXT("ActorInteractionPlugin"));
	check(ThisPlugin.IsValid());
	
	UGameplayTagsManager::Get().EditorRefreshGameplayTagTree();
}

void FActorInteractionPluginEditor::UpdateTagsConfig(const FString& NewContent)
{
	if (!GConfig) return;

	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("ActorInteractionPlugin"))->GetBaseDir();
	const FString ConfigFilePath = PluginDirectory + "/Config/Tags/MounteaInteractionSystemTags.ini";

	FConfigFile* CurrentConfig = GConfig->Find(ConfigFilePath);

	FString CurrentContent;
	CurrentConfig->WriteToString(CurrentContent);

	TArray<FString> Lines;
	NewContent.ParseIntoArray(Lines, TEXT("\n"), true);

	TArray<FString> CleanedLines;
	for (FString& Itr : Lines)
	{
		if (Itr.Equals("[/Script/GameplayTags.GameplayTagsList]")) continue;

		if (Itr.Contains("GameplayTagList="))
		{
			FString NewValue = Itr.Replace(TEXT("GameplayTagList="), TEXT(""));

			CleanedLines.Add(NewValue);
		}
	}

	if (!CurrentContent.Equals(NewContent))
	{
		TArray<FString> CurrentLines;
		FConfigFile NewConfig;
		NewConfig.SetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CleanedLines);
		CurrentConfig->GetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CurrentLines);

		for (const FString& Itr : CleanedLines)
		{
			if (CurrentLines.Contains(Itr)) continue;

			CurrentLines.AddUnique(Itr);
		}

		CurrentConfig->SetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CurrentLines);
		CurrentConfig->Write(ConfigFilePath);

		RefreshGameplayTags();
	}
}

void FActorInteractionPluginEditor::CreateTagsConfig(const FString& NewContent)
{
	if (!GConfig) return;

	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("ActorInteractionPlugin"))->GetBaseDir();
	const FString ConfigFilePath = PluginDirectory + "/Config/Tags/MounteaInteractionSystemTags.ini";

	TArray<FString> Lines;
	NewContent.ParseIntoArray(Lines, TEXT("\n"), true);

	TArray<FString> CleanedLines;
	for (FString& Itr : Lines)
	{
		if (Itr.Equals("[/Script/GameplayTags.GameplayTagsList]")) continue;

		if (Itr.Contains("GameplayTagList="))
		{
			FString NewValue = Itr.Replace(TEXT("GameplayTagList="), TEXT(""));

			CleanedLines.Add(NewValue);
		}
	}
	
	FConfigFile NewConfig;
	NewConfig.SetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CleanedLines);
	NewConfig.Write(ConfigFilePath);
}

void FActorInteractionPluginEditor::PluginButtonClicked() const
{
	const FString URL = "https://discord.gg/waYT2cn37z"; // Interaction Specific Link

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
}

void FActorInteractionPluginEditor::WikiButtonClicked() const
{
	const FString URL = "https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/How-to-Setup-Interaction";

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
}

void FActorInteractionPluginEditor::DialoguerButtonClicked() const
{
	const FString URL = "https://mountea-framework.github.io/MounteaDialoguer/";

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
}

void FActorInteractionPluginEditor::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	// Register in Window tab
	{
		if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Help"))
		{
			if (Menu->ContainsSection("MounteaFramework") == false)
			{
				FToolMenuSection& Section = Menu->FindOrAddSection("MounteaFramework");
				
				Section.InsertPosition.Position = EToolMenuInsertType::First;
				Section.Label = FText::FromString(TEXT("Mountea Framework"));
				
				FToolMenuEntry SupportEntry = Section.AddMenuEntryWithCommandList
				(
					FAIntPCommands::Get().PluginAction,
					PluginCommands,
					LOCTEXT("MounteaSystemEditor_SupportButton_Label", "Mountea Support"),
					LOCTEXT("MounteaSystemEditor_SupportButton_ToolTip", "🆘 Open Mountea Framework Support channel"),
					FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Help")
				);
				SupportEntry.Name = FName("MounteaFrameworkSupport");
			}
		}
	}

	// Register in Level Editor Toolbar
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(MenuName);
		{
			ToolbarMenu->RemoveSection("MounteaFramework"); // Cleanup
			FToolMenuEntry& Entry = ToolbarMenu->FindOrAddSection("MounteaFramework")
				.AddEntry(FToolMenuEntry::InitComboButton(
					"MounteaMenu",
					FUIAction(),
					FOnGetContent::CreateRaw(this, &FActorInteractionPluginEditor::MakeMounteaMenuWidget),
					LOCTEXT("MounteaMainMenu_Label", "Mountea Framework"),
					LOCTEXT("MounteaMainMenu_Tooltip", "📂 Open Mountea Framework menu.\n\n❔ Provides link to Documentation, Support Discord and Dialogue tool."),
					FSlateIcon(FAIntPHelpStyle::Get().GetStyleSetName(), "AIntPStyleSet.Dialoguer"),
					false,
					"MounteaMenu"
				));
			
			Entry.Label = LOCTEXT("MounteaFramework_Label", "Mountea Framework");
			Entry.Name = TEXT("MounteaMenu");
			Entry.StyleNameOverride = "CalloutToolbar";
			Entry.SetCommandList(PluginCommands);
		}
	}
}

TSharedRef<SWidget> FActorInteractionPluginEditor::MakeMounteaMenuWidget() const
{
	FMenuBuilder MenuBuilder(true, PluginCommands);

	MenuBuilder.BeginSection("MounteaMenu_Links", LOCTEXT("MounteaMenuOptions_Options", "Mountea Links"));
	{
		// Support Entry
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_SupportButton_Label", "Mountea Support"),
			LOCTEXT("MounteaSystemEditor_SupportButton_ToolTip", "🆘 Open Mountea Framework Support channel"),
			FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Help"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FActorInteractionPluginEditor::PluginButtonClicked)
			)
		);
		// Wiki Entry
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_WikiButton_Label", "Mountea Wiki"),
			LOCTEXT("MounteaSystemEditor_WikiButton_ToolTip", "📖 Open Mountea Framework Documentation"),
			FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Wiki"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FActorInteractionPluginEditor::WikiButtonClicked)
			)
		);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("MounteaMenu_Tools", LOCTEXT("MounteaMenuOptions_Tools", "Mountea Tools"));
	{
		// Dialoguer Entry
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_DialoguerButton_Label", "Mountea Dialoguer"),
			LOCTEXT("MounteaSystemEditor_DialoguerButton_ToolTip", "⛰ Open Mountea Dialoguer Standalone Tool\n\n❔ Mountea Dialoguer is a standalone tool created for Dialogue crafting. Mountea Dialogue System supports native import for `.mnteadlg` files."),
			FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Dialoguer"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FActorInteractionPluginEditor::DialoguerButtonClicked)
			)
		);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

void FActorInteractionPluginEditor::OnGetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString ResponseBody;
	if (Response.Get() == nullptr) return;
	
	if (Response.IsValid() && Response->GetResponseCode() == 200)
	{
		ResponseBody = Response->GetContentAsString();
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
	}

	// Register Popup even if we have no response, this way we can show at least something
	{
		AIntPPopup::Register(ResponseBody);
	}
}

void FActorInteractionPluginEditor::SendHTTPGet()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	
	Request->OnProcessRequestComplete().BindRaw(this, &FActorInteractionPluginEditor::OnGetResponse);
	Request->SetURL(ChangelogURL);

	Request->SetVerb("GET");
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "text");
	Request->ProcessRequest();
}

void FActorInteractionPluginEditor::OnGetResponse_Tags(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	
}

void FActorInteractionPluginEditor::SendHTTPGet_Tags()
{
	const UMounteaInteractionEditorSettings* Settings = GetDefault<UMounteaInteractionEditorSettings>();
	if (DoesHaveValidTags())
	{
		if (!Settings->AllowCheckTagUpdate())
		{
			return;
		}
	}
	
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	
	Request->OnProcessRequestComplete().BindRaw(this, &FActorInteractionPluginEditor::OnGetResponse_Tags);
	Request->SetURL(Settings->GetGameplayTagsURL());

	Request->SetVerb("GET");
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "text");
	Request->ProcessRequest();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FActorInteractionPluginEditor, ActorInteractionPluginEditor);