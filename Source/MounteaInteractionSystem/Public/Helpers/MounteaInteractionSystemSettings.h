// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MounteaInteractionGeneralDataTypes.h"

#include "MounteaInteractionSystemSettings.generated.h"

class UMounteaInteractorRuntimeConfig;
class UMounteaInteractorUIConfig;
class UMounteaInteractableRuntimeConfig;
class UMounteaInteractableUIConfig;

/**
 * Mountea Interaction System project-wide settings.
 * Lives under Project Settings > Mountea Framework > Mountea Interaction System.
 * Each config slot points to a DataAsset for its domain.
 */
UCLASS(config=MounteaSettings, defaultconfig, meta=(DisplayName="Mountea Interaction System Settings"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractionSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UMounteaInteractionSystemSettings();

public:

	/** Runtime gameplay defaults for all interactor components (state, collision channel, tag, safety trace, candidate settings). */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Interactor")
	TSoftObjectPtr<UMounteaInteractorRuntimeConfig> InteractorConfig;

	/** UI defaults for interactors — controls the attention widget pool size, widget class, and fade timings. */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Interactor")
	TSoftObjectPtr<UMounteaInteractorUIConfig> InteractorUIConfig;

	/** Runtime gameplay defaults for all interactable components (period, state, lifecycle, highlight, weight, max interactors, ranges). */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Interactable")
	TSoftObjectPtr<UMounteaInteractableRuntimeConfig> InteractableConfig;

	/** UI defaults for interactables — interaction widget class, DataTable, key-texture mappings, update frequency. */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Interactable")
	TSoftObjectPtr<UMounteaInteractableUIConfig> InteractableUIConfig;

	/** Enables editor-only debug notifications when interactable/interactor properties change. */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Editor")
	uint8 bEditorDebugEnabled : 1;

	/**
	 * Logging verbosity bitmask.
	 * Controls which message levels are emitted to on-screen and log.
	 */
	UPROPERTY(config, EditDefaultsOnly, Category="Logging", meta=(Bitmask, BitmaskEnum="/Script/MounteaInteractionSystem.EMounteaInteractionLoggingVerbosity"))
	uint8 LogVerbosity;

public:

	bool IsEditorDebugEnabled() const { return bEditorDebugEnabled; }
	EMounteaInteractionLoggingVerbosity GetAllowedLoggVerbosity() const;

	const UMounteaInteractorRuntimeConfig* GetInteractorConfig() const;
	const UMounteaInteractorUIConfig* GetInteractorUIConfig() const;
	const UMounteaInteractableRuntimeConfig* GetInteractableConfig() const;
	const UMounteaInteractableUIConfig* GetInteractableUIConfig() const;

#if WITH_EDITOR
	virtual FText GetSectionText() const override
	{
		return NSLOCTEXT("MounteaInteractionSystem", "MounteaSettingsSectionText", "Mountea Interaction System");
	}

	virtual FText GetSectionDescription() const override
	{
		return NSLOCTEXT("MounteaInteractionSystem", "MounteaSettingsSectionDesc", "Default values for Mountea Interaction System.");
	}

	virtual FName GetContainerName() const override { return TEXT("Project"); }
#endif
};
