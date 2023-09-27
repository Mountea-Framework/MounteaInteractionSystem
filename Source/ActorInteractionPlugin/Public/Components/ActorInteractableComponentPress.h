// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractableComponentBase.h"
#include "ActorInteractableComponentPress.generated.h"

/**
 * Actor Interactable Press Component
 *
 * Child class of Actor Interactable Base Component.
 * This component requires to press interaction key. Interaction period is ignored.
 * 
 * Implements ActorInteractableInterface.
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Press
 */
UCLASS(ClassGroup=(Interaction), Blueprintable, meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Press"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentPress : public UActorInteractableComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractableComponentPress();

protected:

	virtual void BeginPlay() override;

	virtual void InteractionStarted(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	virtual void ResetDefaultValuesImpl() override;
#endif
	
};
