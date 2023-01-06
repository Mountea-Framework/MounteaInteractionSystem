// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractionPluginLog.h"
#include "ActorInteractionPluginSettings.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
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
	static UDataTable* GetInteractableDefaultDataTable()
	{
		const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>();

		if (Settings)
		{
			if (const auto FoundTable = Settings->GetInteractableDefaultDataTable().LoadSynchronous())
			{
				return FoundTable;
			}
		}

		AIntP_LOG(Error, TEXT("[GetInteractableDefaultDataTable] Cannot load ActorInteractionPluginSettings! Using null value."))
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Interactable Data"))
	static TSubclassOf<UUserWidget> GetInteractableDefaultWidgetClass()
	{
		const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>();

		if (Settings)
		{
			const TSubclassOf<UUserWidget> WidgetClass = Settings->GetInteractableDefaultWidgetClass().LoadSynchronous();
			return WidgetClass;
		}
		
		AIntP_LOG(Error, TEXT("[GetInteractableDefaultWidgetClass] Cannot load ActorInteractionPluginSettings! Using null value."))
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Interactable Data"))
	static bool IsEditorDebugEnabled()
	{
		const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>();

		if (Settings)
		{
			return Settings->IsEditorDebugEnabled();
		}
		
		AIntP_LOG(Error, TEXT("[GetInteractableDefaultWidgetClass] Cannot load ActorInteractionPluginSettings! Using null value."))
		return false;
	}
};
