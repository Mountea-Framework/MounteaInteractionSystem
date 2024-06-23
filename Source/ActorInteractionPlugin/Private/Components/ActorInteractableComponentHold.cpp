// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentHold.h"
#include "TimerManager.h"
#include "Interfaces/ActorInteractorInterface.h"
#include "TimerManager.h"
#include "Helpers/ActorInteractionPluginLog.h"

#define LOCTEXT_NAMESPACE "InteractableComponentHold"

UActorInteractableComponentHold::UActorInteractableComponentHold()
{
	bInteractionHighlight = true;
	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = 3.f;
	InteractableName = LOCTEXT("InteractableComponentHold", "Hold");
}

void UActorInteractableComponentHold::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	Super::InteractionStarted(TimeStarted, CausingInteractor);
	
	if (!GetWorld()) return;
	
	if (CanInteract())
	{
		// Force Interaction Period to be at least 0.1s
		const float TempInteractionPeriod = FMath::Max(0.1f, InteractionPeriod);

		// Either unpause or start from start
		if (GetWorld()->GetTimerManager().IsTimerPaused(Timer_Interaction))
		{
			GetWorld()->GetTimerManager().UnPauseTimer(Timer_Interaction);
		}
		else
		{
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &UActorInteractableComponentHold::OnInteractionCompletedCallback);

			GetWorld()->GetTimerManager().SetTimer
			(
				Timer_Interaction,
				Delegate,
				TempInteractionPeriod,
				false
			);
		}
	}
}

void UActorInteractableComponentHold::OnInteractionCompletedCallback()
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
