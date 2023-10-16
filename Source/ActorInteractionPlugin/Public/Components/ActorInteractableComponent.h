// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"

#include "ActorInteractableComponent.generated.h"

#define LOCTEXT_NAMESPACE "ActorInteractionPlugin"


/**
 * DEPRECATED
 * Use UActorInteractableComponentBase classes instead.
 * 
 * Implement an Actor component for being interacted with.
 * 
 * An Interactable Component is a non-transient component that enables other Actors, who posses an Interactor Component, to interact.
 * Interaction is processed by Interactor Component provided with base data from the Interactable Component.
 * 
 * All logic, like hiding and setting values, should be done in Blueprint classes to keep this Widget Component as flexible as possible.
 * 
 * @warning Networking is not implemented!
 * 
 * @see https://sites.google.com/view/dominikpavlicek/home/documentation
 */
UCLASS(
	Deprecated,
	ClassGroup=(Interaction), NotBlueprintable, HideCategories = (Navigation, Physics, Collision, Lighting, Rendering, Mobile, Animation, HLOD),
	meta=(BlueprintSpawnableComponent, DisplayName="Interactable Component",
	DeprecationMessage="This class is deprecated. Use `InteractableComponentBase` instead.\n In Blueprints use:\n* BPC Interactable Auto\n* BPC Interactable Hold\n* BPC Interactable Press\n* BPC Interactable Mash")
)
class ACTORINTERACTIONPLUGIN_API UDEPRECATED_ActorInteractableComponent final : public UWidgetComponent
{
	GENERATED_BODY()
	
	UDEPRECATED_ActorInteractableComponent();
	
};

#undef LOCTEXT_NAMESPACE
