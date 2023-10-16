// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/ActorInteractorComponent.h"

UDEPRECATED_ActorInteractorComponent::UDEPRECATED_ActorInteractorComponent()
{
	// Tick only if ticking is required
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}