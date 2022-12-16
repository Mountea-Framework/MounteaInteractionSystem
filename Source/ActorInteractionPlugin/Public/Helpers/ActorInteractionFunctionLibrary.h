// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractionPluginLog.h"
#include "ActorInteractionPluginSettings.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActorInteractionFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ACTORINTERACTIONPLUGIN_API UActorInteractionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Returns Default Widget Update frequency.
	 * Default value is 0.05.
	 *
	 * This value can be updated in Project Settings.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Widget Frequncy"))
	static float GetDefaultWidgetUpdateFrequency()
	{
		const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>();

		if (Settings)
		{
			return Settings->GetWidgetUpdateFrequency();
		}

		AIntP_LOG(Error, TEXT("[GetDefaultWidgetUpdateFrequency] Cannot load ActorInteractionPluginSettings! Using hardcoded value."))
		return 0.05f;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Interactable Data"))
	static TSoftObjectPtr<UDataTable> GetInteractableDefaultDataTable()
	{
		const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>();

		if (Settings)
		{
			return Settings->GetInteractableDefaultDataTable();
		}

		AIntP_LOG(Error, TEXT("[GetInteractableDefaultDataTable] Cannot load ActorInteractionPluginSettings! Using null value."))
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Interactable Data"))
	static TSoftClassPtr<UUserWidget> GetInteractableDefaultWidgetClass()
	{
		const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>();

		if (Settings)
		{
			return Settings->GetInteractableDefaultWidgetClass();
		}
		
		AIntP_LOG(Error, TEXT("[GetInteractableDefaultWidgetClass] Cannot load ActorInteractionPluginSettings! Using null value."))
		return nullptr;
	}
};
