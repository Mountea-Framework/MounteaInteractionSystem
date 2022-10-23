// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UActorInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IActorInteractorInterface;
enum class EInteractableStateV2 : uint8;
enum class EInteractableLifecycle : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorFound, const TScriptInterface<IActorInteractorInterface>&, FoundInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorLost, const TScriptInterface<IActorInteractorInterface>&, LostInteractor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FInteractorTraced, UPrimitiveComponent*, HitComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, FVector, NormalImpulse, const FHitResult&, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FInteractorOverlapped, UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex, bool, bFromSweep, const FHitResult &, SweepResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FInteractorStopOverlap, UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionCompleted, const float&, FinishTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionStarted, const float&, StartTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionStopped);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLifecycleCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCooldownCompleted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableAutoSetupChanged, const bool, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableWeightChanged, const int32&, NewWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableStateChanged, const EInteractableStateV2&, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableOwnerChanged, const AActor*, NewOwner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableCollisionChannelChanged, const ECollisionChannel, NewCollisionChannel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLifecycleModeChanged, const EInteractableLifecycle&, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLifecycleCountChanged, const int32, NewLifecycleCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownPeriodChanged, const float, NewCooldownPeriod);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorChanged, const TScriptInterface<IActorInteractorInterface>&, NewInteractor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightableComponentAdded, const UMeshComponent*, NewHighlightableComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionComponentAdded, const UPrimitiveComponent*, NewCollisionComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightableComponentRemoved, const UMeshComponent*, RemovedHighlightableComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionComponentRemoved, const UPrimitiveComponent*, RemovedCollisionComp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightableOverrideAdded, const FName, NewTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionOverrideAdded, const FName, NewTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightableOverrideRemoved, const FName, RemovedTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionOverrideRemoved, const FName, RemovedTag);

/**
 * 
 */
class ACTORINTERACTIONPLUGIN_API IActorInteractableInterface
{
	GENERATED_BODY()

public:

	virtual bool DoesAutoSetup() const = 0;
	virtual void ToggleAutoSetup(const bool NewValue) = 0;

	virtual bool ActivateInteractable(FString& ErrorMessage) = 0;
	virtual bool WakeUpInteractable(FString& ErrorMessage) = 0;
	virtual bool SnoozeInteractable(FString& ErrorMessage) = 0;
	virtual bool CompleteInteractable(FString& ErrorMessage) = 0;
	virtual void DeactivateInteractable() = 0;

	virtual void InteractorFound(const TScriptInterface<IActorInteractorInterface>& FoundInteractor) = 0;
	virtual void InteractorLost(const TScriptInterface<IActorInteractorInterface>& LostInteractor) = 0;

	virtual void InteractionCompleted(const float& TimeCompleted) = 0;
	virtual void InteractionStarted(const float& TimeStarted) = 0;
	virtual void InteractionStopped() = 0;
	virtual void InteractionLifecycleCompleted() = 0;
	virtual void InteractionCooldownCompleted() = 0;

	
	
	virtual EInteractableStateV2 GetState() const = 0;
	virtual void SetState(const EInteractableStateV2 NewState) = 0;
	
	virtual TScriptInterface<IActorInteractorInterface> GetInteractor() const = 0;
	virtual void SetInteractor(const TScriptInterface<IActorInteractorInterface> NewInteractor) = 0;

	virtual int32 GetInteractableWeight() const = 0;
	virtual void SetInteractableWeight(const int32 NewWeight) = 0;

	virtual AActor* GetInteractableOwner() const = 0;
	virtual void SetInteractableOwner(AActor* NewOwner) = 0;

	virtual ECollisionChannel GetCollisionChannel() const = 0;
	virtual void SetCollisionChannel(const ECollisionChannel& NewChannel) = 0;

	virtual EInteractableLifecycle GetLifecycleMode() const = 0;
	virtual void SetLifecycleMode(const EInteractableLifecycle& NewMode) = 0;

	virtual int32 GetLifecycleCount() const = 0;
	virtual void SetLifecycleCount(const int32 NewLifecycleCount) = 0;

	virtual float GetCooldownPeriod() const = 0;
	virtual void SetCooldownPeriod(const float NewCooldownPeriod) = 0;
	
	

	virtual TArray<UPrimitiveComponent*> GetCollisionComponents() const = 0;
	virtual void AddCollisionComponent(UPrimitiveComponent* CollisionComp) = 0;
	virtual void AddCollisionComponents(const TArray<UPrimitiveComponent*> CollisionComponents) = 0;
	virtual void RemoveCollisionComponent(UPrimitiveComponent* CollisionComp) = 0;
	virtual void RemoveCollisionComponents(const TArray<UPrimitiveComponent*> CollisionComponents) = 0;

	virtual TArray<UMeshComponent*> GetHighlightableComponents() const = 0;
	virtual void AddHighlightableComponent(UMeshComponent* HighlightableComp) = 0;
	virtual void AddHighlightableComponents(const TArray<UMeshComponent*> HighlightableComponents) = 0;
	virtual void RemoveHighlightableComponent(UMeshComponent* HighlightableComp) = 0;
	virtual void RemoveHighlightableComponents(const TArray<UMeshComponent*> HighlightableComponents) = 0;
	

	
	virtual UMeshComponent* FindMeshByTag(const FName Tag) const = 0;
	virtual UPrimitiveComponent* FindPrimitiveByTag(const FName Tag) const = 0;

	virtual TArray<FName> GetCollisionOverrides() const = 0;
	virtual void AddCollisionOverride(const FName Tag) = 0;
	virtual void AddCollisionOverrides(const TArray<FName> Tags) = 0;
	virtual void RemoveCollisionOverride(const FName Tag) = 0;
	virtual void RemoveCollisionOverrides(const TArray<FName> Tags) = 0;

	virtual TArray<FName> GetHighlightableOverrides() const = 0;
	virtual void AddHighlightableOverride(const FName Tag) = 0;
	virtual void AddHighlightableOverrides(const TArray<FName> Tags) = 0;
	virtual void RemoveHighlightableOverride(const FName Tag) = 0;
	virtual void RemoveHighlightableOverrides(const TArray<FName> Tags) = 0;

	virtual void ToggleDebug() = 0;

	virtual void FindAndAddCollisionShapes() = 0;
	virtual void FindAndAddHighlightableMeshes() = 0;
	
	virtual void BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const = 0;
	virtual void UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const = 0;

	virtual void BindHighlightableMesh(UMeshComponent* MeshComponent) const = 0;
	virtual void UnbindHighlightableMesh(UMeshComponent* MeshComponent) const = 0;
};