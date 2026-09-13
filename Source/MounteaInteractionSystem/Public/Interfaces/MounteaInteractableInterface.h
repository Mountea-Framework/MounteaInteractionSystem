// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "Engine/EngineTypes.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "MounteaInteractableInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IMounteaInteractableInterface;
class IMounteaInteractorInterface;
class UMeshComponent;
struct FDataTableRowHandle;
enum class ECommonInputType : uint8;

// --- Lifecycle delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionCompleted,   const float&, FinishTime,    const TScriptInterface<IMounteaInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractionCycleCompleted, const float&, FinishTime, const int32, RemainingLifecycles, const TScriptInterface<IMounteaInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionStarted,     const float&, StartTime,     const TScriptInterface<IMounteaInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionStopped,     const float&, StopTime,      const TScriptInterface<IMounteaInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionCanceled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLifecycleCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCooldownCompleted);

// --- Detection delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorFound, const TScriptInterface<IMounteaInteractorInterface>&, FoundInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorLost,  const TScriptInterface<IMounteaInteractorInterface>&, LostInteractor);

// --- Slot delegate (N-interactor) ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractorSlotsChanged, const TArray<FInteractorSlot>&, NewSlots);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractorRejected,     const TScriptInterface<IMounteaInteractorInterface>&, RejectedInteractor);

// --- State / property change delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableStateChanged,           const EInteractableStateV3&,    NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableWeightChanged,           const int32&,                   NewWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableOwnerChanged,            const AActor*,                  NewOwner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableCollisionChannelChanged, const ECollisionChannel,        NewChannel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLifecycleModeChanged,                const EInteractableLifecycle&,  NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLifecycleCountChanged,               const int32,                    NewCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownPeriodChanged,               const float,                    NewPeriod);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableAutoSetupChanged,        const bool,                     NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableWidgetVisibilityChanged, const bool,                     bIsVisible);

// --- Highlight delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightTypeChanged,     const EHighlightType&,      NewHighlightType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightMaterialChanged, const UMaterialInterface*,  NewMaterial);

// --- Component delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightableComponentAdded,   const UMeshComponent*,        NewComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionComponentAdded,        const UPrimitiveComponent*,   NewComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightableComponentRemoved,  const UMeshComponent*,        RemovedComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionComponentRemoved,      const UPrimitiveComponent*,   RemovedComp);

// --- Dependency delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableDependencyChanged, const TScriptInterface<IMounteaInteractableInterface>&, ChangedDependency);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableDependencyStarted, const TScriptInterface<IMounteaInteractableInterface>&, NewMaster);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableDependencyStopped, const TScriptInterface<IMounteaInteractableInterface>&, FormerMaster);

// --- Input delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionDeviceChanged, const ECommonInputType, DeviceType, const FName&, DeviceName);

/**
 * v5.0 Interactable Interface.
 * Supports N concurrent interactors via slot system, action cycling, and distance-based widget modes.
 */
