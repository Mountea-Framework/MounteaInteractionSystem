// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentHold.h"

#include "Interfaces/ActorInteractorInterface.h"

#include "Helpers/ActorInteractionPluginLog.h"

#define LOCTEXT_NAMESPACE "InteractableComponentHold"

UActorInteractableComponentHold::UActorInteractableComponentHold()
{
	InteractionPeriod = 3.f;
	InteractableName = LOCTEXT("InteractableComponentHold", "Hold");
}

void UActorInteractableComponentHold::BeginPlay()
{
	Super::BeginPlay();
}

void UActorInteractableComponentHold::InteractionStarted(const float& TimeStarted, const FKey& PressedKey)
{
	if (CanInteract())
	{
		// Force Interaction Period to be at least 0.1s
		const float TempInteractionPeriod = FMath::Max(0.1f, InteractionPeriod);

		if (!GetWorld()) return;
	
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &UActorInteractableComponentHold::OnInteractionCompletedCallback);

		GetWorld()->GetTimerManager().SetTimer
		(
			Timer_Interaction,
			Delegate,
			TempInteractionPeriod,
			false
		);
		
		Super::InteractionStarted(TimeStarted, PressedKey);
	}
}

void UActorInteractableComponentHold::InteractionStopped(const float& TimeStarted, const FKey& PressedKey)
{
	Super::InteractionStopped(TimeStarted, PressedKey);
}

void UActorInteractableComponentHold::InteractionCanceled()
{
	Super::InteractionCanceled();
}

void UActorInteractableComponentHold::InteractorFound(const TScriptInterface<IActorInteractorInterface>& FoundInteractor)
{
	Super::InteractorFound(FoundInteractor);
}

void UActorInteractableComponentHold::InteractorLost(const TScriptInterface<IActorInteractorInterface>& LostInteractor)
{
	Super::InteractorLost(LostInteractor);
}

float UActorInteractableComponentHold::GetInteractionProgress() const
{
	return Super::GetInteractionProgress();
}

void UActorInteractableComponentHold::OnInteractionCompletedCallback()
{
	if (!GetWorld())
	{
		return;
	}

	ToggleWidgetVisibility(false);
	if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
	{
		if (TriggerCooldown()) return;
	}
	
	OnInteractionCompleted.Broadcast(GetWorld()->GetTimeSeconds());
}

#undef LOCTEXT_NAMESPACE
