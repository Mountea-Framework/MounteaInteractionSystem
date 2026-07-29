// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Components/Interactable/MounteaInteractableComponentHold.h"
#include "Interfaces/MounteaInteractorInterface.h"
#include "TimerManager.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableComponentHold"

UMounteaInteractableComponentHold::UMounteaInteractableComponentHold()
{
}

void UMounteaInteractableComponentHold::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	Super::InteractionStarted_Implementation(TimeStarted, CausingInteractor);

	// Only authority drives the hold timer
	if (!GetOwner() || !GetOwner()->HasAuthority())
				return;
	if (!GetWorld())
				return;

	const float period = FMath::Max(0.01f, InteractionPeriod);

	GetWorld()->GetTimerManager().SetTimer(Timer_Hold, this,
		&UMounteaInteractableComponentHold::OnHoldCompleted, period, false);
}

void UMounteaInteractableComponentHold::InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (GetWorld())
	{
		// If hold was not completed, the stop counts as a cancel
		if (GetWorld()->GetTimerManager().IsTimerActive(Timer_Hold))
		{
			GetWorld()->GetTimerManager().ClearTimer(Timer_Hold);
			Execute_InteractionCanceled(this);
		}
	}

	Super::InteractionStopped_Implementation(TimeStarted, CausingInteractor);
}

void UMounteaInteractableComponentHold::OnHoldCompleted()
{
	FString err;
	Execute_CompleteInteractable(this, err);
}

#undef LOCTEXT_NAMESPACE
