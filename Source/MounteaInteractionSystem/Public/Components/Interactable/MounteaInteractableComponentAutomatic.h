// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInteractableComponentBase.h"
#include "MounteaInteractableComponentAutomatic.generated.h"

/**
 * v5.0 Actor Interactable Automatic Component
 *
 * Completes automatically after InteractionPeriod seconds once an interactor enters a slot.
 * No player input required — useful for notification-style interactions (spotting, proximity triggers).
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactable-Component-Automatic
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName="Interactable Component Auto"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableComponentAutomatic : public UMounteaInteractableComponentBase
{
	GENERATED_BODY()

public:

	UMounteaInteractableComponentAutomatic();

protected:

	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;
	virtual void InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;

private:

	UFUNCTION()
	void OnCompletionTimerExpired();

	FTimerHandle Timer_AutoCompletion;
};
