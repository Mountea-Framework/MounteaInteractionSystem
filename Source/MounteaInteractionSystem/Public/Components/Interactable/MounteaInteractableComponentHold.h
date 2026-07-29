// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInteractableComponentBase.h"
#include "MounteaInteractableComponentHold.generated.h"

/**
 * v5.0 Actor Interactable Hold Component
 *
 * Player must hold the interaction key for InteractionPeriod seconds.
 * Progress is computed from InteractionStartServerTime (replicated server time) — no FTimerHandle replication.
 * Releasing the key early cancels the interaction.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactable-Component-Hold
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName="Interactable Component Hold"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableComponentHold : public UMounteaInteractableComponentBase
{
	GENERATED_BODY()

public:

	UMounteaInteractableComponentHold();

protected:

	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;
	virtual void InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;

private:

	UFUNCTION()
	void OnHoldCompleted();

	FTimerHandle Timer_Hold;
};
