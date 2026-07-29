// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Components/Interactable/MounteaInteractableComponentMash.h"
#include "Interfaces/MounteaInteractorInterface.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableComponentMash"

UMounteaInteractableComponentMash::UMounteaInteractableComponentMash()
{
	MinMashAmountRequired = 5;
	KeystrokeTimeThreshold = 1.f;
	ActualMashAmount = 0;
}

void UMounteaInteractableComponentMash::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMounteaInteractableComponentMash, ActualMashAmount);
}

void UMounteaInteractableComponentMash::SetMinMashAmountRequired(const int32 Value)
{
	MinMashAmountRequired = FMath::Max(2, Value);
}

void UMounteaInteractableComponentMash::SetKeystrokeTimeThreshold(const float Value)
{
	KeystrokeTimeThreshold = FMath::Max(0.01f, Value);
}

void UMounteaInteractableComponentMash::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	// Each call to InteractionStarted is a key press (interactor calls this per press)
	Super::InteractionStarted_Implementation(TimeStarted, CausingInteractor);

	// Authority tracks and validates the mash count
	if (!GetOwner() || !GetOwner()->HasAuthority())
				return;
	if (!GetWorld())
				return;

	++ActualMashAmount;
	OnRep_ActualMashAmount();

	OnKeyMashed.Broadcast();
	OnKeyMashedEvent();

	// Reset keystroke threshold timer on each press
	GetWorld()->GetTimerManager().SetTimer(Timer_KeystrokeThreshold, this,
		&UMounteaInteractableComponentMash::OnKeystrokeThresholdExpired, KeystrokeTimeThreshold, false);

	if (ActualMashAmount >= MinMashAmountRequired)
	{
		GetWorld()->GetTimerManager().ClearTimer(Timer_KeystrokeThreshold);
		FString err;
		Execute_CompleteInteractable(this, err);
	}
}

void UMounteaInteractableComponentMash::InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	// Key release doesn't cancel mash — only threshold timeout or stopping interaction does
	Super::InteractionStopped_Implementation(TimeStarted, CausingInteractor);
}

void UMounteaInteractableComponentMash::InteractionCanceled_Implementation()
{
	ResetMashState();
	Super::InteractionCanceled_Implementation();
}

void UMounteaInteractableComponentMash::OnKeystrokeThresholdExpired()
{
	// Too slow between presses — interaction failed
	OnInteractionFailed.Broadcast();
	OnInteractionFailedEvent();
	Execute_InteractionCanceled(this);
}

void UMounteaInteractableComponentMash::ResetMashState()
{
	ActualMashAmount = 0;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(Timer_KeystrokeThreshold);
	}
}

void UMounteaInteractableComponentMash::OnRep_ActualMashAmount()
{
	// Clients update progress display — GetInteractionProgress() is overridden to use mash ratio
}

#undef LOCTEXT_NAMESPACE
