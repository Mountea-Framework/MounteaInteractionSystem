// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentMash.h"

#include "Helpers/ActorInteractionPluginLog.h"

#define LOCTEXT_NAMESPACE "ActorInteractableComponentMash"

UActorInteractableComponentMash::UActorInteractableComponentMash()
{
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
	SetState(EInteractableStateV2::EIS_Awake);
	
	CleanUpComponent();
	
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
	
	OnInteractionCompleted.Broadcast(GetWorld()->GetTimeSeconds());
}

void UActorInteractableComponentMash::CleanUpComponent()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Mashed);
		GetWorld()->GetTimerManager().ClearTimer(Timer_Interaction);
	}

	ActualMashAmount = 0;
}

void UActorInteractableComponentMash::InteractionStarted(const float& TimeStarted, const FKey& PressedKey)
{
	Super::InteractionStarted(TimeStarted, PressedKey);

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

void UActorInteractableComponentMash::InteractionStopped(const float& TimeStarted, const FKey& PressedKey)
{
	if (GetWorld())
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Mashed))
		{
			Super::InteractionStopped(TimeStarted, PressedKey);
		}
	}
}

void UActorInteractableComponentMash::InteractionCanceled()
{
	Super::InteractionCanceled();

	CleanUpComponent();
}

void UActorInteractableComponentMash::InteractionCompleted(const float& TimeCompleted)
{
	if (ActualMashAmount >= MinMashAmountRequired)
	{
		Super::InteractionCompleted(TimeCompleted);
	}
	else
	{
		OnInteractionFailed.Broadcast();
	}

	CleanUpComponent();
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
