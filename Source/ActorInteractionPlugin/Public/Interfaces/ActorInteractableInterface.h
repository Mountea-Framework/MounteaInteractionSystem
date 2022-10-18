// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UActorInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IActorInteractorInterface;

/**
 * 
 */
class ACTORINTERACTIONPLUGIN_API IActorInteractableInterface
{
	GENERATED_BODY()

public:

	virtual void OnInteractionCompleted(const float FinishTime) = 0;
	virtual void OnInteractionStarted(const float ReceivedInteractionTime) = 0;
	virtual void OnInteractionStopped() = 0;

	virtual void OnInteractorFound(TScriptInterface<IActorInteractorInterface> InteractingComponent) = 0;
	virtual void OnInteractorLost(TScriptInterface<IActorInteractorInterface> InteractingComponent) = 0;
};
