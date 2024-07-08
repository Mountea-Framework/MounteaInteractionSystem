// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorInteractionWidget.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UActorInteractionWidget : public UInterface
{
	GENERATED_BODY()
};

class IActorInteractableInterface;

/**
 * Interaction Widget Interface.
 * Implements basic Setters and Getters.
 *
 * This way every single Widget can be used as Interactable Widget.
 * Interface can be further extended in Blueprints.
 */
class ACTORINTERACTIONPLUGIN_API IActorInteractionWidget
{
	GENERATED_BODY()

public:
	
	/**
	 * This function is called to execute a command on the interaction widget.
	 * It can be used to send various commands and payloads to the interaction widget.
	 *
	 * @param Command The command to be executed on the interaction widget.
	 * @param Payload The payload associated with the command, which can be any UObject.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|InteractionWidget")
	void InteractionWidgetCommand(const FString& Command, UObject* Payload);

	
	/**
	 * This event should be called when you want to refresh UI data.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|InteractionWidget")
	void UpdateWidget(const TScriptInterface<IActorInteractableInterface>& InteractableInterface);

	/**
	 * This event should be called when you want to Hide/Unhide the Widget.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|InteractionWidget")
	 void ToggleVisibility();
	virtual void ToggleVisibility_Implementation() = 0;

	/**
	 * Sets new Title Text.
	 * @param NewTittle Title is usually Interactable Name. For example 'Treasure Chest'.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|InteractionWidget")
	void SetTitleText(const FString& NewTittle);
	/**
	 * Returns Title Text.
	 * Title is usually Interactable Name. For example 'Treasure Chest'.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|InteractionWidget")
	FString GetTitleText() const;

	/**
	 * Sets new Action Text.
	 * @param NewAction Action is usually what Interaction is supposed to happen. For example 'Hold' or 'Press'.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|InteractionWidget")
	void SetBodyText(const FString& NewAction);
	/**
	 * Returns Action Text.
	 * Action is usually what Interaction is supposed to happen. For example 'Hold' or 'Press'.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|InteractionWidget")
	FString GetBodyText() const;

	/**
	 * Sets new Interaction Key from Text.
	 * @param NewKey Platform dependant Key, if any is required.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|InteractionWidget")
	void SetKeyText(const FString& NewKey);
	/**
	 * Returns Interaction Key as Text.
	 * Interaction Key is platform dependant Key, if any is required.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|InteractionWidget")
	FString GetKeyText() const;

	/**
	 * Sets progress to be visually displayed in UI.
	 * UI should not be responsible for setting that progress itself!
	 * @param NewProgress Value to be set as displayed Progress.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|InteractionWidget")
	void SetProgress(const float NewProgress);
	/**
	 * Returns Interaction Progress.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|InteractionWidget")
	float GetProgress() const;
};
