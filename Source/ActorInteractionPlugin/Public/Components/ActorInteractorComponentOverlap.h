// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractorComponentBase.h"
#include "ActorInteractorComponentOverlap.generated.h"


UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation, Rendering), meta=(BlueprintSpawnableComponent, DisplayName = "Interactor Component Trace"))
class ACTORINTERACTIONPLUGIN_API UActorInteractorComponentOverlap : public UActorInteractorComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractorComponentOverlap();
};
