// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UActorInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

#pragma region SetupType
/**
 * Enumerator definition of setup modes.
 */
UENUM(BlueprintType)
enum class ESetupType : uint8
{
	EST_FullAll		UMETA(DisplayName="Full Auto Setup",			Tooltip="Will add all components from Owning Actor to Highlightable and Collision Components."),
	EST_AllParent	UMETA(DisplayName="All Parents Auto Setup", 	Tooltip="Will add all parent components to Highlightable and Collision Components."),
	EST_Quick		UMETA(DisplayName="Quick Auto Setup",			Tooltip="Will add only first parent component to Highlightable and Collision Components."),
	EST_None		UMETA(DisplayName="None",						Tooltip="No auto setup will be performed."),

	EST_Default		UMETA(Hidden)
};

#pragma endregion

#pragma region ComparisonMethod
UENUM(BlueprintType)
enum class ETimingComparison : uint8 // TODO: rename, because name is used
{
	ECM_LessThan	UMETA(DisplayName="Less Than"),
	ECM_MoreThan	UMETA(DisplayName="More Than"),
	ECM_None		UMETA(DisplayName="No comparison"),

	Default			 UMETA(Hidden)
   };
#pragma endregion 

#pragma region HighlightType

UENUM(BlueprintType)
enum class EHighlightType : uint8
{
	EHT_PostProcessing		UMETA(DisplayName="PostProcessing",			Tooltip="PostProcessing Material will be used. This option is highly optimised, however, requires Project setup."),
	EHT_OverlayMaterial		UMETA(DisplayName="Overlay Material",		Tooltip="Overlay Material will be used. Unique for 5.1 and newer versions. For very complex meshes might cause performance issues."),

	EHT_Default		UMETA(Hidden)
};

#pragma endregion 

class IActorInteractableInterface;
class IActorInteractorInterface;

struct FDataTableRowHandle;

enum class EInteractableStateV2 : uint8;
enum class EInteractableLifecycle : uint8;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorFound, const TScriptInterface<IActorInteractorInterface>&, FoundInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorLost, const TScriptInterface<IActorInteractorInterface>&, LostInteractor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableSelected, const TScriptInterface<IActorInteractableInterface>&, SelectedInteractable);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FInteractorTraced, UPrimitiveComponent*, HitComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, FVector, NormalImpulse, const FHitResult&, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FInteractorOverlapped, UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex, bool, bFromSweep, const FHitResult &, SweepResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FInteractorStopOverlap, UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionCompleted, const float&, FinishTime, const TScriptInterface<IActorInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractionCycleCompleted, const float&, FinishTime, const int32, RemainingLifecycles, const TScriptInterface<IActorInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractionStarted, const float&, StartTime, const FKey&, PressedKey, const TScriptInterface<IActorInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractionStopped, const float&, StartTime, const FKey&, PressedKey, const TScriptInterface<IActorInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionCanceled);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLifecycleCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCooldownCompleted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableDependencyChanged, const TScriptInterface<IActorInteractableInterface>&, ChagnedDependency);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableAutoSetupChanged, const bool, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableWeightChanged, const int32&, NewWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableStateChanged, const EInteractableStateV2&, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableOwnerChanged, const AActor*, NewOwner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableCollisionChannelChanged, const ECollisionChannel, NewCollisionChannel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLifecycleModeChanged, const EInteractableLifecycle&, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLifecycleCountChanged, const int32, NewLifecycleCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownPeriodChanged, const float, NewCooldownPeriod);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorChanged, const TScriptInterface<IActorInteractorInterface>&, NewInteractor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIgnoredInteractorClassAdded, const TSoftClassPtr<UObject>&, IgnoredClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIgnoredInteractorClassRemoved, const TSoftClassPtr<UObject>&, IgnoredClass);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightableComponentAdded, const UMeshComponent*, NewHighlightableComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionComponentAdded, const UPrimitiveComponent*, NewCollisionComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightableComponentRemoved, const UMeshComponent*, RemovedHighlightableComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionComponentRemoved, const UPrimitiveComponent*, RemovedCollisionComp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightTypeChanged, const EHighlightType&, NewHighlightType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightMaterialChanged, const UMaterialInterface*, NewHighlightMaterial);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableDependencyStarted, const TScriptInterface<IActorInteractableInterface>&, NewMaster);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableDependencyStopped, const TScriptInterface<IActorInteractableInterface>&, FormerMaster);

/**
 * 
 */
class ACTORINTERACTIONPLUGIN_API IActorInteractableInterface
{
	GENERATED_BODY()

#pragma region InteractableEvents

public:
	
