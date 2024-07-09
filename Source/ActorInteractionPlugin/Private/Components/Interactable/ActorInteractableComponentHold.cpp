// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "Components/Interactable/ActorInteractableComponentHold.h"
#include "TimerManager.h"
#include "Interfaces/ActorInteractorInterface.h"
#include "TimerManager.h"
#include "Helpers/ActorInteractionPluginLog.h"
#include "Helpers/MounteaInteractionSystemBFL.h"

#define LOCTEXT_NAMESPACE "InteractableComponentHold"

UActorInteractableComponentHold::UActorInteractableComponentHold()
{
	bInteractionHighlight = true;
	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = 3.f;
	InteractableName = NSLOCTEXT("InteractableComponentHold", "Hold", "Hold");
}

void UActorInteractableComponentHold::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (!GetWorld()) return;
	
		if (!Execute_CanInteract(this))
			return;
		
		// Force Interaction Period to be at least 0.1s
		const float TempInteractionPeriod = FMath::Max(0.1f, InteractionPeriod);

		// Either unpause or start from start
		if (bCanPersist && GetWorld()->GetTimerManager().IsTimerPaused(Timer_Interaction))
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

		Super::InteractionStarted_Implementation(TimeStarted, CausingInteractor);
	}
}

void UActorInteractableComponentHold::OnInteractionCompletedCallback()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (!GetWorld())
		{
			LOG_WARNING(TEXT("[OnInteractionCompletedCallback] No World, this is bad!"))
			OnInteractionCanceled.Broadcast();
			return;
		}

		if (UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(GetWorld()))
		{
			ProcessToggleActive(false);
		}
		else
		{
			ProcessToggleActive_Client(false);
		}
		
		if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
		{
			if (Execute_TriggerCooldown(this)) return;
		}
	
		OnInteractionCompleted.Broadcast(GetWorld()->GetTimeSeconds(), Execute_GetInteractor(this));
	}
}

#undef LOCTEXT_NAMESPACE
