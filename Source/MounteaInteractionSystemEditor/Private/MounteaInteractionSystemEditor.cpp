// All rights reserved Dominik Morse (Pavlicek) 2021

#include "MounteaInteractionSystemEditor.h"

#include "Interfaces/IPluginManager.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"

#include "Modules/ModuleManager.h"

#include "AssetActions/MounteaInteractorComponentAssetActions.h"
#include "AssetActions/MounteaInteractableComponentAssetActions.h"

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
#include "Utilities/MounteaInteractionEditorUtilities.h"

#include "ToolMenus.h"
#include "AssetActions/MounteaInteractionSettingsConfig.h"
#include "DetailsPanel/MounteaInteractableBase_DetailsPanel.h"
#include "ISettingsModule.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Consts/MounteaInteractionEditorConsts.h"
#include "HelpButton/MounteaInteractionSystemTutorialPage.h"
#include "Helpers/MounteaInteractionSystemEditorLog.h"
#include "Interfaces/IHttpResponse.h"

#include "Interfaces/IMainFrameModule.h"
#include "Serialization/JsonReader.h"
#include "Settings/MounteaInteractionEditorSettings.h"

DEFINE_LOG_CATEGORY(MounteaInteractionSystemEditor);

const FString ChangelogURL = FString("https://raw.githubusercontent.com/Mountea-Framework/MounteaInteractionSystem/master/CHANGELOG.md");

#define LOCTEXT_NAMESPACE "FMounteaInteractionSystemEditor"

static const FName MenuName("LevelEditor.LevelEditorToolBar.PlayToolBar");

void FMounteaInteractionSystemEditor::StartupModule()
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

		const TSharedPtr<IPlugin> PluginPtr = IPluginManager::Get().FindPlugin("MounteaInteractionSystem");

		if (PluginPtr.IsValid())
		{
			const FString ContentDir = IPluginManager::Get().FindPlugin("MounteaInteractionSystem")->GetBaseDir();

			InteractionSet->SetContentRoot(ContentDir);

			// Interactor
			{
				FSlateImageBrush* InteractorComponentClassThumb = new FSlateImageBrush(InteractionSet->RootToContentDir(TEXT("Resources/InteractorIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* InteractorComponentClassIcon = new FSlateImageBrush(InteractionSet->RootToContentDir(TEXT("Resources/InteractorIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (InteractorComponentClassThumb && InteractorComponentClassIcon)
				{
					InteractionSet->Set("ClassThumbnail.MounteaInteractorComponentBase", InteractorComponentClassThumb);
					InteractionSet->Set("ClassIcon.MounteaInteractorComponentBase", InteractorComponentClassIcon);
				}
			}

			// Interactable
			{
				FSlateImageBrush* InteractableComponentClassThumb = new FSlateImageBrush(InteractionSet->RootToContentDir(TEXT("Resources/InteractableIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* InteractableComponentClassIcon = new FSlateImageBrush(InteractionSet->RootToContentDir(TEXT("Resources/InteractableIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (InteractableComponentClassThumb && InteractableComponentClassIcon)
				{
					InteractionSet->Set("ClassThumbnail.MounteaInteractableComponentBase", InteractableComponentClassThumb);
					InteractionSet->Set("ClassIcon.MounteaInteractableComponentBase", InteractableComponentClassIcon);
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
			UMounteaInteractableComponentBase::StaticClass(),
			FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw(this, &FMounteaInteractionSystemEditor::HandleNewInteractableBlueprintCreated)
		);

		// New Interactor
		FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback
		(
			this,
			UMounteaInteractorComponentBase::StaticClass(),
			FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw(this, &FMounteaInteractionSystemEditor::HandleNewInteractorBlueprintCreated)
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
				UMounteaInteractableComponentBase::StaticClass()->GetFName(),
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
			FExecuteAction::CreateRaw(this, &FMounteaInteractionSystemEditor::PluginButtonClicked), 
			FCanExecuteAction());

		IMainFrameModule& mainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
		mainFrame.GetMainFrameCommandBindings()->Append(PluginCommands.ToSharedRef());

		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMounteaInteractionSystemEditor::RegisterMenus));
	}

	// Register Tab Spawner
	{
		RegisterTabSpawners(FGlobalTabmanager::Get());
	}
}

void FMounteaInteractionSystemEditor::ShutdownModule()
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

	UE_LOG(MounteaInteractionSystemEditor, Warning, TEXT("MounteaInteractionSystemEditor module has been unloaded"));
}

void FMounteaInteractionSystemEditor::HandleNewInteractorBlueprintCreated(UBlueprint* Blueprint)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return;
	}

	Blueprint->bForceFullEditor = true;
	UEdGraph* FunctionGraph = FMounteaInteractionEditorUtilities::BlueprintGetOrAddFunction
	(
		Blueprint,
		GET_FUNCTION_NAME_CHECKED(UMounteaInteractorComponentBase, CanInteract),
		UMounteaInteractorComponentBase::StaticClass()
	);
	if (FunctionGraph)
	{
		Blueprint->LastEditedDocuments.Add(FunctionGraph);
	}
	
	Blueprint->BlueprintCategory = FString("Mountea");
	Blueprint->BroadcastChanged();
}

void FMounteaInteractionSystemEditor::HandleNewInteractableBlueprintCreated(UBlueprint* Blueprint)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return;
	}

	Blueprint->bForceFullEditor = true;
	UEdGraph* FunctionGraph = FMounteaInteractionEditorUtilities::BlueprintGetOrAddFunction
	(
		Blueprint,
		GET_FUNCTION_NAME_CHECKED(UMounteaInteractableComponentBase, CanInteract),
		UMounteaInteractableComponentBase::StaticClass()
	);
	
	if (FunctionGraph)
	{
		Blueprint->LastEditedDocuments.Add(FunctionGraph);
	}

	Blueprint->BlueprintCategory = FString("Mountea");
	Blueprint->BroadcastChanged();
}

