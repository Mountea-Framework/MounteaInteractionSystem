// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentHold.h"

#if WITH_EDITOR
#include "Helpers/ActorInteractionPluginLog.h"
#endif


UActorInteractableComponentHold::UActorInteractableComponentHold()
{
	PrimaryComponentTick.bCanEverTick = true;

	InteractionPeriod = 3.f;
}

void UActorInteractableComponentHold::BeginPlay()
{
	Super::BeginPlay();
}

void UActorInteractableComponentHold::InteractionStarted(const float& TimeStarted)
{
	Super::InteractionStarted(TimeStarted);

	// Force Interaction Period to be at least 0.1s
	InteractionPeriod = FMath::Max(0.1f, InteractionPeriod);

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
	
	GetWorld()->GetTimerManager().SetTimer(Timer_Interaction, Delegate, InteractionPeriod, false);
}
