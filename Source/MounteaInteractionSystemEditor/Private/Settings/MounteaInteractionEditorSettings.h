// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MounteaInteractionEditorSettings.generated.h"

USTRUCT(Blueprintable)
struct FInteractionEditorPageConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText PageTitle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		meta = (FilePathFilter = "html"))
	FFilePath PageFile;

	FInteractionEditorPageConfig()
	{}

	FInteractionEditorPageConfig(FText InTitle, FString InPath)
		: PageTitle(InTitle)
	{
		PageFile.FilePath = InPath;
	}
};

/**
 * Mountea Interaction System global settings.
 */
UCLASS(config = MounteaSettings, DefaultConfig, ProjectUserConfig)
class MOUNTEAINTERACTIONSYSTEMEDITOR_API UMounteaInteractionEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UMounteaInteractionEditorSettings();

private:

#pragma region GameplayTags

	UPROPERTY(config, EditDefaultsOnly, Category = "GameplayTags", meta=(ConfigRestartRequired=true))
	uint8 bAllowAutoGameplayTagsCheck : 1;

	UPROPERTY(config, EditDefaultsOnly, Category = "GameplayTags", AdvancedDisplay, meta=(ConfigRestartRequired=true))
	FString GameplayTagsURL = FString("https://raw.githubusercontent.com/Mountea-Framework/MounteaInteractionSystem/master/Config/Tags/MounteaInteractionSystemTags.ini");

#pragma endregion

#pragma region EditorTemplates

	/** Shared stylesheet injected into every tutorial HTML page. */
	UPROPERTY(Config, EditAnywhere, Category = "Editor Templates",
		AdvancedDisplay, meta = (FilePathFilter = "css"))
	FFilePath SharedStylesheetPath;

	/** Shared script injected into every tutorial HTML page. */
	UPROPERTY(Config, EditAnywhere, Category = "Editor Templates",
		AdvancedDisplay, meta = (FilePathFilter = "js"))
	FFilePath SharedScriptPath;

	/**
	 * Defines the list of tutorial pages available in the Interaction System Tutorial tab.
	 * You can extend these for your team as needed.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Editor Templates",
		AdvancedDisplay)
	TMap<int32, FInteractionEditorPageConfig> EditorTemplatePages;

#pragma endregion

public:

#pragma region Getters

	bool AllowCheckTagUpdate() const
	{ return bAllowAutoGameplayTagsCheck; }

	FString GetGameplayTagsURL() const
	{ return GameplayTagsURL; }

	FString GetEditorTemplatePagePath(int32 PageId) const;
	FString GetSharedStylesheetPath() const;
	FString GetSharedScriptPath() const;
	FString GetGeneratedChangelogPath() const;

	const TMap<int32, FInteractionEditorPageConfig>& GetEditorTemplatePages() const
	{ return EditorTemplatePages; }

#pragma endregion

#if WITH_EDITOR
	virtual FText GetSectionText() const override
	{
		return NSLOCTEXT("MounteaInteractionEditorSystem", "MounteaInteractionSettingsEditorSection", "Mountea Interaction System (Editor)");
	}

	virtual FText GetSectionDescription() const override
	{
		return NSLOCTEXT("MounteaInteractionEditorSystem", "MounteaInteractionSettingsEditorDescription", "Default values for Mountea Plugins (Editor).");
	}

	virtual FName GetContainerName() const override
	{
		return "Project";
	}
#endif

	// Allow the tutorial page widget to read private page paths
	friend class SInteractionSystemTutorialPage;
};
