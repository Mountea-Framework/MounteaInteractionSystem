// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MounteaInteractionEditorSettings.generated.h"

/**
 * Mountea Interaction System global settings.
 */
UCLASS(config = MounteaSettings, DefaultConfig, ProjectUserConfig)
class ACTORINTERACTIONPLUGINEDITOR_API UMounteaInteractionEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UMounteaInteractionEditorSettings();

private:

#pragma region GameplayTags
	
	/**
	 * Allow automatic gameplay tag checks.
	 * 
	 * If set to true, the system will automatically verify and update gameplay tags on engine startup.
	 * Default is True.
	 */
	UPROPERTY(config, EditDefaultsOnly, Category = "GameplayTags", meta=(ConfigRestartRequired=true))
	uint8 bAllowAutoGameplayTagsCheck : 1;

	/**
	 * URL for the Gameplay Tags configuration file.
	 * 
	 * This URL points to a remote file containing base gameplay tag definitions.
	 * The system will use this URL to download and apply the tags if allowed.
	 * Default: @link https://raw.githubusercontent.com/Mountea-Framework/MounteaInteractionSystem/master/Config/Tags/MounteaInteractionSystemTags.ini
	 */
	UPROPERTY(config, EditDefaultsOnly, Category = "GameplayTags", AdvancedDisplay=true, meta=(ConfigRestartRequired=true))
	FString GameplayTagsURL = FString("https://raw.githubusercontent.com/Mountea-Framework/MounteaInteractionSystem/master/Config/Tags/MounteaInteractionSystemTags.ini");
	
#pragma endregion

public:

#pragma region GameplayTags_Getters

	bool AllowCheckTagUpdate() const
	{ return bAllowAutoGameplayTagsCheck; };

	FString GetGameplayTagsURL() const
	{ return GameplayTagsURL; };
	
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
};
