// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorInteractorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActorInteractorInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableFound, class UActorInteractableComponent*, FoundActorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableLost, class UActorInteractableComponent*, LostActorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionKeyPressed, float, TimeKeyPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionKeyReleased, float, TimeKeyReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractorTypeChanged, float, TimeChanged);

/**
 * Interface responsible for cleaner communication between Interacting components.
 */
class ACTORINTERACTIONPLUGIN_API IActorInteractorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void StartInteraction() = 0;
	virtual void StopInteraction() = 0;
	virtual void OnInteractableOverlapped(class UPrimitiveComponent* OverlappedComponent) = 0;

	virtual enum EInteractorType GetInteractorType() const = 0;
};
