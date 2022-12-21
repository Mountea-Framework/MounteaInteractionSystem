// All rights reserved Dominik Pavlicek 2022.

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
UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Hold"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentHold : public UActorInteractableComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractableComponentHold();

protected:

	virtual void BeginPlay() override;

	virtual void InteractionStarted(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;
	virtual void InteractionStopped(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;
	virtual void InteractionCanceled() override;

	virtual void InteractorFound(const TScriptInterface<IActorInteractorInterface>& FoundInteractor) override;
	virtual void InteractorLost(const TScriptInterface<IActorInteractorInterface>& LostInteractor) override;

	virtual float GetInteractionProgress() const override;

protected:

	UFUNCTION()
	void OnInteractionCompletedCallback();
};
