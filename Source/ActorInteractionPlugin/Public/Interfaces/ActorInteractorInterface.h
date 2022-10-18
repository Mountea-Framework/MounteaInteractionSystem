// All rights reserved Dominik Pavlicek 2022.

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

enum class EInteractorState : uint8;
class IActorInteractableInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableFound, const TScriptInterface<IActorInteractableInterface>&, FoundInteractable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableLost, const TScriptInterface<IActorInteractableInterface>&, LostInteractable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionKeyPressed, float, TimeKeyPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionKeyReleased, float, TimeKeyReleased);

/**
 * 
 */
class ACTORINTERACTIONPLUGIN_API IActorInteractorInterface
{
	GENERATED_BODY()

public:
	
	virtual void StartInteraction() = 0;
	virtual void StopInteraction() = 0;
	
	virtual bool ActivateInteractor(FString& ErrorMessage) = 0;
	virtual void DeactivateInteractor() = 0;

	virtual void TickInteraction(const float DeltaTime) = 0;

	virtual ECollisionChannel GetResponseChannel() const = 0;
	virtual void SetResponseChannel(const ECollisionChannel NewResponseChannel) = 0;

	virtual EInteractorState GetState() const = 0;
	virtual void SetState(const EInteractorState NewState) = 0;

	virtual bool DoesAutoActivate() const = 0;
	virtual void SetDoesAutoActivate(const bool bNewAutoActivate) = 0;

	virtual FKey GetInteractionKey() const = 0;
	virtual void SetInteractionKey(const FKey NewInteractorKey) = 0;
};