	/**
	 * Returns whether this Interactable can interacted with or not.
	 * Calls Internal CanInteract which is implemented in C++.
	 * Be sure to call Parent Event!
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Interaction", meta=(DisplayName = "Can Interact"))
	bool CanInteractEvent() const;

protected:
	
	bool CanInteractEvent_Implementation() const
	{
		return CanInteract();
	}

#pragma endregion 

#pragma region InteractionEvents

protected:
		
	/**
	 * Event bound to OnInteractableSelected.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableSelectedEvent(const TScriptInterface<IActorInteractableInterface>& Interactable);
	
	/**
	 * Event bound to OnInteractorFound.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractorFoundEvent(const TScriptInterface<IActorInteractorInterface>& FoundInteractor);

	/**
	 * Event bound to OnInteractorLost.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractorLostEvent(const TScriptInterface<IActorInteractorInterface>& LostInteractor);
	
	/**
	 * Event bound to OnInteractorOverlapped.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/**
	 * Event bound to OnInteractorStopOverlap.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableStopOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**
	 * Event bound to OnInteractorTraced.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableTracedEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/**
	 * Event bound to OnInteractionCompleted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractionCompletedEvent(const float& FinishTime, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);

	/**
	 * Event called once Interaction Cycle is completed.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractionCycleCompletedEvent(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);

	/**
	 * Event bound to OnInteractionStarted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractionStartedEvent(const float& StartTime, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);

	/**
	 * Event bound to OnInteractionStopped.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractionStoppedEvent(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);

	/**
	 * Event bound to OnInteractionCanceled.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractionCanceledEvent();
	
	/**
	 * Event bound to OnLifecycleCompleted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnLifecycleCompletedEvent();

	/**
	 * Event bound to OnCooldownCompleted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnCooldownCompletedEvent();

#pragma endregion
	
public:

	virtual bool DoesHaveInteractor() const = 0;
	virtual bool DoesAutoSetup() const = 0;
	virtual void ToggleAutoSetup(const ESetupType& NewValue) = 0;

	virtual bool ActivateInteractable(FString& ErrorMessage) = 0;
	virtual bool WakeUpInteractable(FString& ErrorMessage) = 0;
	virtual bool SnoozeInteractable(FString& ErrorMessage) = 0;
	virtual bool CompleteInteractable(FString& ErrorMessage) = 0;
	virtual void DeactivateInteractable() = 0;
	virtual void PauseInteraction(const float ExpirationTime, const FKey UsedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) = 0;

	virtual void InteractableSelected(const TScriptInterface<IActorInteractableInterface>& Interactable) = 0;
	virtual void InteractableLost(const TScriptInterface<IActorInteractableInterface>& LostInteractable) = 0;
	virtual void InteractorFound(const TScriptInterface<IActorInteractorInterface>& FoundInteractor) = 0;
	virtual void InteractorLost(const TScriptInterface<IActorInteractorInterface>& LostInteractor) = 0;

	virtual void InteractionCompleted(const float& TimeCompleted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) = 0;
	virtual void InteractionCycleCompleted(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) = 0;
	virtual void InteractionStarted(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) = 0;
	virtual void InteractionStopped(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) = 0;
	virtual void InteractionCanceled() = 0;
	virtual void InteractionLifecycleCompleted() = 0;
	virtual void InteractionCooldownCompleted() = 0;
	

	virtual bool CanInteract() const = 0;
	virtual bool CanBeTriggered() const = 0;
	virtual bool IsInteracting() const = 0;

	virtual EInteractableStateV2 GetDefaultState() const = 0;
	virtual void SetDefaultState(const EInteractableStateV2 NewState) = 0;
	virtual EInteractableStateV2 GetState() const = 0;
	virtual void SetState(const EInteractableStateV2 NewState) = 0;


	virtual void StartHighlight() = 0;
	virtual void StopHighlight() = 0;
	
	virtual float GetInteractionProgress() const = 0;
	
	virtual float GetInteractionPeriod() const = 0;
	virtual void SetInteractionPeriod(const float NewPeriod) = 0;
	
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

	virtual int32 GetRemainingLifecycleCount() const = 0;

	virtual float GetCooldownPeriod() const = 0;
	virtual void SetCooldownPeriod(const float NewCooldownPeriod) = 0;

	virtual ETimingComparison GetComparisonMethod() const = 0;
	virtual void SetComparisonMethod(const ETimingComparison Value) = 0;

	virtual FKey GetInteractionKeyForPlatform(const FString& RequestedPlatform) const = 0;
	virtual TArray<FKey> GetInteractionKeysForPlatform(const FString& RequestedPlatform) const = 0;
	virtual void SetInteractionKey(const FString& Platform, const FKey NewInteractorKey) = 0;
	virtual TMap<FString, struct FInteractionKeySetup> GetInteractionKeys() const = 0;
	virtual bool FindKey(const FKey& RequestedKey, const FString& Platform) const = 0;

	virtual void AddInteractionDependency(const TScriptInterface<IActorInteractableInterface> InteractionDependency) = 0;
	virtual void RemoveInteractionDependency(const TScriptInterface<IActorInteractableInterface> InteractionDependency) = 0;
	virtual TArray<TScriptInterface<IActorInteractableInterface>> GetInteractionDependencies() const = 0;
	virtual void ProcessDependencies() = 0;


	virtual bool TriggerCooldown() = 0;
	virtual void ToggleWidgetVisibility(const bool IsVisible) = 0;
	
	virtual TArray<TSoftClassPtr<UObject>> GetIgnoredClasses() const = 0;
	virtual void SetIgnoredClasses(const TArray<TSoftClassPtr<UObject>> NewIgnoredClasses) = 0;
	virtual void AddIgnoredClass(TSoftClassPtr<UObject> AddIgnoredClass) = 0;
	virtual void AddIgnoredClasses(TArray<TSoftClassPtr<UObject>> AddIgnoredClasses) = 0;
	virtual void RemoveIgnoredClass(TSoftClassPtr<UObject> AddIgnoredClass) = 0;
	virtual void RemoveIgnoredClasses(TArray<TSoftClassPtr<UObject>> AddIgnoredClasses) = 0;
	

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
	virtual UMeshComponent* FindMeshByName(const FName Name) const = 0;
	virtual UPrimitiveComponent* FindPrimitiveByTag(const FName Tag) const = 0;
	virtual UPrimitiveComponent* FindPrimitiveByName(const FName Name) const = 0;

	virtual TArray<FName> GetCollisionOverrides() const = 0;
	virtual TArray<FName> GetHighlightableOverrides() const = 0;

	virtual FText GetInteractableName() const = 0;
	virtual void SetInteractableName(const FText& NewName) = 0;
	
	virtual void ToggleDebug() = 0;

	virtual void FindAndAddCollisionShapes() = 0;
	virtual void FindAndAddHighlightableMeshes() = 0;
	
	virtual void BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const = 0;
	virtual void UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const = 0;

	virtual void BindHighlightableMesh(UMeshComponent* MeshComponent) const = 0;
	virtual void UnbindHighlightableMesh(UMeshComponent* MeshComponent) const = 0;
	
	virtual FDataTableRowHandle GetInteractableData() = 0;
	virtual void SetInteractableData(FDataTableRowHandle NewData) = 0;

	virtual EHighlightType GetHighlightType() const = 0;
	virtual void SetHighlightType(const EHighlightType NewHighlightType) = 0;
	virtual UMaterialInterface* GetHighlightMaterial() const = 0;
	virtual void SetHighlightMaterial(UMaterialInterface* NewHighlightMaterial) = 0;

	virtual void InteractableDependencyStartedCallback(const TScriptInterface<IActorInteractableInterface>& NewMaster) = 0;
	virtual void InteractableDependencyStoppedCallback(const TScriptInterface<IActorInteractableInterface>& FormerMaster) = 0;

	virtual void SetDefaults() = 0;
	
	virtual FOnInteractableSelected& GetOnInteractableSelectedHandle() = 0;
	virtual FInteractorFound& GetOnInteractorFoundHandle() = 0;
	virtual FInteractorLost& GetOnInteractorLostHandle() = 0;
	virtual FInteractorTraced& GetOnInteractorTracedHandle() = 0;
	virtual FInteractorOverlapped& GetOnInteractorOverlappedHandle() = 0;
	virtual FInteractorStopOverlap& GetOnInteractorStopOverlapHandle() = 0;
	virtual FInteractionCompleted& GetOnInteractionCompletedHandle() = 0;
	virtual FInteractionCycleCompleted& GetOnInteractionCycleCompletedHandle() = 0;
	virtual FInteractionStarted& GetOnInteractionStartedHandle() = 0;
	virtual FInteractionStopped& GetOnInteractionStoppedHandle() = 0;
	virtual FInteractionCanceled& GetOnInteractionCanceledHandle() = 0;
	virtual FInteractableDependencyChanged& GetInteractableDependencyChangedHandle() = 0;

	virtual FInteractableDependencyStarted& GetInteractableDependencyStarted() = 0;
	virtual FInteractableDependencyStopped& GetInteractableDependencyStopped() = 0;

	virtual FHighlightTypeChanged& GetHighlightTypeChanged() = 0;
	virtual FHighlightMaterialChanged& GetHighlightMaterialChanged() = 0;

	virtual FTimerHandle& GetCooldownHandle() = 0;
	virtual FInteractableStateChanged& GetInteractableStateChanged() = 0;
};