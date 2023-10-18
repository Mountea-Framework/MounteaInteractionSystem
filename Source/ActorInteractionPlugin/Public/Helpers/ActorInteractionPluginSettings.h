// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Materials/MaterialInterface.h"
#include "ActorInteractionPluginSettings.generated.h"

class UDataTable;
class UMaterialInterface;
class UUserWidget;

/**
 * Mountea Interaction System global settings.
 */
UCLASS(config = MounteaSettings, meta = (DisplayName = "Mountea Interaction System Settings"))
class ACTORINTERACTIONPLUGIN_API UActorInteractionPluginSettings : public UDeveloperSettings
{
	
  GENERATED_BODY()

	UActorInteractionPluginSettings()
	{
		CategoryName = TEXT("Mountea Framework");
		SectionName = TEXT("Mountea Interaction System");

  		bEditorDebugEnabled = 0;
	}

	/* Defines whether in-editor debug is enabled. */
	UPROPERTY(config, EditAnywhere, Category="Editor")
	uint8 bEditorDebugEnabled : 1;

	/* Defines how often is the Interaction widget updated per second.*/
	UPROPERTY(config, EditAnywhere, Category = "Widgets", meta=(Units="s", UIMin=0.001, ClampMin=0.001))
	float WidgetUpdateFrequency = 0.05f;

	/* Defines default Interactable Widget class.*/
	UPROPERTY(config, EditAnywhere, Category = "Widgets", meta=(AllowedClasses="/Script/UMG.UserWidget", MustImplement="/Script/ActorInteractionPlugin.ActorInteractionWidget"))
	TSoftClassPtr<UUserWidget>InteractableDefaultWidgetClass;
	
	/* Defines default DataTable which contains Interactable data values.*/
	UPROPERTY(config, EditAnywhere, Category = "Interaction Data", meta=(AllowedClasses = "/Script/Engine.DataTable"))
	TSoftObjectPtr<UDataTable> InteractableDefaultDataTable;

	/* Defines default DataTable which contains Interactable data values.*/
	UPROPERTY(config, EditAnywhere, Category = "Interaction Data")
	TSoftObjectPtr<UMaterialInterface> InteractableDefaultHighlightMaterial;
	
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

	TSoftClassPtr<UUserWidget> GetInteractableDefaultWidgetClass() const
	{ return InteractableDefaultWidgetClass; };

	UMaterialInterface* GetDefaultHighlightMaterial() const
	{ return InteractableDefaultHighlightMaterial.LoadSynchronous(); };
};
