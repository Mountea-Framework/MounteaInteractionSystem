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
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName = "Interactor Component Overlap"))
class ACTORINTERACTIONPLUGIN_API UActorInteractorComponentOverlap : public UActorInteractorComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UActorInteractorComponentOverlap();

protected:

	virtual void BeginPlay() override;
	
	virtual void SetupInteractorOverlap();
	virtual void BindCollisions();
	virtual void BindCollision(UPrimitiveComponent* Component);
	virtual void UnbindCollisions();
	virtual void UnbindCollision(UPrimitiveComponent* Component);

public:
	
	/**
	 * 
	 * @param CollisionComponent 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void AddCollisionComponent(UPrimitiveComponent* CollisionComponent);
	virtual void AddCollisionComponent_Implementation(UPrimitiveComponent* CollisionComponent);
	
	/**
	 * 
	 * @param CollisionComponents 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void AddCollisionComponents(const TArray<UPrimitiveComponent*>& CollisionComponents);
	virtual void AddCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents);
	
	/**
	 * 
	 * @param CollisionComponent 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void RemoveCollisionComponent(UPrimitiveComponent* CollisionComponent);
	virtual void RemoveCollisionComponent_Implementation(UPrimitiveComponent* CollisionComponent);
	
	/**
	 * 
	 * @param CollisionComponents 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void RemoveCollisionComponents(const TArray<UPrimitiveComponent*>& CollisionComponents);
	virtual void RemoveCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents);
	
	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	TSet<UPrimitiveComponent*> GetCollisionComponents() const;

protected:

	virtual void ProcessStateChanges() override;

protected:

	UFUNCTION(Server, Unreliable)
	void AddCollisionComponent_Server(UPrimitiveComponent* CollisionComponent);

	UFUNCTION(Server, Unreliable)
	void AddCollisionComponents_Server(const TArray<UPrimitiveComponent*>& CollisionComponents);

	UFUNCTION(Server, Unreliable)
	void RemoveCollisionComponent_Server(UPrimitiveComponent* CollisionComponent);

	UFUNCTION(Server, Unreliable)
	void RemoveCollisionComponents_Server(const TArray<UPrimitiveComponent*>& CollisionComponents);


public:

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionShapeAdded																		OnCollisionShapeAdded;

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionShapeRemoved																	OnCollisionShapeRemoved;
	
protected:

	UPROPERTY(EditAnywhere, Category="Interaction|Optional")
	TArray<FName>																					OverrideCollisionComponents;

	/**
	 * A list of Collision Shapes that are used as interactors.
	 * List is populated on Server Side only!
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only", meta=(DisplayThumbnail = false, ShowOnlyInnerProperties))
	TSet<UPrimitiveComponent*>											CollisionShapes; // TObjectPtr breaks the Getter function, so let's keep it as pointer for now

private:
	
	/**
	 * Cached Collision Shape Settings.
	 * Filled when Collision Shapes are registered.
	 * Once Collision Shape is unregistered, it reads its cached settings and returns to pre-interaction Collision Settings.
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only", meta=(DisplayThumbnail = false, ShowOnlyInnerProperties))
	mutable TMap<UPrimitiveComponent*, FCollisionShapeCache>			CachedCollisionShapesSettings;
};