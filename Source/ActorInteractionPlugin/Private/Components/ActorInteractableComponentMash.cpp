// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentMash.h"

#define LOCTEXT_NAMESPACE "ActorInteractableComponentMash"

UActorInteractableComponentMash::UActorInteractableComponentMash()
{
	InteractableName = LOCTEXT("ActorInteractableComponentMash", "Mash");
}

void UActorInteractableComponentMash::BeginPlay()
{
	Super::BeginPlay();
}

#undef LOCTEXT_NAMESPACE