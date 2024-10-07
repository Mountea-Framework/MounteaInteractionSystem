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

	UPROPERTY(config, EditDefaultsOnly, Category = "GameplayTags")
	uint8 bAllowAutoGameplayTagsCheck : 1;

	UPROPERTY(config, EditDefaultsOnly, Category = "GameplayTags", AdvancedDisplay=true)
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
