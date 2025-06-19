// All rights reserved Dominik Morse (Pavlicek) 2021

#include "ActorInteractionPluginEditor.h"

#include "Interfaces/IPluginManager.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"

#include "Modules/ModuleManager.h"

#include "AssetActions/InteractorComponentAssetActions.h"
#include "AssetActions/InteractableComponentAssetActions.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "FileHelpers.h"
#include "GameplayTagsManager.h"
#include "HttpModule.h"
#include "IContentBrowserSingleton.h"
#include "HelpButton/AIntPCommands.h"
#include "HelpButton/AIntPHelpStyle.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Popup/AIntPPopup.h"
#include "Utilities/ActorInteractionEditorUtilities.h"

#include "ToolMenus.h"
#include "AssetActions/InteractionSettingsConfig.h"
#include "DetailsPanel/MounteaInteractableBase_DetailsPanel.h"
#include "ISettingsModule.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "HelpButton/InteractionSystemTutorialPage.h"
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
		FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(FName("MounteaInteraction"), FText::FromString(TEXT("👉🏻 Mountea Interaction")));
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

	// Register Tab Spawner
	{
		RegisterTabSpawners(FGlobalTabmanager::Get());
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

void FActorInteractionPluginEditor::SettingsButtonClicked() const
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project",  TEXT("Mountea Framework"), TEXT("Mountea Interaction System"));
}

void FActorInteractionPluginEditor::EditorSettingsButtonClicked() const
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project",  TEXT("Mountea Framework"), TEXT("Mountea Interaction System (Editor)"));
}

void FActorInteractionPluginEditor::WikiButtonClicked() const
{
	const FString URL = "https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/How-to-Setup-Interaction";

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
}

void FActorInteractionPluginEditor::YoutubeButtonClicked() const
{
	const FString URL = "https://www.youtube.com/playlist?list=PLIU53wA8zZmgwxOt7-Z4RP65NiqecBZay";

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

void FActorInteractionPluginEditor::LauncherButtonClicked() const
{
	const FString URL = "https://github.com/Mountea-Framework/MounteaProjectLauncher";

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
}

void FActorInteractionPluginEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager)
{
	TabManager->RegisterTabSpawner("InteractionSystemTutorial", 
		FOnSpawnTab::CreateRaw(this, &FActorInteractionPluginEditor::OnSpawnInteractionSystemTutorialTab))
		.SetDisplayName(FText::FromString("Interaction System Tutorial"))
		.SetTooltipText(FText::FromString("Learn about the Mountea Interaction System"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "InputBindingEditor.OutputLog"));
}

TSharedRef<SDockTab> FActorInteractionPluginEditor::OnSpawnInteractionSystemTutorialTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(FText::FromString("Interaction System Tutorial"))
		[
			SNew(SInteractionSystemTutorialPage)
		];
}

void FActorInteractionPluginEditor::TutorialButtonClicked() const
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("InteractionSystemTutorial"));
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
			FToolMenuEntry& Entry = ToolbarMenu->FindOrAddSection("MounteaInteractionFramework")
				.AddEntry(FToolMenuEntry::InitComboButton(
					"MounteaMenu",
					FUIAction(),
					FOnGetContent::CreateRaw(this, &FActorInteractionPluginEditor::MakeMounteaMenuWidget),
					LOCTEXT("MounteaMainMenu_Label", "Mountea Interaction Menu"),
					LOCTEXT("MounteaMainMenu_Tooltip", "📂 Open Mountea Interaction Menu.\n\n❔ Provides link to Documentation, Support Discord and other Mountea Tools."),
					FSlateIcon(FAIntPHelpStyle::Get().GetStyleSetName(), "AIntPStyleSet.Interaction"),
					false,
					"MounteaMenu"
				));
			
			Entry.Label = LOCTEXT("MounteaMainMenu_Label", "Mountea Interaction Menu");
			Entry.Name = TEXT("MounteaMenu");
			Entry.StyleNameOverride = "CalloutToolbar";
			Entry.SetCommandList(PluginCommands);
		}
	}
}

