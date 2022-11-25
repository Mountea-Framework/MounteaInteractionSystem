// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentHold.h"

#include "Interfaces/ActorInteractorInterface.h"

#if WITH_EDITOR
#include "Helpers/ActorInteractionPluginLog.h"
#endif

#define LOCTEXT_NAMESPACE "InteractableComponentHold"

UActorInteractableComponentHold::UActorInteractableComponentHold()
{
	PrimaryComponentTick.bCanEverTick = true;

	InteractionPeriod = 3.f;
	InteractableName = LOCTEXT("InteractableComponentHold", "Hold");
}

void UActorInteractableComponentHold::BeginPlay()
{
	Super::BeginPlay();
}

void UActorInteractableComponentHold::InteractionStarted(const float& TimeStarted, const FKey& PressedKey)
{
	if (CanInteract())
	{
		// Force Interaction Period to be at least 0.1s
		const float TempInteractionPeriod = FMath::Max(0.1f, InteractionPeriod);

		if (!GetWorld()) return;
	
		FTimerDelegate Delegate;

		auto Interactable = this;
		Delegate.BindLambda([Interactable]()
		{
			if (!Interactable)
			{
				AIntP_LOG(Error, TEXT("[InteractionStarted] Interactable is null, cannot request OnInteractionCompleted!"))
				return;
			}
			if (!Interactable->GetWorld())
			{
				AIntP_LOG(Error, TEXT("[InteractionStarted] Interactable has no World, cannot request OnInteractionCompleted!"))
				return;
			
			}
			AIntP_LOG(Display, TEXT("[InteractionStarted] Interactable requested OnInteractionCompleted!"))
			Interactable->OnInteractionCompleted.Broadcast(Interactable->GetWorld()->GetTimeSeconds());
		});

		GetWorld()->GetTimerManager().SetTimer
		(
			Timer_Interaction,
			Delegate,
			TempInteractionPeriod,
			false
		);
		
		Super::InteractionStarted(TimeStarted, PressedKey);
	}
}

void UActorInteractableComponentHold::InteractionStopped(const float& TimeStarted, const FKey& PressedKey)
{
	Super::InteractionStopped(TimeStarted, PressedKey);
}

void UActorInteractableComponentHold::InteractionCanceled()
{
	Super::InteractionCanceled();
}

void UActorInteractableComponentHold::InteractorFound(const TScriptInterface<IActorInteractorInterface>& FoundInteractor)
{
	Super::InteractorFound(FoundInteractor);
}

void UActorInteractableComponentHold::InteractorLost(const TScriptInterface<IActorInteractorInterface>& LostInteractor)
{
	Super::InteractorLost(LostInteractor);
}

float UActorInteractableComponentHold::GetInteractionProgress() const
{
	return Super::GetInteractionProgress();
}

#undef LOCTEXT_NAMESPACE