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

	/** Returns the project's Mountea Interaction System settings object. */
	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::GetInteractionSystemSettings instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable", meta=(CompactNodeTitle="Default Interaction Settings",
		DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::GetInteractionSystemSettings instead."),
		meta=(DisplayName="Interaction Settings - Get Interaction Settings"))
	static UMounteaInteractionSystemSettings* GetInteractionSettings();

	/**
	 * Returns Default Widget Update frequency.
	 * Default value is 0.05.
	 *
	 * This value can be updated in Project Settings.
	 */
	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::GetDefaultWidgetUpdateFrequency instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable", meta=(CompactNodeTitle="Default Widget Frequncy",
		DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::GetDefaultWidgetUpdateFrequency instead."),
		meta=(DisplayName="Interaction Settings - Get Default Widget Update Frequency"))
	static float GetDefaultWidgetUpdateFrequency();

	/** Returns the default interactable DataTable configured in Project Settings. */
	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::GetInteractableDefaultDataTable instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable", meta=(CompactNodeTitle="Default Interactable Data",
		DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::GetInteractableDefaultDataTable instead."),
		meta=(DisplayName="Interaction Settings - Get Interactable Default Data Table"))
	static UDataTable* GetInteractableDefaultDataTable();

	/** Returns the default interaction widget class configured in Project Settings. */
	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::GetInteractableDefaultWidgetClass instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable", meta=(CompactNodeTitle="Default Interactable Widget",
		DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::GetInteractableDefaultWidgetClass instead."),
		meta=(DisplayName="Interaction Settings - Get Interactable Default Widget Class"))
	static TSubclassOf<UUserWidget> GetInteractableDefaultWidgetClass();

	/** Returns whether editor debug logging/visualization is enabled in Project Settings. */
	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::IsEditorDebugEnabled instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable", meta=(CompactNodeTitle="Is Debug Enabled",
		DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::IsEditorDebugEnabled instead."),
		meta=(DisplayName="Interaction Settings - Is Editor Debug Enabled"))
	static bool IsEditorDebugEnabled();
};
