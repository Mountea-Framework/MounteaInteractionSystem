// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "ActorInteractorComponentBase.h"
#include "ActorInteractorComponentOverlap.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionShapeAdded, UPrimitiveComponent*, AddedComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionShapeRemoved, UPrimitiveComponent*, RemovedComponent);

/**
 * 
 */
UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation, Rendering), meta=(BlueprintSpawnableComponent, DisplayName = "Interactor Component Overlap"))
class ACTORINTERACTIONPLUGIN_API UActorInteractorComponentOverlap : public UActorInteractorComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UActorInteractorComponentOverlap();

protected:

	virtual void BeginPlay() override;

	virtual void SetState(const EInteractorStateV2 NewState) override;

	virtual UPrimitiveComponent* FindComponentByName(const FName& CollisionComponentName);
	virtual void SetupInteractorOverlap();
	virtual void BindCollisions();
	virtual void BindCollision(UPrimitiveComponent* Component);
	virtual void UnbindCollisions();
	virtual void UnbindCollision(UPrimitiveComponent* Component);
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddCollisionComponent(UPrimitiveComponent* CollisionComponent);
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddCollisionComponents(TArray<UPrimitiveComponent*> CollisionComponents);
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveCollisionComponent(UPrimitiveComponent* CollisionComponent);
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveCollisionComponents(TArray<UPrimitiveComponent*> CollisionComponents);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual	TArray<UPrimitiveComponent*> GetCollisionComponents() const;

protected:

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionShapeAdded OnCollisionShapeAdded;

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionShapeRemoved OnCollisionShapeRemoved;
	
protected:

	UPROPERTY(EditAnywhere, Category="Interaction|Optional")
	TArray<FName> OverrideCollisionComponents;
	
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only", meta=(DisplayThumbnail = false, ShowOnlyInnerProperties))
	TArray<UPrimitiveComponent*> CollisionShapes;

private:
	
	/**
	 * Cached Collision Shape Settings.
	 * Filled when Collision Shapes are registered.
	 * Once Collision Shape is unregistered, it reads its cached settings and returns to pre-interaction Collision Settings.
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only", meta=(DisplayThumbnail = false, ShowOnlyInnerProperties))
	mutable TMap<UPrimitiveComponent*, FCollisionShapeCache> CachedCollisionShapesSettings;
	
};