bool FMounteaInteractionSystemEditor::DoesHaveValidTags() const
{
	if (!GConfig) return false;
	
	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("MounteaInteractionSystem"))->GetBaseDir();
	const FString ConfigFilePath = PluginDirectory + "/Config/Tags/MounteaInteractionSystemTags.ini";
	FString NormalizedConfigFilePath = FConfigCacheIni::NormalizeConfigIniPath(ConfigFilePath);
	
	if (FPaths::FileExists(ConfigFilePath))
	{
		return GConfig->Find(NormalizedConfigFilePath) != nullptr;
	}
	
	return false;
}

void FMounteaInteractionSystemEditor::RefreshGameplayTags()
{
	TSharedPtr<IPlugin> ThisPlugin = IPluginManager::Get().FindPlugin(TEXT("MounteaInteractionSystem"));
	check(ThisPlugin.IsValid());
	
	UGameplayTagsManager::Get().EditorRefreshGameplayTagTree();
}

void FMounteaInteractionSystemEditor::UpdateTagsConfig(const FString& NewContent)
{
	if (!GConfig) return;

	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("MounteaInteractionSystem"))->GetBaseDir();
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

void FMounteaInteractionSystemEditor::CreateTagsConfig(const FString& NewContent)
{
	if (!GConfig) return;

	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("MounteaInteractionSystem"))->GetBaseDir();
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

void FMounteaInteractionSystemEditor::PluginButtonClicked() const
{
	const FString URL = "https://discord.gg/waYT2cn37z"; // Interaction Specific Link

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
}

void FMounteaInteractionSystemEditor::SettingsButtonClicked() const
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project",  TEXT("Mountea Framework"), TEXT("Mountea Interaction System"));
}

void FMounteaInteractionSystemEditor::EditorSettingsButtonClicked() const
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project",  TEXT("Mountea Framework"), TEXT("Mountea Interaction System (Editor)"));
}

void FMounteaInteractionSystemEditor::WikiButtonClicked() const
{
	const FString URL = "https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/How-to-Setup-Interaction";

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
}

void FMounteaInteractionSystemEditor::YoutubeButtonClicked() const
{
	const FString URL = "https://www.youtube.com/playlist?list=PLIU53wA8zZmgwxOt7-Z4RP65NiqecBZay";

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
}

void FMounteaInteractionSystemEditor::DialoguerButtonClicked() const
{
	const FString URL = "https://mountea-framework.github.io/MounteaDialoguer/";

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
}

void FMounteaInteractionSystemEditor::LauncherButtonClicked() const
{
	const FString URL = "https://github.com/Mountea-Framework/MounteaProjectLauncher";

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
}

void FMounteaInteractionSystemEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager)
{
	TabManager->RegisterTabSpawner("InteractionSystemTutorial", 
		FOnSpawnTab::CreateRaw(this, &FMounteaInteractionSystemEditor::OnSpawnInteractionSystemTutorialTab))
		.SetDisplayName(FText::FromString("Interaction System Tutorial"))
		.SetTooltipText(FText::FromString("Learn about the Mountea Interaction System"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "InputBindingEditor.OutputLog"));
}

TSharedRef<SDockTab> FMounteaInteractionSystemEditor::OnSpawnInteractionSystemTutorialTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(FText::FromString("Interaction System Tutorial"))
		[
			SNew(SInteractionSystemTutorialPage)
		];
}

void FMounteaInteractionSystemEditor::TutorialButtonClicked() const
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("InteractionSystemTutorial"));
}

void FMounteaInteractionSystemEditor::RegisterMenus()
{
	if (!UToolMenus::Get()->IsMenuRegistered(MounteaAdvancedDialogueToolbar::MounteaSharedMenuName))
		UToolMenus::Get()->RegisterMenu(MounteaAdvancedDialogueToolbar::MounteaSharedMenuName, NAME_None, EMultiBoxType::Menu, false);

	{
		UToolMenu* toolbarMenu = UToolMenus::Get()->ExtendMenu(MenuName);
		FToolMenuSection& sharedSection = toolbarMenu->FindOrAddSection(MounteaAdvancedDialogueToolbar::MounteaSharedSectionName);
		sharedSection.Label = LOCTEXT("SharedSection_Label", "Mountea Framework");

		if (sharedSection.FindEntry(MounteaAdvancedDialogueToolbar::MounteaSharedButtonName) == nullptr)
		{
			FToolMenuEntry comboButton = FToolMenuEntry::InitComboButton(
				MounteaAdvancedDialogueToolbar::MounteaSharedButtonName,
				FUIAction(),
				FOnGetContent::CreateLambda([SharedMenuName = MounteaAdvancedDialogueToolbar::MounteaSharedMenuName, CmdList = PluginCommands]() -> TSharedRef<SWidget>
				{
					return UToolMenus::Get()->GenerateWidget(SharedMenuName, FToolMenuContext(CmdList));
				}),
				LOCTEXT("MounteaMainMenu_Label", "Mountea Framework"),
				LOCTEXT("MounteaMainMenu_Tooltip", "📂 Open Mountea Framework menu..."),
				FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.MounteaLogo"),
				false,
				MounteaAdvancedDialogueToolbar::MounteaSharedButtonName
			);
			comboButton.StyleNameOverride = "CalloutToolbar";
			sharedSection.AddEntry(comboButton);
		}
	}

	// Phase C: Owner-scoped — cleaned up automatically on ShutdownModule
	FToolMenuOwnerScoped OwnerScoped(this);
	{
		// Help menu entry
		if (UToolMenu* toolMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Help"))
		{
			FToolMenuSection& mounteaSection = toolMenu->FindOrAddSection("MounteaFramework");
			mounteaSection.InsertPosition.Position = EToolMenuInsertType::First;
			mounteaSection.Label = FText::FromString(TEXT("Mountea Framework"));
			FToolMenuEntry supportEntry = mounteaSection.AddMenuEntryWithCommandList(
				FAIntPCommands::Get().PluginAction,
				PluginCommands,
				LOCTEXT("MounteaSystemEditor_SupportButton_Label", "Mountea Support"),
				LOCTEXT("MounteaSystemEditor_SupportButton_ToolTip", "🆘 Open Mountea Framework Support channel"),
				FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Help")
			);
			supportEntry.Name = FName("MounteaFrameworkSupport");
		}

		// Extend shared submenu with a nested Interaction submenu entry
		UToolMenu* sharedMenu = UToolMenus::Get()->ExtendMenu(MounteaAdvancedDialogueToolbar::MounteaSharedMenuName);
		{
			FToolMenuSection& interactionSection = sharedMenu->FindOrAddSection("MounteaInteraction");
			interactionSection.Label = LOCTEXT("InteractionSection_Label", "Mountea Interaction System");
			interactionSection.AddEntry(FToolMenuEntry::InitSubMenu(
				"MounteaInteractionSubMenu",
				LOCTEXT("InteractionSubMenu_Label", "Mountea Interaction"),
				LOCTEXT("InteractionSubMenu_Tooltip", "🖱️ Mountea Interaction System tools"),
				FNewToolMenuDelegate::CreateLambda([this](UToolMenu* subMenu)
				{
					{
						FToolMenuSection& tutorialSection = subMenu->FindOrAddSection("MounteaInteraction_Tutorial");
						tutorialSection.Label = LOCTEXT("InteractionTutorial_Label", "Mountea Interaction Tutorial");
						tutorialSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInteraction_Tutorial",
							LOCTEXT("MounteaSystemEditor_TutorialButton_Label", "Interaction System Tutorial"),
							LOCTEXT("MounteaSystemEditor_TutorialButton_ToolTip", "📖 Open the Mountea Interaction System Tutorial"),
							FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Tutorial"),
							FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext&) { TutorialButtonClicked(); })
						));
						tutorialSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInteraction_ExampleLevel",
							LOCTEXT("MounteaSystemEditor_OpenExampleLevel_Label", "Open Example Level"),
							LOCTEXT("MounteaSystemEditor_OpenExampleLevel_ToolTip", "🌄 Opens an example level demonstrating Mountea Interaction System"),
							FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Level"),
							FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
							{
								const FString mapPath = TEXT("/MounteaInteractionSystem/Example/M_Example");
								if (FPackageName::DoesPackageExist(mapPath))
									FEditorFileUtils::LoadMap(mapPath, false, true);
								else
									EDITOR_LOG_ERROR(TEXT("Example map not found at:\nContent/Mountea/Maps/ExampleMap.umap"))
							})
						));
						tutorialSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInteraction_PluginFolder",
							LOCTEXT("MounteaSystemEditor_OpenPluginFolder_Label", "Open Plugin Folder"),
							LOCTEXT("MounteaSystemEditor_OpenPluginFolder_ToolTip", "📂 Open the Mountea plugin's folder on disk"),
							FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Folder"),
							FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
							{
								const FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
								TArray<FString> folderPaths;
								folderPaths.Add(TEXT("/MounteaInteractionSystem"));
								contentBrowserModule.Get().SetSelectedPaths(folderPaths, true);
							})
						));
					}

					{
						FToolMenuSection& settingsSection = subMenu->FindOrAddSection("MounteaInteraction_Settings");
						settingsSection.Label = LOCTEXT("InteractionSettings_Label", "Mountea Interaction Settings");
						settingsSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInteraction_Settings",
							LOCTEXT("MounteaSystemEditor_SettingsButton_Label", "Mountea Interaction Settings"),
							LOCTEXT("MounteaSystemEditor_SettingsButton_ToolTip", "⚙ Open Mountea Interaction Settings\n\n❔ Configure core interaction system settings including default behaviors, input mappings, widget settings, and logging options. Customize the foundation of the interaction system here, including widget update frequency, interaction commands, and more."),
							FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Settings"),
							FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext&) { SettingsButtonClicked(); })
						));
						settingsSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInteraction_EditorSettings",
							LOCTEXT("MounteaSystemEditor_EditorSettingsButton_Label", "Mountea Interaction Editor Settings"),
							LOCTEXT("MounteaSystemEditor_EditorSettingsButton_ToolTip", "⚙ Open Mountea Interaction Editor Settings\n\n❔ Customize your interaction editor experience with settings for:\n\n🏷️ Gameplay Tags: Set automatic gameplay tag checks and provide URLs for external resources.\n\nAll settings are saved in DefaultMounteaSettings.ini."),
							FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Settings"),
							FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext&) { EditorSettingsButtonClicked(); })
						));
					}

					{
						FToolMenuSection& linksSection = subMenu->FindOrAddSection("MounteaInteraction_Links");
						linksSection.Label = LOCTEXT("InteractionLinks_Label", "Mountea Interaction Links");
						linksSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInteraction_Support",
							LOCTEXT("MounteaSystemEditor_SupportButton_Label", "Mountea Support"),
							LOCTEXT("MounteaSystemEditor_SupportButton_ToolTip", "🆘 Open Mountea Framework Support Channel\n\n❔ Get direct assistance from our support team and community. Find solutions to common issues, share your experiences, and get help with implementation challenges. Join our active community of developers!"),
							FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Help"),
							FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext&) { PluginButtonClicked(); })
						));
						linksSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInteraction_Wiki",
							LOCTEXT("MounteaSystemEditor_WikiButton_Label", "Mountea Interaction Wiki"),
							LOCTEXT("MounteaSystemEditor_WikiButton_ToolTip", "📖 Open Mountea Interaction Documentation"),
							FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Wiki"),
							FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext&) { WikiButtonClicked(); })
						));
						linksSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInteraction_Youtube",
							LOCTEXT("MounteaSystemEditor_YoutubeButton_Label", "Mountea Interaction Youtube"),
							LOCTEXT("MounteaSystemEditor_YoutubeButton_ToolTip", "👁️ Watch Mountea Interaction Youtube Videos\n\n❔ Visual learning resources featuring step-by-step tutorials, implementation guides, and practical examples. Perfect for both beginners and advanced users looking to expand their knowledge through video content."),
							FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Youtube"),
							FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext&) { YoutubeButtonClicked(); })
						));
					}
				}),
				false,
				FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Interaction")
			));
		}

		// Shared Tools section — always last; FindEntry guards prevent duplicates when multiple plugins are loaded
		{
			FToolMenuSection& toolsSection = sharedMenu->FindOrAddSection("MounteaFramework_Tools");
			toolsSection.InsertPosition = FToolMenuInsert(NAME_None, EToolMenuInsertType::Last);
			toolsSection.Label = LOCTEXT("Tools_Label", "Mountea Tools");

			if (toolsSection.FindEntry("MounteaFramework_Dialoguer") == nullptr)
				toolsSection.AddEntry(FToolMenuEntry::InitMenuEntry(
					"MounteaFramework_Dialoguer",
					LOCTEXT("MounteaSystemEditor_DialoguerButton_Label", "Mountea Dialoguer"),
					LOCTEXT("MounteaSystemEditor_DialoguerButton_ToolTip", "⛰ Open Mountea Dialoguer Standalone Tool\n\n❔ A powerful standalone dialogue crafting tool designed for narrative designers and writers. Create, edit, and manage complex dialogue trees with an intuitive interface. Seamlessly import your `.mnteadlg` files directly into the Mountea Dialogue System.\n\n💡 Perfect for teams wanting to separate dialogue content creation from engine implementation."),
					FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Dialoguer"),
					FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext&) { DialoguerButtonClicked(); })
				));

			if (toolsSection.FindEntry("MounteaFramework_Launcher") == nullptr)
				toolsSection.AddEntry(FToolMenuEntry::InitMenuEntry(
					"MounteaFramework_Launcher",
					LOCTEXT("MounteaSystemEditor_LauncherButton_Label", "Mountea Project Launcher"),
					LOCTEXT("MounteaSystemEditor_LauncherButton_ToolTip", "🚀 Open Mountea Project Launcher\n\n❔ A versatile standalone tool for streamlined project testing and deployment. Launch your projects with customized configurations, test different build settings, and validate implementations in various environments.\n\n💡 Features include:\n- Multiple configuration profiles\n- Quick-launch presets\n- Custom command-line parameters\n- Integrated testing tools"),
					FSlateIcon(FAIntPHelpStyle::GetStyleSetName(), "AIntPStyleSet.Launcher"),
					FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext&) { LauncherButtonClicked(); })
				));
		}
	}
}

void FMounteaInteractionSystemEditor::OnGetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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

void FMounteaInteractionSystemEditor::SendHTTPGet()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	
	Request->OnProcessRequestComplete().BindRaw(this, &FMounteaInteractionSystemEditor::OnGetResponse);
	Request->SetURL(ChangelogURL);

	Request->SetVerb("GET");
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "text");
	Request->ProcessRequest();
}

void FMounteaInteractionSystemEditor::OnGetResponse_Tags(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	
}

void FMounteaInteractionSystemEditor::SendHTTPGet_Tags()
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
	
	Request->OnProcessRequestComplete().BindRaw(this, &FMounteaInteractionSystemEditor::OnGetResponse_Tags);
	Request->SetURL(Settings->GetGameplayTagsURL());

	Request->SetVerb("GET");
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "text");
	Request->ProcessRequest();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMounteaInteractionSystemEditor, MounteaInteractionSystemEditor);