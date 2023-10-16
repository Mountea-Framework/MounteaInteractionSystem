// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/ActorInteractableComponent.h"

UDEPRECATED_ActorInteractableComponent::UDEPRECATED_ActorInteractableComponent()
{
	UActorComponent::SetComponentTickEnabled(false);
	SetIsReplicatedByDefault(false);
}
