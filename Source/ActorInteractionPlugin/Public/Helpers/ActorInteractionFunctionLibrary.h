// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActorInteractionFunctionLibrary.generated.h"

class UDataTable;
class UUserWidget;

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
	static float GetDefaultWidgetUpdateFrequency();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Interactable Data"))
	static UDataTable* GetInteractableDefaultDataTable();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Interactable Data"))
	static TSubclassOf<UUserWidget> GetInteractableDefaultWidgetClass();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Interactable Data"))
	static bool IsEditorDebugEnabled();
};
