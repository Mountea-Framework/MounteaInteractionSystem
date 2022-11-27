// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractableComponentBase.h"
#include "ActorInteractableComponentMash.generated.h"


UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Mash"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentMash : public UActorInteractableComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractableComponentMash();

protected:
	
	virtual void BeginPlay() override;

};
