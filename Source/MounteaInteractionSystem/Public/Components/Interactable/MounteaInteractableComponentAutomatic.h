// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInteractableComponentBase.h"
#include "MounteaInteractableComponentAutomatic.generated.h"

/**
 * Actor Interactable Automatic Component
 *
 * Child class of Actor Interactable Base Component.
 * This component will automatically finish with no need to press any key. Useful for notification interactions, like spotting an object from distance.
 *
 * Implements MounteaInteractableInterface.
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactable-Component-Automatic
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Auto"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableComponentAutomatic : public UMounteaInteractableComponentBase
{
	GENERATED_BODY()

public:

	UMounteaInteractableComponentAutomatic();

protected:

	virtual void BeginPlay() override;
	
	virtual void OnInteractionCompletedCallback();

	virtual void InteractableSelected_Implementation(const TScriptInterface<IMounteaInteractableInterface>& Interactable) override;
	
	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;
	virtual void InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;

public:
	
	virtual FInteractionStarted& GetOnInteractionStartedHandle() override;
	virtual FInteractionStopped& GetOnInteractionStoppedHandle() override;

private:

	FInteractionStarted EmptyHandle_Started;
	FInteractionStopped EmptyHandle_Stopped;
};