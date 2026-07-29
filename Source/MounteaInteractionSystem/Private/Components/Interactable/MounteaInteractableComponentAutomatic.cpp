// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Components/Interactable/MounteaInteractableComponentAutomatic.h"
#include "Interfaces/MounteaInteractorInterface.h"
#include "TimerManager.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableComponentAutomatic"

UMounteaInteractableComponentAutomatic::UMounteaInteractableComponentAutomatic()
{
	// Automatic is standalone — no cycling, triggers immediately
	bStandaloneInteraction = true;
}

void UMounteaInteractableComponentAutomatic::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	Super::InteractionStarted_Implementation(TimeStarted, CausingInteractor);

	// Only authority schedules the completion timer
	if (!GetOwner() || !GetOwner()->HasAuthority())
				return;
	if (!GetWorld())
				return;

	const float period = FMath::Max(0.01f, InteractionPeriod);

	GetWorld()->GetTimerManager().SetTimer(Timer_AutoCompletion, this,
		&UMounteaInteractableComponentAutomatic::OnCompletionTimerExpired, period, false);
}

void UMounteaInteractableComponentAutomatic::InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(Timer_AutoCompletion);
	}

	Super::InteractionStopped_Implementation(TimeStarted, CausingInteractor);
}

void UMounteaInteractableComponentAutomatic::OnCompletionTimerExpired()
{
	FString Err;
	Execute_CompleteInteractable(this, Err);
}

#undef LOCTEXT_NAMESPACE
