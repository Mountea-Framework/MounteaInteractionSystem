// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentAutomatic.h"
#include "TimerManager.h"

#include "Helpers/ActorInteractionPluginLog.h"

#define LOCTEXT_NAMESPACE "ActorInteractableComponentAutomatic"

UActorInteractableComponentAutomatic::UActorInteractableComponentAutomatic()
{
	InteractableName = LOCTEXT("ActorInteractableComponentAutomatic", "Auto");
	InteractionKeysPerPlatform.Empty();

	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = 1.f;

	bInteractionHighlight = false;
}

void UActorInteractableComponentAutomatic::BeginPlay()
{
	Super::BeginPlay();

	OnInteractionStopped.RemoveAll(this);
}

void UActorInteractableComponentAutomatic::InteractableSelected(const TScriptInterface<IActorInteractableInterface>& Interactable)
{
	if(!GetWorld()) return;
	Super::InteractableSelected(Interactable);
	
	if (Interactable == this)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(Timer_Interaction) == false)
		{
			OnInteractionStarted.Broadcast(GetWorld()->GetTimeSeconds(), FKey(""), GetInteractor());
		}
	}
}

void UActorInteractableComponentAutomatic::InteractionStarted(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	if (!GetWorld()) return;

	if (CanInteract() && !GetWorld()->GetTimerManager().IsTimerActive(Timer_Interaction))
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

		Super::InteractionStarted(TimeStarted, PressedKey, CausingInteractor);
		
		UpdateInteractionWidget();
	}
}

void UActorInteractableComponentAutomatic::InteractionStopped(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
#if WITH_EDITOR
	AIntP_LOG(Warning, TEXT("[InteractionStopped] This function does nothing in UActorInteractableComponentAutomatic"))
#endif
}

FInteractionStarted& UActorInteractableComponentAutomatic::GetOnInteractionStartedHandle()
{
#if WITH_EDITOR
	AIntP_LOG(Warning, TEXT("[GetOnInteractionStartedHandle] This handle is invalid in UActorInteractableComponentAutomatic"))
#endif
	
	return EmptyHandle_Started;
}

FInteractionStopped& UActorInteractableComponentAutomatic::GetOnInteractionStoppedHandle()
{
#if WITH_EDITOR
	AIntP_LOG(Warning, TEXT("[GetOnInteractionStoppedHandle] This handle is invalid in UActorInteractableComponentAutomatic"))
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

	ToggleWidgetVisibility(false);
	if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
	{
		if (TriggerCooldown()) return;
	}
	
	OnInteractionCompleted.Broadcast(GetWorld()->GetTimeSeconds(), GetInteractor());
}

#undef LOCTEXT_NAMESPACE
