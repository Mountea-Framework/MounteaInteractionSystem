// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ActorInteractionPluginSettings.generated.h"

class UDataTable;

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
		SectionName = TEXT("Interaction");
	}

	/* Defines how often is the Interaction widget updated per second.*/
	UPROPERTY(config, EditAnywhere, Category = "Widgets", meta=(Units="s", UIMin=0.001, ClampMin=0.001))
	float WidgetUpdateFrequency = 0.05f;

	/* Defines default Interactable Widget class.*/
	UPROPERTY(config, EditAnywhere, Category = "Widgets", meta=(MustImplement="ActorInteractionWidget"))
	TSoftClassPtr<UUserWidget> InteractableDefaultWidgetClass;
	
	/* Defines default DataTable which contains Interactable data values.*/
	UPROPERTY(config, EditAnywhere, Category = "Interaction Data")
	TSoftObjectPtr<UDataTable> InteractableDefaultDataTable;
	
	virtual FName GetContainerName() const override
	{
		return "Project";
	}

public:

	float GetWidgetUpdateFrequency() const
	{ return WidgetUpdateFrequency; }

	TSoftObjectPtr<UDataTable> GetInteractableDefaultDataTable() const
	{ return InteractableDefaultDataTable; };

	TSoftClassPtr<UUserWidget> GetInteractableDefaultWidgetClass() const
	{ return InteractableDefaultWidgetClass; };
};