TSharedRef<SWidget> FActorInteractionPluginEditor::MakeMounteaMenuWidget() const
{
	FMenuBuilder MenuBuilder(true, PluginCommands);
	MenuBuilder.BeginSection("MounteaMenu_Tools", LOCTEXT("MounteaMenuOptions_Tutorial", "Mountea Interaction Tutorial"));
	{
		MenuBuilder.AddMenuEntry(
		LOCTEXT("MounteaSystemEditor_TutorialButton_Label", "Interaction System Tutorial"),
		LOCTEXT("MounteaSystemEditor_TutorialButton_ToolTip", "📖 Open the Mountea Interaction System Tutorial"),
		FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Tutorial"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &FActorInteractionPluginEditor::TutorialButtonClicked)
			)
		);
		
		MenuBuilder.AddMenuEntry(
        		LOCTEXT("MounteaSystemEditor_OpenExampleLevel_Label", "Open Example Level"),
        		LOCTEXT("MounteaSystemEditor_OpenExampleLevel_ToolTip", "🌄 Opens an example level demonstrating Mountea Interaction System"),
        		FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Level"),
        		FUIAction(
        			FExecuteAction::CreateLambda([]()
        			{
        				const FString mapPath = TEXT("/ActorInteractionPlugin/Example/M_Example");
						if (FPackageName::DoesPackageExist(mapPath))
							FEditorFileUtils::LoadMap(mapPath, false, true);
        				else
        					EDITOR_LOG_ERROR(TEXT("Example map not found at:\nContent/Mountea/Maps/ExampleMap.umap"))
        			})
        		)
        	);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_OpenPluginFolder_Label", "Open Plugin Folder"),
			LOCTEXT("MounteaSystemEditor_OpenPluginFolder_ToolTip", "📂 Open the Mountea plugin's folder on disk"),
			FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Folder"),
			FUIAction(
				FExecuteAction::CreateLambda([]()
				{
					const FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
					TArray<FString> folderPaths;
					folderPaths.Add(TEXT("/ActorInteractionPlugin"));
					contentBrowserModule.Get().SetSelectedPaths(folderPaths, true);
				})
			)
		);
	};
	MenuBuilder.EndSection();
	
	MenuBuilder.BeginSection("MounteaMenu_Settings", LOCTEXT("MounteaMenuOptions_Settings", "Mountea Interaction Settings"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_SettingsButton_Label", "Mountea Interaction Settings"),
			LOCTEXT("MounteaSystemEditor_SettingsButton_ToolTip", "⚙ Open Mountea Interaction Settings\n\n❔ Configure core interaction system settings including default behaviors, input mappings, widget settings, and logging options. Customize the foundation of the interaction system here, including widget update frequency, interaction commands, and more."),
			FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Settings"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FActorInteractionPluginEditor::SettingsButtonClicked)
			)
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_EditorSettingsButton_Label", "Mountea Interaction Editor Settings"),
			LOCTEXT("MounteaSystemEditor_EditorSettingsButton_ToolTip", "⚙ Open Mountea Interaction Editor Settings\n\n❔ Customize your interaction editor experience with settings for:\n\n🏷️ Gameplay Tags: Set automatic gameplay tag checks and provide URLs for external resources.\n\nAll settings are saved in DefaultMounteaSettings.ini."),
			FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Settings"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FActorInteractionPluginEditor::EditorSettingsButtonClicked)
			)
		);
	}
	MenuBuilder.EndSection();
	
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
			LOCTEXT("MounteaSystemEditor_WikiButton_Label", "Mountea Interaction Wiki"),
			LOCTEXT("MounteaSystemEditor_WikiButton_ToolTip", "📖 Open Mountea Interaction Documentation"),
			FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Wiki"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FActorInteractionPluginEditor::WikiButtonClicked)
			)
		);
		// YouTube button
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_YoutubeButton_Label", "Mountea Interaction Youtube"),
			LOCTEXT("MounteaSystemEditor_YoutubeButton_ToolTip", "👁️ Watch Mountea Interaction Youtube Videos\n\n❔ Visual learning resources featuring step-by-step tutorials, implementation guides, and practical examples. Perfect for both beginners and advanced users looking to expand their knowledge through video content."),
			FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Youtube"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FActorInteractionPluginEditor::YoutubeButtonClicked)
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

		// Launcher Tool Entry
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_LauncherButton_Label", "Mountea Project Launcher"),
			LOCTEXT("MounteaSystemEditor_LauncherButton_ToolTip", "🚀 Open Mountea Project Launcher\n\n❔ Mountea Project Launcher is a standalone tool created for project launcher which can launch projects locally with multiple settings.\nUseful for testing."),
			FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Launcher"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FActorInteractionPluginEditor::LauncherButtonClicked)
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