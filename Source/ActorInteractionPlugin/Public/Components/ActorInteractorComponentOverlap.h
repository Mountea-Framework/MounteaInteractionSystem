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

	virtual FString ToString_Implementation() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	
	virtual void SetupInteractorOverlap();
	virtual void BindCollisions();
	
	virtual void BindCollision(UPrimitiveComponent* Component);
	virtual void UnbindCollisions();
	virtual void UnbindCollision(UPrimitiveComponent* Component);

protected:
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction|Tracing")
	void ProcessOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& SweepResult, const bool bOverlapStarted);
	virtual void ProcessOverlap_Implementation(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& SweepResult, const bool bOverlapStarted);

	UFUNCTION()
	void StartInteractorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void StopInteractorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void HandleStartOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& HitResult);
	void HandleEndOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp);

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
	TArray<UPrimitiveComponent*> GetCollisionComponents() const;

	
	/**
	 * 
	 * @param bEnable						
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction|Tracing")
	void UseSafetyTrace(bool bEnable);
	virtual void UseSafetyTrace_Implementation(bool bEnable);

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	bool DoesUseSafetyTrace() const;

	/**
	 * 
	 * @param NewSafetyChannel		
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction|Tracing")
	void SetValidationCollisionChannel(const ECollisionChannel NewSafetyChannel);
	void SetValidationCollisionChannel_Implementation(const ECollisionChannel NewSafetyChannel);

	/**
	 * 
	 * @return									
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	ECollisionChannel GetValidationCollisionChannel() const;
	
protected:

	virtual void ProcessStateChanged() override;

protected:

	UFUNCTION(Server, Unreliable)
	void AddCollisionComponent_Server(UPrimitiveComponent* CollisionComponent);

	UFUNCTION(Server, Unreliable)
	void AddCollisionComponents_Server(const TArray<UPrimitiveComponent*>& CollisionComponents);

	UFUNCTION(Server, Unreliable)
	void RemoveCollisionComponent_Server(UPrimitiveComponent* CollisionComponent);

	UFUNCTION(Server, Unreliable)
	void RemoveCollisionComponents_Server(const TArray<UPrimitiveComponent*>& CollisionComponents);

	UFUNCTION(Server, Reliable)
	void ProcessOverlap_Server(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& SweepResult, const bool bOverlapStarted);

	UFUNCTION(Server, Reliable)
	void StartInteractorOverlap_Server(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable)
	void StopInteractorOverlap_Server(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Server, Unreliable)
	void UseSafetyTrace_Server(bool bEnable);

	UFUNCTION(Server, Unreliable)
	void SetValidationCollisionChannel_Server(const ECollisionChannel NewSafetyChannel);

public:

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionShapeAdded																		OnCollisionShapeAdded;

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionShapeRemoved																	OnCollisionShapeRemoved;
	
protected:

	UPROPERTY(EditAnywhere, Category="Interaction|Optional")
	TArray<FName>																					OverrideCollisionComponents;

	/**
	 * Used for validation. If allowed a single LineTrace will be shot from Owner's location towards Overlapped Actor. `ValidationCollisionChannel` is used for this check.
	 * This is useful to prevent overlapping collision which is clipping through a wall, for instance.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	uint8																									bUseSafetyTrace : 1;
	
	/**
	 * Additional collision channel used for validation Trace after initial overlap.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault), meta=(EditCondition="bUseSafetyTrace == true"))
	TEnumAsByte<ECollisionChannel>														ValidationCollisionChannel;

	/**
	 * A list of Collision Shapes that are used as interactors.
	 * List is populated on Server Side only!
	 */
	UPROPERTY(Replicated, SaveGame, VisibleAnywhere, Category="Interaction|Read Only", meta=(DisplayThumbnail = false, ShowOnlyInnerProperties))
	TArray<TObjectPtr<UPrimitiveComponent>>										CollisionShapes;

private:
	
	/**
	 * Cached Collision Shape Settings.
	 * Filled when Collision Shapes are registered.
	 * Once Collision Shape is unregistered, it reads its cached settings and returns to pre-interaction Collision Settings.
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only", meta=(DisplayThumbnail = false, ShowOnlyInnerProperties))
	mutable TMap<UPrimitiveComponent*, FCollisionShapeCache>			CachedCollisionShapesSettings;
	
};