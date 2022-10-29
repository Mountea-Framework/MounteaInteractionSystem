// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractableComponentBase.h"
#include "ActorInteractionComponentOverlap.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Overlap"))
class ACTORINTERACTIONPLUGIN_API UActorInteractionComponentOverlap : public UActorInteractableComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractionComponentOverlap();

protected:

	virtual void BeginPlay() override;

#pragma region InteractableFunctions
	
	virtual void OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	virtual void OnInteractableStopOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const override;
	virtual void UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const override;

#pragma endregion 
};
