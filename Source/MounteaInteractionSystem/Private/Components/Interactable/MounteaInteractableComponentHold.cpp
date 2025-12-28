// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "Components/Interactable/MounteaInteractableComponentHold.h"
#include "TimerManager.h"
#include "Interfaces/MounteaInteractorInterface.h"
#include "TimerManager.h"
#include "Helpers/MounteaInteractionSystemLog.h"
#include "Helpers/MounteaInteractionSystemBFL.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableComponentHold"

UMounteaInteractableComponentHold::UMounteaInteractableComponentHold()
{
	bInteractionHighlight = true;
	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = 3.f;
	InteractableName = NSLOCTEXT("MounteaInteractableComponentHold", "Hold", "Hold");
}

void UMounteaInteractableComponentHold::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
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
			Delegate.BindUObject(this, &UMounteaInteractableComponentHold::OnInteractionCompletedCallback);

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

void UMounteaInteractableComponentHold::OnInteractionCompletedCallback()
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

#undef LOCTEXT_NAMESPAC