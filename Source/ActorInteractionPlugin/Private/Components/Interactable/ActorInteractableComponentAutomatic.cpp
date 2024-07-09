// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "Components/Interactable/ActorInteractableComponentAutomatic.h"
#include "TimerManager.h"

#include "Helpers/ActorInteractionPluginLog.h"

#define LOCTEXT_NAMESPACE "ActorInteractableComponentAutomatic"

UActorInteractableComponentAutomatic::UActorInteractableComponentAutomatic()
{
	InteractableName = NSLOCTEXT("ActorInteractableComponentAutomatic", "Auto", "Auto");

	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = 1.f;

	bInteractionHighlight = false;
}

void UActorInteractableComponentAutomatic::BeginPlay()
{
	Super::BeginPlay();

	OnInteractionStopped.RemoveAll(this);
}

void UActorInteractableComponentAutomatic::InteractableSelected_Implementation(const TScriptInterface<IActorInteractableInterface>& Interactable)
{
	if(!GetWorld()) return;
	Super::InteractableSelected_Implementation(Interactable);
	
	if (Interactable == this)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(Timer_Interaction) == false)
		{
			OnInteractionStarted.Broadcast(GetWorld()->GetTimeSeconds(), Execute_GetInteractor(this));
		}
	}
}

void UActorInteractableComponentAutomatic::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	if (!GetWorld()) return;

	if (Execute_CanInteract(this) && !GetWorld()->GetTimerManager().IsTimerActive(Timer_Interaction))
	{
		// Force Interaction Period to be at least 0.01s
		const float TempInteractionPeriod = FMath::Max(0.01f, InteractionPeriod);
		
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &UActorInteractableComponentAutomatic::OnInteractionCompletedCallback);

		GetWorld()->GetTimerManager().SetTimer
		(
			Timer_Interaction,
			Delegate,
			TempInteractionPeriod,
			false
		);

		Super::InteractionStarted_Implementation(TimeStarted, CausingInteractor);
		
		UpdateInteractionWidget();
	}
}

void UActorInteractableComponentAutomatic::InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
#if WITH_EDITOR
	LOG_WARNING(TEXT("[InteractionStopped] This function does nothing in UActorInteractableComponentAutomatic"))
#endif
}

FInteractionStarted& UActorInteractableComponentAutomatic::GetOnInteractionStartedHandle()
{
#if WITH_EDITOR
	LOG_WARNING(TEXT("[GetOnInteractionStartedHandle] This handle is invalid in UActorInteractableComponentAutomatic"))
#endif
	
	return EmptyHandle_Started;
}

FInteractionStopped& UActorInteractableComponentAutomatic::GetOnInteractionStoppedHandle()
{
#if WITH_EDITOR
	LOG_WARNING(TEXT("[GetOnInteractionStoppedHandle] This handle is invalid in UActorInteractableComponentAutomatic"))
#endif
	
	return EmptyHandle_Stopped;
}

void UActorInteractableComponentAutomatic::OnInteractionCompletedCallback()
{
	if (!GetWorld())
	{
		OnInteractionCanceled.Broadcast();
		return;
	}

	Execute_ToggleWidgetVisibility(this, false);
	if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
	{
		if (Execute_TriggerCooldown(this)) return;
	}
	
	OnInteractionCompleted.Broadcast(GetWorld()->GetTimeSeconds(), Execute_GetInteractor(this));
}

#undef LOCTEXT_NAMESPACE
