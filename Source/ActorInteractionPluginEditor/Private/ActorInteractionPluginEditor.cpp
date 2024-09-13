// All rights reserved Dominik Morse (Pavlicek) 2021

#include "ActorInteractionPluginEditor.h"

#include "Interfaces/IPluginManager.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"

#include "Modules/ModuleManager.h"

#include "AssetActions/InteractorComponentAssetActions.h"
#include "AssetActions/InteractableComponentAssetActions.h"

#include "AssetToolsModule.h"
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

DEFINE_LOG_CATEGORY(ActorInteractionPluginEditor);

static const FName AIntPHelpTabName("MounteaFramework");

const FString ChangelogURL = FString("https://raw.githubusercontent.com/Mountea-Framework/MounteaInteractionSystem/5.1/CHANGELOG.md");

#define LOCTEXT_NAMESPACE "FActorInteractionPluginEditor"

void FActorInteractionPluginEditor::StartupModule()
{
	// Try to request Changelog from GitHub
	{
		Http = &FHttpModule::Get();
		SendHTTPGet();
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

	// Register Help Button
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

void FActorInteractionPluginEditor::PluginButtonClicked()
{
	const FString URL = "https://discord.gg/waYT2cn37z"; // Interaction Specific Link

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
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Help");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("MounteaFramework");
			Section.Label = FText::FromString(TEXT("Mountea Framework"));
						
			FToolMenuEntry Entry = Section.AddMenuEntryWithCommandList
			(
				FAIntPCommands::Get().PluginAction,
				PluginCommands,
				NSLOCTEXT("MounteaSupport", "TabTitle", "Mountea Support"),
				NSLOCTEXT("MounteaSupport", "TooltipText", "Opens Mountea Framework Support channel"),
				FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPSupport.PluginAction.small")
			);
		}
	}

	// Register in Level Editor Toolbar
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("MounteaFramework");
			{
				Section.Label = FText::FromString(TEXT("Mountea Framework"));
				
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FAIntPCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
				
				Entry.InsertPosition.Position = EToolMenuInsertType::First;
			}
		}
	}
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

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FActorInteractionPluginEditor, ActorInteractionPluginEditor);