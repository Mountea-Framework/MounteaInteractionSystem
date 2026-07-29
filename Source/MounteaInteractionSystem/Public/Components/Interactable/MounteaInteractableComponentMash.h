// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInteractableComponentBase.h"
#include "MounteaInteractableComponentMash.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKeyMashed);

/**
 * v5.0 Actor Interactable Mash Component
 *
 * Player must press the interaction key MinMashAmountRequired times within KeystrokeTimeThreshold
 * seconds between each press. Runs out of time → interaction fails.
 * ActualMashAmount is replicated so all clients see consistent progress.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactable-Component-Mash
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName="Interactable Component Mash"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableComponentMash : public UMounteaInteractableComponentBase
{
	GENERATED_BODY()

public:

	UMounteaInteractableComponentMash();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	int32 GetMinMashAmountRequired() const { return MinMashAmountRequired; }

	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetMinMashAmountRequired(const int32 Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	float GetKeystrokeTimeThreshold() const { return KeystrokeTimeThreshold; }

	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetKeystrokeTimeThreshold(const float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	int32 GetActualMashAmount() const { return ActualMashAmount; }

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;
	virtual void InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;
	virtual void InteractionCanceled_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractionFailedEvent();

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnKeyMashedEvent();

private:

	UFUNCTION()
	void OnKeystrokeThresholdExpired();

	void ResetMashState();

	FTimerHandle Timer_KeystrokeThreshold;

protected:

	/** How many key presses are required to complete the interaction. */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="MounteaInteraction|Required",
		meta=(Units="times", UIMin=2, ClampMin=2))
	int32 MinMashAmountRequired = 5;

	/** Max time allowed between consecutive key presses before interaction fails. */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="MounteaInteraction|Required",
		meta=(Units="s", UIMin=0.01, ClampMin=0.01))
	float KeystrokeTimeThreshold = 1.f;

	/** Current mash count (replicated for consistent progress display). */
	UPROPERTY(ReplicatedUsing=OnRep_ActualMashAmount, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	int32 ActualMashAmount = 0;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactable")
	FOnInteractionFailed OnInteractionFailed;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactable")
	FOnKeyMashed OnKeyMashed;

private:

	UFUNCTION()
	void OnRep_ActualMashAmount();
};
