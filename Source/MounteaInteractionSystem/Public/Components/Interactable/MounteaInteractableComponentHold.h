// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInteractableComponentBase.h"
#include "MounteaInteractableComponentHold.generated.h"

/**
 * Actor Interactable Hold Component
 *
 * Child class of Actor Interactable Base Component.
 * This component requires to hold interaction key for specified period of time.
 *
 * Implements MounteaInteractableInterface.
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactable-Component-Hold
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Hold"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableComponentHold : public UMounteaInteractableComponentBase
{
	GENERATED_BODY()

public:

	UMounteaInteractableComponentHold();

protected:

	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;

protected:

	UFUNCTION()
	void OnInteractionCompletedCallback();
};	