// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractableComponentBase.h"
#include "ActorInteractableComponentHold.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentHold : public UActorInteractableComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractableComponentHold();

protected:

	virtual void BeginPlay() override;

	virtual void InteractionStarted(const float& TimeStarted) override;
};
