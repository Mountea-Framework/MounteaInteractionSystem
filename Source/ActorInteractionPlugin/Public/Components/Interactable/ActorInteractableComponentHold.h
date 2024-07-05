// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractableComponentBase.h"
#include "ActorInteractableComponentHold.generated.h"

/**
 * Actor Interactable Hold Component
 *
 * Child class of Actor Interactable Base Component.
 * This component requires to hold interaction key for specified period of time.
 *
 * Implements ActorInteractableInterface.
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Hold
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Hold"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentHold : public UActorInteractableComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractableComponentHold();

protected:

	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;

protected:

	UFUNCTION()
	void OnInteractionCompletedCallback();
};
