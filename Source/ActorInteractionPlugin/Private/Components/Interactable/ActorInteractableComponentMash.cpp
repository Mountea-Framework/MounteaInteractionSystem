// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "Components/Interactable/ActorInteractableComponentMash.h"

#include "TimerManager.h"
#include "Helpers/ActorInteractionPluginLog.h"

#define LOCTEXT_NAMESPACE "ActorInteractableComponentMash"

UActorInteractableComponentMash::UActorInteractableComponentMash() :
		MinMashAmountRequired(5),
		KeystrokeTimeThreshold(1.f),
		ActualMashAmount(0)
{
	bInteractionHighlight = true;
	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = 3.f;
	InteractableName = NSLOCTEXT("ActorInteractableComponentMash", "Mash", "Mash");
}

void UActorInteractableComponentMash::BeginPlay()
{
	Super::BeginPlay();

	OnInteractionFailed.		AddUniqueDynamic(this, &UActorInteractableComponentMash::InteractionFailed);
	OnKeyMashed.				AddUniqueDynamic(this, &UActorInteractableComponentMash::OnKeyMashedEvent);
}

void UActorInteractableComponentMash::InteractionFailed()
{
	Execute_SetState(this, Execute_GetDefaultState(this));
	
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
		if (Execute_TriggerCooldown(this)) return;
	}
	
	OnInteractionCompleted.Broadcast(GetWorld()->GetTimeSeconds(), Execute_GetInteractor(this));
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

void UActorInteractableComponentMash::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	Super::InteractionStarted_Implementation(TimeStarted, CausingInteractor);
	
	if (Execute_CanInteract(this))
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

void UActorInteractableComponentMash::InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	if (GetWorld())
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Mashed))
		{
			Super::InteractionStopped_Implementation(TimeStarted, CausingInteractor);
		}
	}
}

void UActorInteractableComponentMash::InteractionCanceled_Implementation()
{
	Super::InteractionCanceled_Implementation();

	CleanupComponent();
}

void UActorInteractableComponentMash::InteractionCompleted_Implementation(const float& TimeCompleted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	if (ActualMashAmount >= MinMashAmountRequired)
	{
		Super::InteractionCompleted_Implementation(TimeCompleted, CausingInteractor);
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