class MOUNTEAINTERACTIONSYSTEM_API IMounteaInteractableInterface
{
	GENERATED_BODY()

protected:

#pragma region BlueprintImplementableEvents

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea Interaction|Interactable")
	void OnInteractionStartedEvent(const float& StartTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea Interaction|Interactable")
	void OnInteractionStoppedEvent(const float& StopTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea Interaction|Interactable")
	void OnInteractionCompletedEvent(const float& FinishTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea Interaction|Interactable")
	void OnInteractionCycleCompletedEvent(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea Interaction|Interactable")
	void OnInteractionCanceledEvent();

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea Interaction|Interactable")
	void OnLifecycleCompletedEvent();

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea Interaction|Interactable")
	void OnCooldownCompletedEvent();

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea Interaction|Interactable")
	void OnInteractorFoundEvent(const TScriptInterface<IMounteaInteractorInterface>& FoundInteractor);

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea Interaction|Interactable")
	void OnInteractorLostEvent(const TScriptInterface<IMounteaInteractorInterface>& LostInteractor);

#pragma endregion

public:

#pragma region SlotAPI

	/** Returns all currently active interactor slots. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable|Slots")
	TArray<FInteractorSlot> GetActiveInteractorSlots() const;
	virtual TArray<FInteractorSlot> GetActiveInteractorSlots_Implementation() const = 0;

	/** Maximum number of interactors that can simultaneously occupy slots. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable|Slots")
	int32 GetMaxInteractors() const;
	virtual int32 GetMaxInteractors_Implementation() const = 0;

	/** Tags an interactor must possess (any match) to be accepted into a slot. Empty = accept all. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable|Slots")
	FGameplayTagContainer GetRequiredInteractorTags() const;
	virtual FGameplayTagContainer GetRequiredInteractorTags_Implementation() const = 0;

	/** Tags that disqualify an interactor (any match) from being accepted into a slot - checked
	 *  alongside GetRequiredInteractorTags. Empty = exclude none. Lets an interaction stay fully
	 *  set up (colliders, widgets, etc.) while still being turned off for specific groups, e.g. an
	 *  NPC-tagged interactor excluded from a player-only console, or a Team.B-tagged interactor
	 *  excluded from Team A's vehicle. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable|Slots")
	FGameplayTagContainer GetExcludedInteractorTags() const;
	virtual FGameplayTagContainer GetExcludedInteractorTags_Implementation() const = 0;

	/** True if at least one slot is available for a new interactor. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable|Slots")
	bool IsSlotAvailable() const;
	virtual bool IsSlotAvailable_Implementation() const = 0;

	/** True if the slot count has at least one occupied slot. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	bool HasInteractor() const;
	virtual bool HasInteractor_Implementation() const = 0;

	/** Server RPC: request an interactor join an available slot. Tags are validated server-side. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable|Slots")
	void RequestInteractorJoin(const TScriptInterface<IMounteaInteractorInterface>& RequestingInteractor);
	virtual void RequestInteractorJoin_Implementation(const TScriptInterface<IMounteaInteractorInterface>& RequestingInteractor) = 0;

	/** Server RPC: request an interactor leave its slot. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable|Slots")
	void RequestInteractorLeave(const TScriptInterface<IMounteaInteractorInterface>& LeavingInteractor);
	virtual void RequestInteractorLeave_Implementation(const TScriptInterface<IMounteaInteractorInterface>& LeavingInteractor) = 0;

#pragma endregion

#pragma region WidgetRanges

	/** Attention and Interaction range thresholds for this interactable. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable|Ranges")
	FInteractableWidgetRanges GetWidgetRanges() const;
	virtual FInteractableWidgetRanges GetWidgetRanges_Implementation() const = 0;

	/** Sets new widget range thresholds. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable|Ranges")
	void SetWidgetRanges(const FInteractableWidgetRanges& NewRanges);
	virtual void SetWidgetRanges_Implementation(const FInteractableWidgetRanges& NewRanges) = 0;

#pragma endregion

#pragma region ActionCycling

	/** If true, this component forces immediate selection (no cycling UI) - v4 single-action behavior. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable|Cycling")
	bool IsStandaloneInteraction() const;
	virtual bool IsStandaloneInteraction_Implementation() const = 0;

	/** Sets standalone interaction mode. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable|Cycling")
	void SetStandaloneInteraction(const bool bStandalone);
	virtual void SetStandaloneInteraction_Implementation(const bool bStandalone) = 0;

#pragma endregion

#pragma region StateMachine

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	bool ActivateInteractable(FString& ErrorMessage);
	virtual bool ActivateInteractable_Implementation(FString& ErrorMessage) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	bool WakeUpInteractable(FString& ErrorMessage);
	virtual bool WakeUpInteractable_Implementation(FString& ErrorMessage) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	bool CompleteInteractable(FString& ErrorMessage);
	virtual bool CompleteInteractable_Implementation(FString& ErrorMessage) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void DeactivateInteractable();
	virtual void DeactivateInteractable_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void PauseInteraction(const float ExpirationTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);
	virtual void PauseInteraction_Implementation(const float ExpirationTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	bool CanInteract() const;
	virtual bool CanInteract_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	bool CanBeTriggered() const;
	virtual bool CanBeTriggered_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	bool IsInteracting() const;
	virtual bool IsInteracting_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	EInteractableStateV3 GetDefaultState() const;
	virtual EInteractableStateV3 GetDefaultState_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, CallInEditor, Category="Mountea Interaction|Interactable")
	void SetDefaultState(const EInteractableStateV3 NewState);
	virtual void SetDefaultState_Implementation(const EInteractableStateV3 NewState) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	EInteractableStateV3 GetState() const;
	virtual EInteractableStateV3 GetState_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetState(const EInteractableStateV3 NewState);
	virtual void SetState_Implementation(const EInteractableStateV3 NewState) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	bool TriggerCooldown();
	virtual bool TriggerCooldown_Implementation() = 0;

#pragma endregion

#pragma region LifecycleEvents

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void InteractorFound(const TScriptInterface<IMounteaInteractorInterface>& FoundInteractor);
	virtual void InteractorFound_Implementation(const TScriptInterface<IMounteaInteractorInterface>& FoundInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void InteractorLost(const TScriptInterface<IMounteaInteractorInterface>& LostInteractor);
	virtual void InteractorLost_Implementation(const TScriptInterface<IMounteaInteractorInterface>& LostInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void InteractionStarted(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);
	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void InteractionStopped(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);
	virtual void InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void InteractionCompleted(const float& TimeCompleted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);
	virtual void InteractionCompleted_Implementation(const float& TimeCompleted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void InteractionCycleCompleted(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);
	virtual void InteractionCycleCompleted_Implementation(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void InteractionCanceled();
	virtual void InteractionCanceled_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void InteractionLifecycleCompleted();
	virtual void InteractionLifecycleCompleted_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void InteractionCooldownCompleted();
	virtual void InteractionCooldownCompleted_Implementation() = 0;

#pragma endregion

#pragma region Properties

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	float GetInteractionProgress() const;
	virtual float GetInteractionProgress_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	float GetInteractionPeriod() const;
	virtual float GetInteractionPeriod_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetInteractionPeriod(const float NewPeriod);
	virtual void SetInteractionPeriod_Implementation(const float NewPeriod) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	int32 GetInteractableWeight() const;
	virtual int32 GetInteractableWeight_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetInteractableWeight(const int32 NewWeight);
	virtual void SetInteractableWeight_Implementation(const int32 NewWeight) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	AActor* GetInteractableOwner() const;
	virtual AActor* GetInteractableOwner_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	ECollisionChannel GetCollisionChannel() const;
	virtual ECollisionChannel GetCollisionChannel_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetCollisionChannel(const TEnumAsByte<ECollisionChannel>& NewChannel);
	virtual void SetCollisionChannel_Implementation(const TEnumAsByte<ECollisionChannel>& NewChannel) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	EInteractableLifecycle GetLifecycleMode() const;
	virtual EInteractableLifecycle GetLifecycleMode_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetLifecycleMode(const EInteractableLifecycle& NewMode);
	virtual void SetLifecycleMode_Implementation(const EInteractableLifecycle& NewMode) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	int32 GetLifecycleCount() const;
	virtual int32 GetLifecycleCount_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetLifecycleCount(const int32 NewLifecycleCount);
	virtual void SetLifecycleCount_Implementation(const int32 NewLifecycleCount) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	int32 GetRemainingLifecycleCount() const;
	virtual int32 GetRemainingLifecycleCount_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	float GetCooldownPeriod() const;
	virtual float GetCooldownPeriod_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetCooldownPeriod(const float NewCooldownPeriod);
	virtual void SetCooldownPeriod_Implementation(const float NewCooldownPeriod) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	ETimingComparison GetComparisonMethod() const;
	virtual ETimingComparison GetComparisonMethod_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetComparisonMethod(const ETimingComparison Value);
	virtual void SetComparisonMethod_Implementation(const ETimingComparison Value) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	FText GetInteractableName() const;
	virtual FText GetInteractableName_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetInteractableName(const FText& NewName);
	virtual void SetInteractableName_Implementation(const FText& NewName) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	FDataTableRowHandle GetInteractableData() const;
	virtual FDataTableRowHandle GetInteractableData_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetInteractableData(FDataTableRowHandle NewData);
	virtual void SetInteractableData_Implementation(FDataTableRowHandle NewData) = 0;

#pragma endregion

#pragma region Setup

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	bool DoesAutoSetup() const;
	virtual bool DoesAutoSetup_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void ToggleAutoSetup(const ESetupType& NewValue);
	virtual void ToggleAutoSetup_Implementation(const ESetupType& NewValue) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void FindAndAddCollisionShapes();
	virtual void FindAndAddCollisionShapes_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void FindAndAddHighlightableMeshes();
	virtual void FindAndAddHighlightableMeshes_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable", meta=(DisplayName="SetDefaults"))
	void SetDefaults();
	virtual void SetDefaults_Implementation() = 0;

#pragma endregion

#pragma region Highlight

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void StartHighlight();
	virtual void StartHighlight_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void StopHighlight();
	virtual void StopHighlight_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	EHighlightType GetHighlightType() const;
	virtual EHighlightType GetHighlightType_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetHighlightType(const EHighlightType NewHighlightType);
	virtual void SetHighlightType_Implementation(const EHighlightType NewHighlightType) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	UMaterialInterface* GetHighlightMaterial() const;
	virtual UMaterialInterface* GetHighlightMaterial_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetHighlightMaterial(UMaterialInterface* NewHighlightMaterial);
	virtual void SetHighlightMaterial_Implementation(UMaterialInterface* NewHighlightMaterial) = 0;

#pragma endregion

#pragma region CollisionAndMesh

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	TArray<UPrimitiveComponent*> GetCollisionComponents() const;
	virtual TArray<UPrimitiveComponent*> GetCollisionComponents_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void AddCollisionComponent(UPrimitiveComponent* CollisionComp);
	virtual void AddCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void AddCollisionComponents(const TArray<UPrimitiveComponent*>& CollisionComponents);
	virtual void AddCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void RemoveCollisionComponent(UPrimitiveComponent* CollisionComp);
	virtual void RemoveCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void RemoveCollisionComponents(const TArray<UPrimitiveComponent*>& CollisionComponents);
	virtual void RemoveCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	TArray<UMeshComponent*> GetHighlightableComponents() const;
	virtual TArray<UMeshComponent*> GetHighlightableComponents_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void AddHighlightableComponent(UMeshComponent* HighlightableComp);
	virtual void AddHighlightableComponent_Implementation(UMeshComponent* HighlightableComp) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void AddHighlightableComponents(const TArray<UMeshComponent*>& HighlightableComponents);
	virtual void AddHighlightableComponents_Implementation(const TArray<UMeshComponent*>& HighlightableComponents) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void RemoveHighlightableComponent(UMeshComponent* HighlightableComp);
	virtual void RemoveHighlightableComponent_Implementation(UMeshComponent* HighlightableComp) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void RemoveHighlightableComponents(const TArray<UMeshComponent*>& HighlightableComponents);
	virtual void RemoveHighlightableComponents_Implementation(const TArray<UMeshComponent*>& HighlightableComponents) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	TArray<FName> GetCollisionOverrides() const;
	virtual TArray<FName> GetCollisionOverrides_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	TArray<FName> GetHighlightableOverrides() const;
	virtual TArray<FName> GetHighlightableOverrides_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const;
	virtual void BindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const;
	virtual void UnbindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void BindHighlightableMesh(UMeshComponent* MeshComponent) const;
	virtual void BindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void UnbindHighlightableMesh(UMeshComponent* MeshComponent) const;
	virtual void UnbindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const = 0;

#pragma endregion

#pragma region Tags

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	FGameplayTagContainer GetInteractableCompatibleTags() const;
	virtual FGameplayTagContainer GetInteractableCompatibleTags_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetInteractableCompatibleTags(const FGameplayTagContainer& Tags);
	virtual void SetInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void AddInteractableCompatibleTag(const FGameplayTag& Tag);
	virtual void AddInteractableCompatibleTag_Implementation(const FGameplayTag& Tag) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void AddInteractableCompatibleTags(const FGameplayTagContainer& Tags);
	virtual void AddInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void RemoveInteractableCompatibleTag(const FGameplayTag& Tag);
	virtual void RemoveInteractableCompatibleTag_Implementation(const FGameplayTag& Tag) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void RemoveInteractableCompatibleTags(const FGameplayTagContainer& Tags);
	virtual void RemoveInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void ClearInteractableCompatibleTags();
	virtual void ClearInteractableCompatibleTags_Implementation() = 0;

#pragma endregion

#pragma region IgnoredClasses

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	TArray<TSoftClassPtr<UObject>> GetIgnoredClasses() const;
	virtual TArray<TSoftClassPtr<UObject>> GetIgnoredClasses_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void SetIgnoredClasses(const TArray<TSoftClassPtr<UObject>>& NewIgnoredClasses);
	virtual void SetIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& NewIgnoredClasses) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void AddIgnoredClass(const TSoftClassPtr<UObject>& AddIgnoredClass);
	virtual void AddIgnoredClass_Implementation(const TSoftClassPtr<UObject>& AddIgnoredClass) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void AddIgnoredClasses(const TArray<TSoftClassPtr<UObject>>& AddIgnoredClasses);
	virtual void AddIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& AddIgnoredClasses) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void RemoveIgnoredClass(const TSoftClassPtr<UObject>& RemoveIgnoredClass);
	virtual void RemoveIgnoredClass_Implementation(const TSoftClassPtr<UObject>& RemoveIgnoredClass) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void RemoveIgnoredClasses(const TArray<TSoftClassPtr<UObject>>& RemoveIgnoredClasses);
	virtual void RemoveIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& RemoveIgnoredClasses) = 0;

#pragma endregion

#pragma region Dependencies

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void AddInteractionDependency(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency);
	virtual void AddInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void RemoveInteractionDependency(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency);
	virtual void RemoveInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	TArray<TScriptInterface<IMounteaInteractableInterface>> GetInteractionDependencies() const;
	virtual TArray<TScriptInterface<IMounteaInteractableInterface>> GetInteractionDependencies_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void ProcessDependencies();
	virtual void ProcessDependencies_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void InteractableDependencyStartedCallback(const TScriptInterface<IMounteaInteractableInterface>& NewMaster);
	virtual void InteractableDependencyStartedCallback_Implementation(const TScriptInterface<IMounteaInteractableInterface>& NewMaster) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void InteractableDependencyStoppedCallback(const TScriptInterface<IMounteaInteractableInterface>& FormerMaster);
	virtual void InteractableDependencyStoppedCallback_Implementation(const TScriptInterface<IMounteaInteractableInterface>& FormerMaster) = 0;

#pragma endregion

#pragma region Debug

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void ToggleWidgetVisibility(const bool IsVisible);
	virtual void ToggleWidgetVisibility_Implementation(const bool IsVisible) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void ToggleDebug();
	virtual void ToggleDebug_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	FDebugSettings GetDebugSettings() const;
	virtual FDebugSettings GetDebugSettings_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	FString ToString() const;
	virtual FString ToString_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactable")
	void OnInputDeviceChanged(const ECommonInputType DeviceType, const FName& DeviceName);
	virtual void OnInputDeviceChanged_Implementation(const ECommonInputType DeviceType, const FName& DeviceName) = 0;

#pragma endregion

#pragma region DelegateAccessors

	virtual FInteractorFound&               GetOnInteractorFoundHandle() = 0;
	virtual FInteractorLost&                GetOnInteractorLostHandle() = 0;
	virtual FOnInteractorSlotsChanged&      GetOnInteractorSlotsChangedHandle() = 0;
	virtual FOnInteractorRejected&          GetOnInteractorRejectedHandle() = 0;
	virtual FInteractionStarted&            GetOnInteractionStartedHandle() = 0;
	virtual FInteractionStopped&            GetOnInteractionStoppedHandle() = 0;
	virtual FInteractionCompleted&          GetOnInteractionCompletedHandle() = 0;
	virtual FInteractionCycleCompleted&     GetOnInteractionCycleCompletedHandle() = 0;
	virtual FInteractionCanceled&           GetOnInteractionCanceledHandle() = 0;
	virtual FLifecycleCompleted&            GetOnLifecycleCompletedHandle() = 0;
	virtual FCooldownCompleted&             GetOnCooldownCompletedHandle() = 0;
	virtual FInteractableStateChanged&      GetInteractableStateChangedHandle() = 0;
	virtual FInteractableDependencyChanged& GetInteractableDependencyChangedHandle() = 0;
	virtual FInteractableDependencyStarted& GetInteractableDependencyStartedHandle() = 0;
	virtual FInteractableDependencyStopped& GetInteractableDependencyStoppedHandle() = 0;
	virtual FHighlightTypeChanged&          GetHighlightTypeChangedHandle() = 0;
	virtual FHighlightMaterialChanged&      GetHighlightMaterialChangedHandle() = 0;
	virtual FInteractableWidgetVisibilityChanged& GetInteractableWidgetVisibilityChangedHandle() = 0;
	virtual FInteractionDeviceChanged&      GetInteractionDeviceChangedHandle() = 0;

#pragma endregion
};
