// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "Components/Interactable/MounteaInteractableComponentMash.h"

#include "TimerManager.h"
#include "Helpers/MounteaInteractionSystemLog.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableComponentMash"

UMounteaInteractableComponentMash::UMounteaInteractableComponentMash() :
		MinMashAmountRequired(5),
		KeystrokeTimeThreshold(1.f),
		ActualMashAmount(0)
{
	bInteractionHighlight = true;
	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = 3.f;
	InteractableName = NSLOCTEXT("MounteaInteractableComponentMash", "Mash", "Mash");
}

void UMounteaInteractableComponentMash::BeginPlay()
{
	Super::BeginPlay();

	OnInteractionFailed.		AddUniqueDynamic(this, &UMounteaInteractableComponentMash::InteractionFailed);
	OnKeyMashed.				AddUniqueDynamic(this, &UMounteaInteractableComponentMash::OnKeyMashedEvent);
}

void UMounteaInteractableComponentMash::InteractionFailed()
{
	Execute_SetState(this, Execute_GetDefaultState(this));
	
	CleanupComponent();
	
	OnInteractionFailedEvent();
}

void UMounteaInteractableComponentMash::OnInteractionFailedCallback()
{
	OnInteractionFailed.Broadcast();
}

void UMounteaInteractableComponentMash::OnInteractionCompletedCallback()
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
	
	if (ActualMashAmount >= MinMashAmountRequired)
	{
		OnInteractionCompleted.Broadcast(GetWorld()->GetTimeSeconds(), Execute_GetInteractor(this));
	}
	else
	{
		OnInteractionFailed.Broadcast();
	}

	CleanupComponent();
}

void UMounteaInteractableComponentMash::CleanupComponent()
{
	ActualMashAmount = 0;
	
	OnInteractableStateChanged.Broadcast(InteractableState);
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Mashed);
		GetWorld()->GetTimerManager().ClearTimer(Timer_Interaction);
	}
}

void UMounteaInteractableComponentMash::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	Super::InteractionStarted_Implementation(TimeStarted, CausingInteractor);
	
	if (Execute_CanInteract(this))
	{
		if(!GetWorld()->GetTimerManager().IsTimerActive(Timer_Interaction))
		{
			// Force Interaction Period to be at least 0.1s
			const float TempInteractionPeriod = FMath::Max(0.1f, InteractionPeriod);
	
			FTimerDelegate Delegate_Completed;
			Delegate_Completed.BindUObject(this, &UMounteaInteractableComponentMash::OnInteractionCompletedCallback);

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
		Delegate_Mashed.BindUObject(this, &UMounteaInteractableComponentMash::OnInteractionFailedCallback);
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

void UMounteaInteractableComponentMash::InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (GetWorld())
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Mashed))
		{
			Super::InteractionStopped_Implementation(TimeStarted, CausingInteractor);
		}
	}
}

void UMounteaInteractableComponentMash::InteractionCanceled_Implementation()
{
	Super::InteractionCanceled_Implementation();

	CleanupComponent();
}

int32 UMounteaInteractableComponentMash::GetMinMashAmountRequired() const
{ return MinMashAmountRequired; }

void UMounteaInteractableComponentMash::SetMinMainAmountRequired(const int32 Value)
{
	MinMashAmountRequired = FMath::Max(Value, 2);
}

float UMounteaInteractableComponentMash::GetKeystrokeTimeThreshold() const
{ return KeystrokeTimeThreshold; }

void UMounteaInteractableComponentMash::SetKeystrokeTimeThreshold(const float Value)
{
	KeystrokeTimeThreshold = FMath::Max(Value, 0.01f);
}

#undef LOCTEXT_NAMESPAC