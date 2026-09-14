// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "MounteaInteractionWidget.generated.h"

UINTERFACE(BlueprintType, Blueprintable)
class UActorInteractionWidget : public UInterface
{
	GENERATED_BODY()
};

class IMounteaInteractableInterface;

/**
 * v5.0 Interaction Widget Interface.
 * Implements setters/getters for both attention and interaction widget modes.
 * Every widget used by the Interaction System must implement this interface.
 */
class MOUNTEAINTERACTIONSYSTEM_API IActorInteractionWidget
{
	GENERATED_BODY()

public:

	/**
	 * Sets the widget display mode.
	 * Attention: shown when actor is in attention range but not yet interaction range.
	 * Interaction: shown when actor is focused and in interaction range.
	 * None: hidden.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Set Widget Mode"))
	void SetWidgetMode(const EInteractionWidgetMode NewMode);

	/**
	 * Informs the widget how many actions are available for cycling on the focused actor.
	 * Useful for displaying "1 / 3" indicators in the UI.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Set Action Count"))
	void SetActionCount(const int32 TotalActions);

	/**
	 * Informs the widget which action index is currently highlighted.
	 * Zero-based index into the action list on the focused actor.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Set Selected Action Index"))
	void SetSelectedActionIndex(const int32 SelectedIndex);

	/** General command/payload channel for custom widget communication. */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Interaction Widget Command"))
	void InteractionWidgetCommand(const FString& Command, UObject* Payload);

	/** Called to refresh all displayed data from the given interactable. */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Update Widget"))
	void UpdateWidget(const TScriptInterface<IMounteaInteractableInterface>& InteractableInterface);

	/** Shows or hides the widget. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Toggle Visibility"))
	void ToggleVisibility();
	virtual void ToggleVisibility_Implementation() = 0;

	/** Sets the title text (usually the interactable's display name, e.g. "Treasure Chest"). */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Set Title Text"))
	void SetTitleText(const FString& NewTitle);

	/** Returns the current title text. */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Get Title Text"))
	FString GetTitleText() const;

	/** Sets the body/action text (e.g. "Hold", "Press"). */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Set Body Text"))
	void SetBodyText(const FString& NewBody);

	/** Returns the current body text. */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Get Body Text"))
	FString GetBodyText() const;

	/** Sets the key prompt text displayed on the widget. */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Set Key Text"))
	void SetKeyText(const FString& NewKey);

	/** Returns the current key prompt text. */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Get Key Text"))
	FString GetKeyText() const;

	/** Sets the interaction progress value [0..1]. Widget handles visual display - do not compute this in the widget itself. */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Set Progress"))
	void SetProgress(const float NewProgress);

	/** Returns the current progress value. */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Widget",
		meta=(DisplayName="Get Progress"))
	float GetProgress() const;
};
