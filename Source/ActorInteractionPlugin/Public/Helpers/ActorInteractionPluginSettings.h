// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "ActorInteractionPluginSettings.generated.h"

class UMounteaInteractionSettingsConfig;
class UInputMappingContext;
class UDataTable;
class UMaterialInterface;
class UUserWidget;

/**
 * Mountea Interaction System global settings.
 */
UCLASS(config = MounteaSettings, defaultconfig, meta = (DisplayName = "Mountea Interaction System Settings"))
class ACTORINTERACTIONPLUGIN_API UActorInteractionPluginSettings : public UDeveloperSettings
{
	
  GENERATED_BODY()

	UActorInteractionPluginSettings();

public:

	UPROPERTY(config, BlueprintReadOnly, EditAnywhere, Category = "Interactor")
	TSoftObjectPtr<UMounteaInteractionSettingsConfig>		DefaultInteractionSystemConfig;

	/** Defines whether in-editor debug is enabled. */
	UPROPERTY(config, BlueprintReadOnly, EditAnywhere, Category="Editor")
	uint8															bEditorDebugEnabled : 1;

	/** Defines how often is the Interaction widget updated per second.*/
	UPROPERTY(config, BlueprintReadOnly, EditAnywhere, Category = "Widgets", meta=(Units="s", UIMin=0.001, ClampMin=0.001))
	float																WidgetUpdateFrequency =					0.05f;

	/** Defines default Interactable Widget class.*/
	UPROPERTY(config, BlueprintReadOnly, EditAnywhere, Category = "Widgets", meta=(AllowedClasses="/Script/UMG.UserWidget", MustImplement="/Script/ActorInteractionPlugin.ActorInteractionWidget"))
	TSoftClassPtr<UUserWidget>						InteractableDefaultWidgetClass;
	
	/** Defines default DataTable which contains Interactable data values.*/
	UPROPERTY(config, BlueprintReadOnly, EditAnywhere, Category = "Interaction Data", meta=(AllowedClasses = "/Script/Engine.DataTable"))
	TSoftObjectPtr<UDataTable>						InteractableDefaultDataTable;
		
	/** Defines default Interaction Mapping for Mountea Interaction System. */
	UPROPERTY(config, BlueprintReadOnly, EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputMappingContext>		InteractionInputMapping;

	/** Defines default Interaction Commands. Serves purpose of containing default commands. */
	TSet<FString>												InteractionWidgetCommands;
	
#if WITH_EDITOR
	virtual FText GetSectionText() const override
	{
		return NSLOCTEXT("ActorInteractionPlugin", "MounteaSettingsDescription", "Mountea Interaction System");
	}

	virtual FText GetSectionDescription() const override
	{
		return NSLOCTEXT("ActorInteractionPlugin", "MounteaSettingsDescription", "Default values for Mountea Plugins.");
	}

	virtual FName GetContainerName() const override
	{
		return "Project";
	}
#endif
	
public:

	bool IsEditorDebugEnabled() const
	{ return bEditorDebugEnabled; };

	float GetWidgetUpdateFrequency() const
	{ return WidgetUpdateFrequency; }

	TSoftObjectPtr<UDataTable> GetInteractableDefaultDataTable() const
	{ return InteractableDefaultDataTable; };

	TSoftClassPtr<UUserWidget> GetInteractableDefaultWidgetClass() const;

	UMaterialInterface* GetDefaultHighlightMaterial() const;

	UInputMappingContext* GetDefaultInputMappingContext() const;
};
