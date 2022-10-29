// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractableComponentBase.h"
#include "ActorInteractionComponentTrace.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Overlap"))
class ACTORINTERACTIONPLUGIN_API UActorInteractionComponentTrace : public UActorInteractableComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractionComponentTrace();

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region InteractableFunctions
	
	virtual void OnInteractableTraced(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
	virtual void BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const override;
	virtual void UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const override;

#pragma endregion 
};
