// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentMash.h"

#include "TimerManager.h"
#include "Helpers/ActorInteractionPluginLog.h"

#define LOCTEXT_NAMESPACE "ActorInteractableComponentMash"

UActorInteractableComponentMash::UActorInteractableComponentMash()
{
	bInteractionHighlight = true;
	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	MinMashAmountRequired = 5;
	KeystrokeTimeThreshold = 1.f;
	ActualMashAmount = 0;
	InteractionPeriod = 3.f;
	InteractableName = LOCTEXT("ActorInteractableComponentMash", "Mash");
}

void UActorInteractableComponentMash::BeginPlay()
{
	Super::BeginPlay();

	OnInteractionFailed.AddUniqueDynamic(this, &UActorInteractableComponentMash::InteractionFailed);
	OnKeyMashed.AddUniqueDynamic(this, &UActorInteractableComponentMash::OnKeyMashedEvent);
}

void UActorInteractableComponentMash::InteractionFailed()
{
	SetState(GetDefaultState());
	
	CleanupComponent();
	
	OnInteractionFailedEvent();
}

void UActorInteractableComponentMash::OnInteractionFailedCallback()
{
	OnInteractionFailed.Broadcast();
}

void UActorInteractableComponentMash::OnInteractionCompletedCallback()
{
	if (!GetWorld())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Mashed);
	
	if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
	{
		if (TriggerCooldown()) return;
	}
	
	OnInteractionCompleted.Broadcast(GetWorld()->GetTimeSeconds(), GetInteractor());
}

void UActorInteractableComponentMash::CleanupComponent()
{
	ActualMashAmount = 0;
	
	OnInteractableStateChanged.Broadcast(InteractableState);
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Mashed);
		GetWorld()->GetTimerManager().ClearTimer(Timer_Interaction);
	}
}

void UActorInteractableComponentMash::InteractionStarted(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	Super::InteractionStarted(TimeStarted, PressedKey, CausingInteractor);
	
	if (CanInteract())
	{
		if(!GetWorld()->GetTimerManager().IsTimerActive(Timer_Interaction))
		{
			// Force Interaction Period to be at least 0.1s
			const float TempInteractionPeriod = FMath::Max(0.1f, InteractionPeriod);
	
			FTimerDelegate Delegate_Completed;
			Delegate_Completed.BindUObject(this, &UActorInteractableComponentMash::OnInteractionCompletedCallback);

			GetWorld()->GetTimerManager().SetTimer
			(
				Timer_Interaction,
				Delegate_Completed,
				TempInteractionPeriod,
				false
			);
		}

		if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Mashed))
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Mashed);
		}
		
		FTimerDelegate Delegate_Mashed;
		Delegate_Mashed.BindUObject(this, &UActorInteractableComponentMash::OnInteractionFailedCallback);
		GetWorld()->GetTimerManager().SetTimer
		(
			TimerHandle_Mashed,
			Delegate_Mashed,
			KeystrokeTimeThreshold,
			false
		);
		
		ActualMashAmount++;

		OnKeyMashed.Broadcast();
	}
}

void UActorInteractableComponentMash::InteractionStopped(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	if (GetWorld())
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Mashed))
		{
			Super::InteractionStopped(TimeStarted, PressedKey, CausingInteractor);
		}
	}
}

void UActorInteractableComponentMash::InteractionCanceled()
{
	Super::InteractionCanceled();

	CleanupComponent();
}

void UActorInteractableComponentMash::InteractionCompleted(const float& TimeCompleted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	if (ActualMashAmount >= MinMashAmountRequired)
	{
		Super::InteractionCompleted(TimeCompleted, CausingInteractor);
	}
	else
	{
		OnInteractionFailed.Broadcast();
	}

	CleanupComponent();
}

int32 UActorInteractableComponentMash::GetMinMashAmountRequired() const
{ return MinMashAmountRequired; }

void UActorInteractableComponentMash::SetMinMainAmountRequired(const int32 Value)
{
	MinMashAmountRequired = FMath::Max(Value, 2);
}

float UActorInteractableComponentMash::GetKeystrokeTimeThreshold() const
{ return KeystrokeTimeThreshold; }

void UActorInteractableComponentMash::SetKeystrokeTimeThreshold(const float Value)
{
	KeystrokeTimeThreshold = FMath::Max(Value, 0.01f);
}

#undef LOCTEXT_NAMESPACE
