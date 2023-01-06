// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ActorInteractionPluginSettings.generated.h"

class UDataTable;
class UUserWidget;

/**
 * Actor Interaction Plugin global settings.
 */
UCLASS(config = MounteaSettings, meta = (DisplayName = "Actor Interaction Plugin Settings"))
class ACTORINTERACTIONPLUGIN_API UActorInteractionPluginSettings : public UDeveloperSettings
{
	
  GENERATED_BODY()

	UActorInteractionPluginSettings()
	{
		CategoryName = TEXT("Mountea Framework");
		SectionName = TEXT("Actor Interaction Plugin");
	}

	/* Defines whether in-editor debug is enabled. */
	UPROPERTY(config, EditAnywhere, Category="Editor")
	uint8 bEditorDebugEnabled : 1;

	/* Defines how often is the Interaction widget updated per second.*/
	UPROPERTY(config, EditAnywhere, Category = "Widgets", meta=(Units="s", UIMin=0.001, ClampMin=0.001, ConfigRestartRequired = true))
	float WidgetUpdateFrequency = 0.05f;

	/* Defines default Interactable Widget class.*/
	UPROPERTY(config, EditAnywhere, Category = "Widgets", meta=(AllowedClasses="UserWidget", MustImplement="/Script/ActorInteractionPlugin.ActorInteractionWidget", ConfigRestartRequired = true))
	TSoftClassPtr<UUserWidget>InteractableDefaultWidgetClass;
	
	/* Defines default DataTable which contains Interactable data values.*/
	UPROPERTY(config, EditAnywhere, Category = "Interaction Data", meta=(AllowedClasses = "DataTable", ConfigRestartRequired = true))
	TSoftObjectPtr<UDataTable> InteractableDefaultDataTable;
	
#if WITH_EDITOR
	virtual FText GetSectionText() const override
	{
		return NSLOCTEXT("ActorInteractionPlugin", "MounteaSettingsDescription", "Actor Interaction Plugin");
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

	TSoftClassPtr<UUserWidget> GetInteractableDefaultWidgetClass() const
	{ return InteractableDefaultWidgetClass; };
};
