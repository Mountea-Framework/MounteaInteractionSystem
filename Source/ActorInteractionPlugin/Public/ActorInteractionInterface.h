#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ActorInteractionInterface.generated.h"

class UActorInteractableComponent;

UINTERFACE(BlueprintType)
class ACTORINTERACTIONPLUGIN_API UActorInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class ACTORINTERACTIONPLUGIN_API IActorInteractionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool CanInteract(const UActorInteractableComponent* Interactable, const AActor* With);
};
