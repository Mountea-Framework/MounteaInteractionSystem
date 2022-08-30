// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActorInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionCompleted, enum EInteractableType, FinishedInteractionType, const float, FinishTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionStarted, enum EInteractableType, RecievedInteractionType, const float, RecievedInteractionTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionStopped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionMashKeyPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionMashFailed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractorFound, class UActorInteractorComponent*, InteractingComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractorLost, class UActorInteractorComponent*, InteractingComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableTraced, class UActorInteractorComponent*, TracingInteractorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractorOverlapped, UPrimitiveComponent*, OverlappedInteractableCollisionComponent);

/**
 * Interface responsible for cleaner communication between Interacting components.
 */
class ACTORINTERACTIONPLUGIN_API IActorInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
