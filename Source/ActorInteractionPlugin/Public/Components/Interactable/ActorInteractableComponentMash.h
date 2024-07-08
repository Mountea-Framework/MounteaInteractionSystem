// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractableComponentBase.h"
#include "ActorInteractableComponentMash.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKeyMashed);

/**
 * Actor Interactable Mash Component
 *
 * Child class of Actor Interactable Base Component.
 * This component requires to press interaction key multiple times within specified time period.
 * This interaction can fail if the interaction key is not pressed enough times or time runs out.
 * 
 * Implements ActorInteractableInterface.
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Mash
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Mash"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentMash : public UActorInteractableComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractableComponentMash();

protected:
	
	virtual void BeginPlay() override;
	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;
	virtual void InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;
	virtual void InteractionCanceled_Implementation() override;
	virtual void InteractionCompleted_Implementation(const float& TimeCompleted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;

protected:
	
	/**
	 * Event called from OnInteractionFailed
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractionFailedEvent();
	UFUNCTION()
	virtual void InteractionFailed();
	virtual void OnInteractionFailedCallback();

	UFUNCTION()
	void OnInteractionCompletedCallback();
	
	/**
	 * Event called once Key is pressed.
	 * Is the same as Interaction Started, but with more understandable name.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnKeyMashedEvent();

	virtual void CleanupComponent() override;
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	int32 GetMinMashAmountRequired() const;
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetMinMainAmountRequired(const int32 Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	float GetKeystrokeTimeThreshold() const;
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetKeystrokeTimeThreshold(const float Value);
	
protected:
	
	/**
	 * How many time the button must be pressed to accept the interaction as completed.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly,  Category="MounteaInteraction|Required", meta=(Units = "times", UIMin = 2, ClampMin = 2))
	int32 MinMashAmountRequired;

	/**
	 * How much time it is allowed between each Key stroke before interaction fails.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly,  Category="MounteaInteraction|Required", meta=(Units = "s", UIMin = 0.01, ClampMin = 0.01))
	float KeystrokeTimeThreshold;

protected:

	FTimerHandle TimerHandle_Mashed;

	/**
	 * How many times the key was mashed.
	 */
	UPROPERTY(VisibleAnywhere, Category="MounteaInteraction|Read Only")
	int32 ActualMashAmount;
	
protected:

	/**
	 * Event called once Interaction Mash fails.
	 * Either because of Time is out or not enough Key strokes are registered.
	 * Calls OnInteractionFailedEvent.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactable")
	FOnInteractionFailed OnInteractionFailed;

	/**
	 * Event called once Key is pressed.
	 * Is the same as Interaction Started, but with more understandable name.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactable")
	FOnKeyMashed OnKeyMashed;
};