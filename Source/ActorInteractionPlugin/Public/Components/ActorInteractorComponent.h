// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ActorInteractorComponent.generated.h"


/**
 * DEPRECATED.
 * Use ActorInteractorComponentBase classes instead.
 * 
 * Implement an Actor component for interaction.
 *
 * An Interactor Component is a non-transient component that enables its owning Actor to interact with Interactable Actors.
 * Interaction is processed by Interactor Component provided with base data from the Interactable Component.
 * Dependency injection is achieved by using IInteractorComponentInterface instead of direct class usage.
 * 
 * @warning 	Interactor Actor Component is using "ActorEyesViewPoint" by default. To change this value, go to Actor -> Base Eye Height.
 * - Higher the value, higher the tracing start point will be located.
 *
 * @warning Networking is not implemented.
 *
 * @see https://sites.google.com/view/dominikpavlicek/home/documentation
 */
UCLASS(
	Deprecated,
	ClassGroup=(Interaction), Blueprintable, meta=(BlueprintSpawnableComponent, DisplayName = "Interactor Component",
	DeprecationMessage="This class is deprecated. Use `InteractorComponentBase` instead.\n In Blueprints use:\n* BPC Interactor Trace\n* BPC Interactor Overlap")
)
class ACTORINTERACTIONPLUGIN_API UDEPRECATED_ActorInteractorComponent final : public UActorComponent
{
	GENERATED_BODY()
	
	UDEPRECATED_ActorInteractorComponent();
};
