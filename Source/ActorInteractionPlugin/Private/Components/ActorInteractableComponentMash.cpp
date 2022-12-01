// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentMash.h"

#define LOCTEXT_NAMESPACE "ActorInteractableComponentMash"

UActorInteractableComponentMash::UActorInteractableComponentMash()
{
	MinMashAmountRequired = 5;
	KeystrokeTimeThreshold = 0.1f;
	ActualMashAmount = 0;
	InteractableName = LOCTEXT("ActorInteractableComponentMash", "Mash");
}

void UActorInteractableComponentMash::BeginPlay()
{
	Super::BeginPlay();

	OnInteractionFailed.AddUniqueDynamic(this, &UActorInteractableComponentMash::InteractionFailed);
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

void UActorInteractableComponentMash::CleanUpComponent()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Mashed);
	}

	ActualMashAmount = 0;
}

void UActorInteractableComponentMash::InteractionStarted(const float& TimeStarted, const FKey& PressedKey)
{
	Super::InteractionStarted(TimeStarted, PressedKey);

	if (GetWorld())
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Mashed))
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Mashed);
		}
		
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &UActorInteractableComponentMash::OnInteractionFailedCallback);
		GetWorld()->GetTimerManager().SetTimer
		(
			TimerHandle_Mashed,
			Delegate,
			KeystrokeTimeThreshold,
			false
		);

		ActualMashAmount++;
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
