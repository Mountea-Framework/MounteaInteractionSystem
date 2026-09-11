// All rights reserved Dominik Morse (Pavlicek) 2024

#include "MounteaInteractionEditorSettings.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionEditorSettings"

UMounteaInteractionEditorSettings::UMounteaInteractionEditorSettings() : bAllowAutoGameplayTagsCheck(true)
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Interaction System (Editor)");

	auto ResolvePluginPath = [](const FString& RelativePath) -> FString
	{
		const TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin(TEXT("MounteaInteractionSystem"));
		if (!plugin.IsValid())
			return FString();
		return FPaths::ConvertRelativePathToFull(FPaths::Combine(plugin->GetBaseDir(), RelativePath));
	};

	SharedStylesheetPath.FilePath = ResolvePluginPath(TEXT("Resources/Help/MounteaInteractionHelp.css"));
	SharedScriptPath.FilePath     = ResolvePluginPath(TEXT("Resources/Help/MounteaInteractionHelp.js"));

	EditorTemplatePages.Add(0, FInteractionEditorPageConfig(
		NSLOCTEXT("MounteaInteractionEditorSettings", "Page0_Title", "Overview"),
		ResolvePluginPath(TEXT("Resources/Help/page_0.html"))
	));
	EditorTemplatePages.Add(1, FInteractionEditorPageConfig(
		NSLOCTEXT("MounteaInteractionEditorSettings", "Page1_Title", "Config Assets"),
		ResolvePluginPath(TEXT("Resources/Help/page_1.html"))
	));
	EditorTemplatePages.Add(2, FInteractionEditorPageConfig(
		NSLOCTEXT("MounteaInteractionEditorSettings", "Page2_Title", "Interactor Setup"),
		ResolvePluginPath(TEXT("Resources/Help/page_2.html"))
	));
	EditorTemplatePages.Add(3, FInteractionEditorPageConfig(
		NSLOCTEXT("MounteaInteractionEditorSettings", "Page3_Title", "Interactable Setup"),
		ResolvePluginPath(TEXT("Resources/Help/page_3.html"))
	));
	EditorTemplatePages.Add(4, FInteractionEditorPageConfig(
		NSLOCTEXT("MounteaInteractionEditorSettings", "Page4_Title", "UMG Widget Setup"),
		ResolvePluginPath(TEXT("Resources/Help/page_4.html"))
	));
	EditorTemplatePages.Add(5, FInteractionEditorPageConfig(
		NSLOCTEXT("MounteaInteractionEditorSettings", "Page5_Title", "Testing & Troubleshooting"),
		ResolvePluginPath(TEXT("Resources/Help/page_5.html"))
	));
}

FString UMounteaInteractionEditorSettings::GetEditorTemplatePagePath(const int32 PageId) const
{
	const FInteractionEditorPageConfig* found = EditorTemplatePages.Find(PageId);
	if (!found)
		return FString();
	return FPaths::ConvertRelativePathToFull(found->PageFile.FilePath);
}

FString UMounteaInteractionEditorSettings::GetSharedStylesheetPath() const
{
	return FPaths::ConvertRelativePathToFull(SharedStylesheetPath.FilePath);
}

FString UMounteaInteractionEditorSettings::GetSharedScriptPath() const
{
	return FPaths::ConvertRelativePathToFull(SharedScriptPath.FilePath);
}

FString UMounteaInteractionEditorSettings::GetGeneratedChangelogPath() const
{
	const TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin(TEXT("MounteaInteractionSystem"));
	if (!plugin.IsValid())
		return FString();
	return FPaths::ConvertRelativePathToFull(plugin->GetBaseDir() / TEXT("Resources/Help/popup_changelog.html"));
}

#undef LOCTEXT_NAMESPACE
