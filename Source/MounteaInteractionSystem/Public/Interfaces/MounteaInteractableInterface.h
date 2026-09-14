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

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="On Interaction Started"))
	void OnInteractionStartedEvent(const float& StartTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="On Interaction Stopped"))
	void OnInteractionStoppedEvent(const float& StopTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="On Interaction Completed"))
	void OnInteractionCompletedEvent(const float& FinishTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="On Interaction Cycle Completed"))
	void OnInteractionCycleCompletedEvent(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="On Interaction Canceled"))
	void OnInteractionCanceledEvent();

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="On Lifecycle Completed"))
	void OnLifecycleCompletedEvent();

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="On Cooldown Completed"))
	void OnCooldownCompletedEvent();

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="On Interactor Found"))
	void OnInteractorFoundEvent(const TScriptInterface<IMounteaInteractorInterface>& FoundInteractor);

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="On Interactor Lost"))
	void OnInteractorLostEvent(const TScriptInterface<IMounteaInteractorInterface>& LostInteractor);

#pragma endregion

public:

#pragma region SlotAPI

	/** Returns all currently active interactor slots. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable|Slots",
		meta=(DisplayName="Get Active Interactor Slots"))
	TArray<FInteractorSlot> GetActiveInteractorSlots() const;
	virtual TArray<FInteractorSlot> GetActiveInteractorSlots_Implementation() const = 0;

	/** Maximum number of interactors that can simultaneously occupy slots. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable|Slots",
		meta=(DisplayName="Get Max Interactors"))
	int32 GetMaxInteractors() const;
	virtual int32 GetMaxInteractors_Implementation() const = 0;

	/** Tags an interactor must possess (any match) to be accepted into a slot. Empty = accept all. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable|Slots",
		meta=(DisplayName="Get Required Interactor Tags"))
	FGameplayTagContainer GetRequiredInteractorTags() const;
	virtual FGameplayTagContainer GetRequiredInteractorTags_Implementation() const = 0;

	/** Tags that disqualify an interactor (any match) from being accepted into a slot - checked
	 *  alongside GetRequiredInteractorTags. Empty = exclude none. Lets an interaction stay fully
	 *  set up (colliders, widgets, etc.) while still being turned off for specific groups, e.g. an
	 *  NPC-tagged interactor excluded from a player-only console, or a Team.B-tagged interactor
	 *  excluded from Team A's vehicle. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable|Slots",
		meta=(DisplayName="Get Excluded Interactor Tags"))
	FGameplayTagContainer GetExcludedInteractorTags() const;
	virtual FGameplayTagContainer GetExcludedInteractorTags_Implementation() const = 0;

	/** True if at least one slot is available for a new interactor. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable|Slots",
		meta=(DisplayName="Is Slot Available"))
	bool IsSlotAvailable() const;
	virtual bool IsSlotAvailable_Implementation() const = 0;

	/** True if the slot count has at least one occupied slot. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Has Interactor"))
	bool HasInteractor() const;
	virtual bool HasInteractor_Implementation() const = 0;

	/** Server RPC: request an interactor join an available slot. Tags are validated server-side. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable|Slots",
		meta=(DisplayName="Request Interactor Join"))
	void RequestInteractorJoin(const TScriptInterface<IMounteaInteractorInterface>& RequestingInteractor);
	virtual void RequestInteractorJoin_Implementation(const TScriptInterface<IMounteaInteractorInterface>& RequestingInteractor) = 0;

	/** Server RPC: request an interactor leave its slot. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable|Slots",
		meta=(DisplayName="Request Interactor Leave"))
	void RequestInteractorLeave(const TScriptInterface<IMounteaInteractorInterface>& LeavingInteractor);
	virtual void RequestInteractorLeave_Implementation(const TScriptInterface<IMounteaInteractorInterface>& LeavingInteractor) = 0;

#pragma endregion

#pragma region WidgetRanges

	/** Attention and Interaction range thresholds for this interactable. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable|Ranges",
		meta=(DisplayName="Get Widget Ranges"))
	FInteractableWidgetRanges GetWidgetRanges() const;
	virtual FInteractableWidgetRanges GetWidgetRanges_Implementation() const = 0;

	/** Sets new widget range thresholds. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable|Ranges",
		meta=(DisplayName="Set Widget Ranges"))
	void SetWidgetRanges(const FInteractableWidgetRanges& NewRanges);
	virtual void SetWidgetRanges_Implementation(const FInteractableWidgetRanges& NewRanges) = 0;

#pragma endregion

#pragma region ActionCycling

	/** If true, this component forces immediate selection (no cycling UI) - v4 single-action behavior. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable|Cycling",
		meta=(DisplayName="Is Standalone Interaction"))
	bool IsStandaloneInteraction() const;
	virtual bool IsStandaloneInteraction_Implementation() const = 0;

	/** Sets standalone interaction mode. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable|Cycling",
		meta=(DisplayName="Set Standalone Interaction"))
	void SetStandaloneInteraction(const bool bStandalone);
	virtual void SetStandaloneInteraction_Implementation(const bool bStandalone) = 0;

#pragma endregion

#pragma region StateMachine

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Activate Interactable"))
	bool ActivateInteractable(FString& ErrorMessage);
	virtual bool ActivateInteractable_Implementation(FString& ErrorMessage) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Wake Up Interactable"))
	bool WakeUpInteractable(FString& ErrorMessage);
	virtual bool WakeUpInteractable_Implementation(FString& ErrorMessage) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Complete Interactable"))
	bool CompleteInteractable(FString& ErrorMessage);
	virtual bool CompleteInteractable_Implementation(FString& ErrorMessage) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Deactivate Interactable"))
	void DeactivateInteractable();
	virtual void DeactivateInteractable_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Pause Interaction"))
	void PauseInteraction(const float ExpirationTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);
	virtual void PauseInteraction_Implementation(const float ExpirationTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Can Interact"))
	bool CanInteract() const;
	virtual bool CanInteract_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Can Be Triggered"))
	bool CanBeTriggered() const;
	virtual bool CanBeTriggered_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Is Interacting"))
	bool IsInteracting() const;
	virtual bool IsInteracting_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Default State"))
	EInteractableStateV3 GetDefaultState() const;
	virtual EInteractableStateV3 GetDefaultState_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, CallInEditor, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Default State"))
	void SetDefaultState(const EInteractableStateV3 NewState);
	virtual void SetDefaultState_Implementation(const EInteractableStateV3 NewState) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get State"))
	EInteractableStateV3 GetState() const;
	virtual EInteractableStateV3 GetState_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set State"))
	void SetState(const EInteractableStateV3 NewState);
	virtual void SetState_Implementation(const EInteractableStateV3 NewState) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Trigger Cooldown"))
	bool TriggerCooldown();
	virtual bool TriggerCooldown_Implementation() = 0;

#pragma endregion

#pragma region LifecycleEvents

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Interactor Found"))
	void InteractorFound(const TScriptInterface<IMounteaInteractorInterface>& FoundInteractor);
	virtual void InteractorFound_Implementation(const TScriptInterface<IMounteaInteractorInterface>& FoundInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Interactor Lost"))
	void InteractorLost(const TScriptInterface<IMounteaInteractorInterface>& LostInteractor);
	virtual void InteractorLost_Implementation(const TScriptInterface<IMounteaInteractorInterface>& LostInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Interaction Started"))
	void InteractionStarted(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);
	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Interaction Stopped"))
	void InteractionStopped(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);
	virtual void InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Interaction Completed"))
	void InteractionCompleted(const float& TimeCompleted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);
	virtual void InteractionCompleted_Implementation(const float& TimeCompleted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Interaction Cycle Completed"))
	void InteractionCycleCompleted(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);
	virtual void InteractionCycleCompleted_Implementation(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Interaction Canceled"))
	void InteractionCanceled();
	virtual void InteractionCanceled_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Interaction Lifecycle Completed"))
	void InteractionLifecycleCompleted();
	virtual void InteractionLifecycleCompleted_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Interaction Cooldown Completed"))
	void InteractionCooldownCompleted();
	virtual void InteractionCooldownCompleted_Implementation() = 0;

#pragma endregion

#pragma region Properties

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Interaction Progress"))
	float GetInteractionProgress() const;
	virtual float GetInteractionProgress_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Interaction Period"))
	float GetInteractionPeriod() const;
	virtual float GetInteractionPeriod_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Interaction Period"))
	void SetInteractionPeriod(const float NewPeriod);
	virtual void SetInteractionPeriod_Implementation(const float NewPeriod) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Interactable Weight"))
	int32 GetInteractableWeight() const;
	virtual int32 GetInteractableWeight_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Interactable Weight"))
	void SetInteractableWeight(const int32 NewWeight);
	virtual void SetInteractableWeight_Implementation(const int32 NewWeight) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Interactable Owner"))
	AActor* GetInteractableOwner() const;
	virtual AActor* GetInteractableOwner_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Owning Actor"))
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Collision Channel"))
	ECollisionChannel GetCollisionChannel() const;
	virtual ECollisionChannel GetCollisionChannel_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Collision Channel"))
	void SetCollisionChannel(const TEnumAsByte<ECollisionChannel>& NewChannel);
	virtual void SetCollisionChannel_Implementation(const TEnumAsByte<ECollisionChannel>& NewChannel) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Lifecycle Mode"))
	EInteractableLifecycle GetLifecycleMode() const;
	virtual EInteractableLifecycle GetLifecycleMode_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Lifecycle Mode"))
	void SetLifecycleMode(const EInteractableLifecycle& NewMode);
	virtual void SetLifecycleMode_Implementation(const EInteractableLifecycle& NewMode) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Lifecycle Count"))
	int32 GetLifecycleCount() const;
	virtual int32 GetLifecycleCount_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Lifecycle Count"))
	void SetLifecycleCount(const int32 NewLifecycleCount);
	virtual void SetLifecycleCount_Implementation(const int32 NewLifecycleCount) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Remaining Lifecycle Count"))
	int32 GetRemainingLifecycleCount() const;
	virtual int32 GetRemainingLifecycleCount_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Cooldown Period"))
	float GetCooldownPeriod() const;
	virtual float GetCooldownPeriod_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Cooldown Period"))
	void SetCooldownPeriod(const float NewCooldownPeriod);
	virtual void SetCooldownPeriod_Implementation(const float NewCooldownPeriod) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Comparison Method"))
	ETimingComparison GetComparisonMethod() const;
	virtual ETimingComparison GetComparisonMethod_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Comparison Method"))
	void SetComparisonMethod(const ETimingComparison Value);
	virtual void SetComparisonMethod_Implementation(const ETimingComparison Value) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Interactable Name"))
	FText GetInteractableName() const;
	virtual FText GetInteractableName_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Interactable Name"))
	void SetInteractableName(const FText& NewName);
	virtual void SetInteractableName_Implementation(const FText& NewName) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Interactable Data"))
	FDataTableRowHandle GetInteractableData() const;
	virtual FDataTableRowHandle GetInteractableData_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Interactable Data"))
	void SetInteractableData(FDataTableRowHandle NewData);
	virtual void SetInteractableData_Implementation(FDataTableRowHandle NewData) = 0;

#pragma endregion

#pragma region Setup

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Does Auto Setup"))
	bool DoesAutoSetup() const;
	virtual bool DoesAutoSetup_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Toggle Auto Setup"))
	void ToggleAutoSetup(const ESetupType& NewValue);
	virtual void ToggleAutoSetup_Implementation(const ESetupType& NewValue) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Find And Add Collision Shapes"))
	void FindAndAddCollisionShapes();
	virtual void FindAndAddCollisionShapes_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Find And Add Highlightable Meshes"))
	void FindAndAddHighlightableMeshes();
	virtual void FindAndAddHighlightableMeshes_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable", meta=(DisplayName="SetDefaults"))
	void SetDefaults();
	virtual void SetDefaults_Implementation() = 0;

#pragma endregion

#pragma region Highlight

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Start Highlight"))
	void StartHighlight();
	virtual void StartHighlight_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Stop Highlight"))
	void StopHighlight();
	virtual void StopHighlight_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Highlight Type"))
	EHighlightType GetHighlightType() const;
	virtual EHighlightType GetHighlightType_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Highlight Type"))
	void SetHighlightType(const EHighlightType NewHighlightType);
	virtual void SetHighlightType_Implementation(const EHighlightType NewHighlightType) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Highlight Material"))
	UMaterialInterface* GetHighlightMaterial() const;
	virtual UMaterialInterface* GetHighlightMaterial_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Highlight Material"))
	void SetHighlightMaterial(UMaterialInterface* NewHighlightMaterial);
	virtual void SetHighlightMaterial_Implementation(UMaterialInterface* NewHighlightMaterial) = 0;

#pragma endregion

#pragma region CollisionAndMesh

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Collision Components"))
	TArray<UPrimitiveComponent*> GetCollisionComponents() const;
	virtual TArray<UPrimitiveComponent*> GetCollisionComponents_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Add Collision Component"))
	void AddCollisionComponent(UPrimitiveComponent* CollisionComp);
	virtual void AddCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Add Collision Components"))
	void AddCollisionComponents(const TArray<UPrimitiveComponent*>& CollisionComponents);
	virtual void AddCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Remove Collision Component"))
	void RemoveCollisionComponent(UPrimitiveComponent* CollisionComp);
	virtual void RemoveCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Remove Collision Components"))
	void RemoveCollisionComponents(const TArray<UPrimitiveComponent*>& CollisionComponents);
	virtual void RemoveCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Highlightable Components"))
	TArray<UMeshComponent*> GetHighlightableComponents() const;
	virtual TArray<UMeshComponent*> GetHighlightableComponents_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Add Highlightable Component"))
	void AddHighlightableComponent(UMeshComponent* HighlightableComp);
	virtual void AddHighlightableComponent_Implementation(UMeshComponent* HighlightableComp) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Add Highlightable Components"))
	void AddHighlightableComponents(const TArray<UMeshComponent*>& HighlightableComponents);
	virtual void AddHighlightableComponents_Implementation(const TArray<UMeshComponent*>& HighlightableComponents) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Remove Highlightable Component"))
	void RemoveHighlightableComponent(UMeshComponent* HighlightableComp);
	virtual void RemoveHighlightableComponent_Implementation(UMeshComponent* HighlightableComp) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Remove Highlightable Components"))
	void RemoveHighlightableComponents(const TArray<UMeshComponent*>& HighlightableComponents);
	virtual void RemoveHighlightableComponents_Implementation(const TArray<UMeshComponent*>& HighlightableComponents) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Collision Overrides"))
	TArray<FName> GetCollisionOverrides() const;
	virtual TArray<FName> GetCollisionOverrides_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Highlightable Overrides"))
	TArray<FName> GetHighlightableOverrides() const;
	virtual TArray<FName> GetHighlightableOverrides_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Bind Collision Shape"))
	void BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const;
	virtual void BindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Unbind Collision Shape"))
	void UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const;
	virtual void UnbindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Bind Highlightable Mesh"))
	void BindHighlightableMesh(UMeshComponent* MeshComponent) const;
	virtual void BindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Unbind Highlightable Mesh"))
	void UnbindHighlightableMesh(UMeshComponent* MeshComponent) const;
	virtual void UnbindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const = 0;

#pragma endregion

#pragma region Tags

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Interactable Compatible Tags"))
	FGameplayTagContainer GetInteractableCompatibleTags() const;
	virtual FGameplayTagContainer GetInteractableCompatibleTags_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Interactable Compatible Tags"))
	void SetInteractableCompatibleTags(const FGameplayTagContainer& Tags);
	virtual void SetInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Add Interactable Compatible Tag"))
	void AddInteractableCompatibleTag(const FGameplayTag& Tag);
	virtual void AddInteractableCompatibleTag_Implementation(const FGameplayTag& Tag) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Add Interactable Compatible Tags"))
	void AddInteractableCompatibleTags(const FGameplayTagContainer& Tags);
	virtual void AddInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Remove Interactable Compatible Tag"))
	void RemoveInteractableCompatibleTag(const FGameplayTag& Tag);
	virtual void RemoveInteractableCompatibleTag_Implementation(const FGameplayTag& Tag) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Remove Interactable Compatible Tags"))
	void RemoveInteractableCompatibleTags(const FGameplayTagContainer& Tags);
	virtual void RemoveInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Clear Interactable Compatible Tags"))
	void ClearInteractableCompatibleTags();
	virtual void ClearInteractableCompatibleTags_Implementation() = 0;

#pragma endregion

#pragma region IgnoredClasses

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Ignored Classes"))
	TArray<TSoftClassPtr<UObject>> GetIgnoredClasses() const;
	virtual TArray<TSoftClassPtr<UObject>> GetIgnoredClasses_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Set Ignored Classes"))
	void SetIgnoredClasses(const TArray<TSoftClassPtr<UObject>>& NewIgnoredClasses);
	virtual void SetIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& NewIgnoredClasses) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Add Ignored Class"))
	void AddIgnoredClass(const TSoftClassPtr<UObject>& AddIgnoredClass);
	virtual void AddIgnoredClass_Implementation(const TSoftClassPtr<UObject>& AddIgnoredClass) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Add Ignored Classes"))
	void AddIgnoredClasses(const TArray<TSoftClassPtr<UObject>>& AddIgnoredClasses);
	virtual void AddIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& AddIgnoredClasses) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Remove Ignored Class"))
	void RemoveIgnoredClass(const TSoftClassPtr<UObject>& RemoveIgnoredClass);
	virtual void RemoveIgnoredClass_Implementation(const TSoftClassPtr<UObject>& RemoveIgnoredClass) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Remove Ignored Classes"))
	void RemoveIgnoredClasses(const TArray<TSoftClassPtr<UObject>>& RemoveIgnoredClasses);
	virtual void RemoveIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& RemoveIgnoredClasses) = 0;

#pragma endregion

#pragma region Dependencies

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Add Interaction Dependency"))
	void AddInteractionDependency(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency);
	virtual void AddInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Remove Interaction Dependency"))
	void RemoveInteractionDependency(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency);
	virtual void RemoveInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Interaction Dependencies"))
	TArray<TScriptInterface<IMounteaInteractableInterface>> GetInteractionDependencies() const;
	virtual TArray<TScriptInterface<IMounteaInteractableInterface>> GetInteractionDependencies_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Process Dependencies"))
	void ProcessDependencies();
	virtual void ProcessDependencies_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Interactable Dependency Started Callback"))
	void InteractableDependencyStartedCallback(const TScriptInterface<IMounteaInteractableInterface>& NewMaster);
	virtual void InteractableDependencyStartedCallback_Implementation(const TScriptInterface<IMounteaInteractableInterface>& NewMaster) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Interactable Dependency Stopped Callback"))
	void InteractableDependencyStoppedCallback(const TScriptInterface<IMounteaInteractableInterface>& FormerMaster);
	virtual void InteractableDependencyStoppedCallback_Implementation(const TScriptInterface<IMounteaInteractableInterface>& FormerMaster) = 0;

#pragma endregion

#pragma region Debug

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Toggle Widget Visibility"))
	void ToggleWidgetVisibility(const bool IsVisible);
	virtual void ToggleWidgetVisibility_Implementation(const bool IsVisible) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Toggle Debug"))
	void ToggleDebug();
	virtual void ToggleDebug_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="Get Debug Settings"))
	FDebugSettings GetDebugSettings() const;
	virtual FDebugSettings GetDebugSettings_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="To String"))
	FString ToString() const;
	virtual FString ToString_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable",
		meta=(DisplayName="On Input Device Changed"))
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
