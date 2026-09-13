// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "MounteaInteractorComponentBase.h"
#include "MounteaInteractorComponentOverlap.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionShapeAdded, UPrimitiveComponent*, AddedComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionShapeRemoved, UPrimitiveComponent*, RemovedComponent);

/**
 * v5.0 Actor Interactor Overlap Component
 *
 * Detection method: UPrimitiveComponent::OnBeginOverlap / OnEndOverlap.
 * When an interactable is detected in overlap, calls AddDetected() on the base.
 * When an interactable leaves overlap, calls RemoveDetected() on the base.
 * The base's RefreshFocus() handles Tier 1/2 selection from the detected set.
 *
 * Manages a set of collision shapes on the owning actor.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactor-Component-Overlap
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName="Interactor Component Overlap"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractorComponentOverlap : public UMounteaInteractorComponentBase
{
	GENERATED_BODY()

public:

	UMounteaInteractorComponentOverlap();

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ProcessStateChanged() override;
	virtual FString ToString_Implementation() const override;

public:

	/** Add a primitive component as an interactor collision shape. Binds overlap events. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea Interaction|Interactor")
	void AddCollisionComponent(UPrimitiveComponent* CollisionComponent);
	virtual void AddCollisionComponent_Implementation(UPrimitiveComponent* CollisionComponent);

	/** Add multiple collision components. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea Interaction|Interactor")
	void AddCollisionComponents(const TArray<UPrimitiveComponent*>& CollisionComponents);
	virtual void AddCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents);

	/** Remove a collision component and unbind its overlap events. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea Interaction|Interactor")
	void RemoveCollisionComponent(UPrimitiveComponent* CollisionComponent);
	virtual void RemoveCollisionComponent_Implementation(UPrimitiveComponent* CollisionComponent);

	/** Remove multiple collision components. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea Interaction|Interactor")
	void RemoveCollisionComponents(const TArray<UPrimitiveComponent*>& CollisionComponents);
	virtual void RemoveCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents);

	/** Returns all currently registered collision shapes. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea Interaction|Interactor")
	TArray<UPrimitiveComponent*> GetCollisionComponents() const;

protected:

	virtual void SetupInteractorOverlap();
	virtual void BindCollisions();
	virtual void BindCollision(UPrimitiveComponent* Component);
	virtual void UnbindCollisions();
	virtual void UnbindCollision(UPrimitiveComponent* Component);

	UFUNCTION()
	void StartInteractorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void StopInteractorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	void HandleStartOverlap(AActor* OtherActor);
	void HandleEndOverlap(AActor* OtherActor);

	UFUNCTION(Server, Reliable)
	void AddCollisionComponent_Server(UPrimitiveComponent* CollisionComponent);

	UFUNCTION(Server, Reliable)
	void RemoveCollisionComponent_Server(UPrimitiveComponent* CollisionComponent);

	UFUNCTION(Server, Reliable)
	void StartInteractorOverlap_Server(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable)
	void StopInteractorOverlap_Server(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactor")
	FCollisionShapeAdded OnCollisionShapeAdded;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactor")
	FCollisionShapeRemoved OnCollisionShapeRemoved;

protected:

	/** Optional component tag filter for auto-setup. Only components with matching tags are added. */
	UPROPERTY(EditAnywhere, Category="Mountea Interaction|Optional")
	TArray<FName> OverrideCollisionComponents;

	/** Registered collision shapes on which overlap events are bound. */
	UPROPERTY(Replicated, SaveGame, VisibleAnywhere, Category="Mountea Interaction|Read Only")
	TArray<TObjectPtr<UPrimitiveComponent>> CollisionShapes;

private:

	UPROPERTY(SaveGame, VisibleAnywhere, Category="Mountea Interaction|Read Only")
	mutable TMap<TObjectPtr<UPrimitiveComponent>, FCollisionShapeCache> CachedCollisionShapesSettings;
};
