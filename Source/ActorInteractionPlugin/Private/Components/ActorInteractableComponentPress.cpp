// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentPress.h"


// Sets default values for this component's properties
UActorInteractableComponentPress::UActorInteractableComponentPress()
{
	InteractionPeriod = -1.f;
}

void UActorInteractableComponentPress::BeginPlay()
{
	Super::BeginPlay();

	SetInteractionPeriod(-1.f);
	
	Timer_Interaction.Invalidate();
}

void UActorInteractableComponentPress::InteractionStarted(const float& TimeStarted, const FKey& PressedKey)
{
	Super::InteractionStarted(TimeStarted, PressedKey);

	OnInteractionCompleted.Broadcast(TimeStarted);
}
