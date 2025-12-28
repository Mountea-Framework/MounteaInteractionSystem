// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInteractionFunctionLibrary.generated.h"

struct FInteractableBaseSettings;
struct FInteractorBaseSettings;
struct FInteractionHighlightSetup;
class UDataTable;
class UUserWidget;
class UMounteaInteractionSystemSettings;

/**
 * 
 */
UCLASS()
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Interaction Settings"))
	static UMounteaInteractionSystemSettings* GetInteractionSettings();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Interaction Settings"))
	static FInteractorBaseSettings GetDefaultInteractorSettings();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Interaction Settings"))
	static FInteractableBaseSettings GetDefaultInteractableSettings();
	
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Default Interactable Widget"))
	static TSubclassOf<UUserWidget> GetInteractableDefaultWidgetClass();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction", meta=(CompactNodeTitle="Is Debug Enabled"))
	static bool IsEditorDebugEnabled();
};
