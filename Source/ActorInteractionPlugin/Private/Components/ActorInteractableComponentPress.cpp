// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentPress.h"

#define LOCTEXT_NAMESPACE "InteractableComponentPress"

// Sets default values for this component's properties
UActorInteractableComponentPress::UActorInteractableComponentPress()
{
	InteractionPeriod = -1.f;
	InteractableName = LOCTEXT("InteractableComponentPress", "Press");
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

	if (CanInteract())
	{
		if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
		{
			if (TriggerCooldown()) return;
		}
		
		OnInteractionCompleted.Broadcast(TimeStarted);
	}
}

#undef LOCTEXT_NAMESPACE