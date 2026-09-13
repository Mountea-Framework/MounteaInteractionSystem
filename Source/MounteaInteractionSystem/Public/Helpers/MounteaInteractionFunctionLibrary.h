// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInteractionFunctionLibrary.generated.h"

struct FInteractionHighlightSetup;
class UDataTable;
class UUserWidget;
class UMounteaInteractionSystemSettings;

/**
 * Deprecated - kept only so existing customer Blueprints keep compiling.
 * All functions are ported to UMounteaInteractableStatics; use that class for new content.
 * See Config/DefaultMounteaInteractionSystem.ini for the matching CoreRedirects entries.
 */
UCLASS()
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::GetInteractionSystemSettings instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable", meta=(CompactNodeTitle="Default Interaction Settings",
		DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::GetInteractionSystemSettings instead."))
	static UMounteaInteractionSystemSettings* GetInteractionSettings();

	/**
	 * Returns Default Widget Update frequency.
	 * Default value is 0.05.
	 *
	 * This value can be updated in Project Settings.
	 */
	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::GetDefaultWidgetUpdateFrequency instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable", meta=(CompactNodeTitle="Default Widget Frequncy",
		DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::GetDefaultWidgetUpdateFrequency instead."))
	static float GetDefaultWidgetUpdateFrequency();

	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::GetInteractableDefaultDataTable instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable", meta=(CompactNodeTitle="Default Interactable Data",
		DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::GetInteractableDefaultDataTable instead."))
	static UDataTable* GetInteractableDefaultDataTable();

	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::GetInteractableDefaultWidgetClass instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable", meta=(CompactNodeTitle="Default Interactable Widget",
		DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::GetInteractableDefaultWidgetClass instead."))
	static TSubclassOf<UUserWidget> GetInteractableDefaultWidgetClass();

	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::IsEditorDebugEnabled instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable", meta=(CompactNodeTitle="Is Debug Enabled",
		DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::IsEditorDebugEnabled instead."))
	static bool IsEditorDebugEnabled();
};
