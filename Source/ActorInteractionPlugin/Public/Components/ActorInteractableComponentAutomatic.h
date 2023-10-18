// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractableComponentBase.h"
#include "ActorInteractableComponentAutomatic.generated.h"

/**
 * Actor Interactable Automatic Component
 *
 * Child class of Actor Interactable Base Component.
 * This component will automatically finish with no need to press any key. Useful for notification interactions, like spotting an object from distance.
 *
 * Implements ActorInteractableInterface.
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Automatic
 */
UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Auto"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentAutomatic : public UActorInteractableComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractableComponentAutomatic();

protected:

	virtual void BeginPlay() override;
	
	virtual void OnInteractionCompletedCallback();

	virtual void InteractableSelected(const TScriptInterface<IActorInteractableInterface>& Interactable) override;
	
	virtual void InteractionStarted(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;
	virtual void InteractionStopped(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;

public:
	
	virtual FInteractionStarted& GetOnInteractionStartedHandle() override;
	virtual FInteractionStopped& GetOnInteractionStoppedHandle() override;

private:

	FInteractionStarted EmptyHandle_Started;
	FInteractionStopped EmptyHandle_Stopped;
};
