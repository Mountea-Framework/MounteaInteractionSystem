// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "MounteaInteractableStatics.generated.h"

class IMounteaInteractableInterface;
class IMounteaInteractorInterface;
class UPrimitiveComponent;
class UMeshComponent;
class UMaterialInterface;
class UMounteaInteractionSystemSettings;
class UDataTable;
class UUserWidget;

/**
 * Static helpers scoped to interactable components.
 * All slot/state/property queries and mutations for IMounteaInteractableInterface.
 */
UCLASS()
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

#pragma region SlotQueries

	/** Returns the number of currently occupied interactor slots. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Slots",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactor Slot Count"))
	static int32 GetInteractorSlotCount(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns the maximum number of simultaneous interactors allowed. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Slots",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Max Interactors"))
	static int32 GetMaxInteractors(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns true if at least one slot is open for a new interactor. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Slots",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Interactable - Is Slot Available"))
	static bool IsSlotAvailable(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns all currently active interactor slots. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Slots",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Active Interactor Slots"))
	static TArray<FInteractorSlot> GetActiveInteractorSlots(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns true if the given interactor is already occupying a slot. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Slots",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Interactable - Is Interactor In Slot"))
	static bool IsInteractorInSlot(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const TScriptInterface<IMounteaInteractorInterface>& Interactor);

#pragma endregion

#pragma region ProgressQuery

	/**
	 * Computes normalized interaction progress [0..1] from the server start time.
	 * Returns 0 if interaction hasn't started (InteractionStartServerTime == -1).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable",
		meta=(MounteaGetter, DefaultToSelf="WorldContext", HidePin="WorldContext"),
		meta=(DisplayName="Interactable - Get Interaction Progress"))
	static float GetInteractionProgress(
		const UObject* WorldContext,
		const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns the interaction period in seconds. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interaction Period"))
	static float GetInteractionPeriod(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns the remaining lifecycle count (-1 = infinite). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Remaining Lifecycle Count"))
	static int32 GetRemainingLifecycleCount(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

#pragma endregion

#pragma region StateQueries

	/** Returns the current interactable state. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|State",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactable State"))
	static EInteractableStateV3 GetInteractableState(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns true if the interactable is in a state that permits interaction. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|State",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Interactable - Can Interactable Be Triggered"))
	static bool CanInteractableBeTriggered(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns true if the interactable currently has at least one active interactor. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|State",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Interactable - Is Interactable Interacting"))
	static bool IsInteractableInteracting(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns true if the interactable opts out of cycling (forces immediate select). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|State",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Is Standalone Interaction"))
	static bool IsStandaloneInteraction(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns the attention and interaction range thresholds. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|State",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Widget Ranges"))
	static FInteractableWidgetRanges GetWidgetRanges(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns the interactable's sort weight (higher = preferred in Tier 1 selection). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|State",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactable Weight"))
	static int32 GetInteractableWeight(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

#pragma endregion

#pragma region StateMutations

	/**
	 * Sets the interactable state. Routes to authority via Server RPC when called on a client.
	 * Use ActivateInteractable / DeactivateInteractable for lifecycle-aware transitions.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|State",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Interactable State"))
	static void SetInteractableState(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const EInteractableStateV3 NewState);

	/** Attempts to transition the interactable to Active state. Populates ErrorMessage on failure. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaCommand, ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Interactable - Activate Interactable"))
	static bool ActivateInteractable(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		FString& ErrorMessage);

	/** Transitions the interactable to Disabled state and removes all active interactors. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Deactivate Interactable"))
	static void DeactivateInteractable(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Attempts to transition the interactable to Awake (ready to detect). */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaCommand, ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Interactable - Wake Up Interactable"))
	static bool WakeUpInteractable(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		FString& ErrorMessage);

	/** Sets the interaction period in seconds. Authority-only; routes via Server RPC on clients. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Interaction Period"))
	static void SetInteractionPeriod(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const float NewPeriod);

	/** Sets the widget range thresholds (attention and interaction distances). */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Widget Ranges"))
	static void SetWidgetRanges(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const FInteractableWidgetRanges& NewRanges);

	/** Sets standalone interaction mode (true = no cycling, immediate select). */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Standalone Interaction"))
	static void SetStandaloneInteraction(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const bool bStandalone);

#pragma endregion

#pragma region SlotMutations

	/** Requests the given interactor join an available slot. Tag validation is performed server-side. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Slots",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Request Interactor Join"))
	static void RequestInteractorJoin(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Requests the given interactor leave its slot. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Slots",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Request Interactor Leave"))
	static void RequestInteractorLeave(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns the tags an interactor must match (any) to occupy a slot. Empty = no filter. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Slots",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Required Interactor Tags"))
	static FGameplayTagContainer GetRequiredInteractorTags(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

#pragma endregion

#pragma region DefaultState

	/** Returns the boot state the interactable starts in. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|State",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Default Interactable State"))
	static EInteractableStateV3 GetDefaultInteractableState(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Sets the boot state (applied on BeginPlay / SetDefaults). */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|State",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Default Interactable State"))
	static void SetDefaultInteractableState(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const EInteractableStateV3 NewState);

#pragma endregion

#pragma region Cooldown

	/** Starts the interactable's cooldown timer. Returns false when not authority. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaCommand, ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Interactable - Trigger Cooldown"))
	static bool TriggerCooldown(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Pauses the ongoing interaction until expiration or external wake. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Pause Interaction"))
	static void PauseInteraction(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const float ExpirationTime,
		const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor);

#pragma endregion

#pragma region Properties

	/** Sets the interactable's sort weight. Higher = preferred in Tier 1 selection. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Interactable Weight"))
	static void SetInteractableWeight(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const int32 NewWeight);

	/** Returns the interactable's cooldown period in seconds. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Cooldown Period"))
	static float GetCooldownPeriod(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Sets the cooldown period in seconds. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Cooldown Period"))
	static void SetCooldownPeriod(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const float NewCooldownPeriod);

	/** Returns the lifecycle mode (Cycled or Once). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Lifecycle Mode"))
	static EInteractableLifecycle GetLifecycleMode(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Sets the lifecycle mode. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Lifecycle Mode"))
	static void SetLifecycleMode(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const EInteractableLifecycle NewMode);

	/** Returns the total lifecycle count (-1 = infinite). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Lifecycle Count"))
	static int32 GetLifecycleCount(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Sets the total lifecycle count (-1 = infinite). */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Lifecycle Count"))
	static void SetLifecycleCount(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const int32 NewLifecycleCount);

	/** Returns the interactable's display name. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactable Name"))
	static FText GetInteractableName(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Sets the interactable's display name. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Interactable Name"))
	static void SetInteractableName(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const FText& NewName);

	/** Returns the interactable's data table row handle. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactable Data"))
	static FDataTableRowHandle GetInteractableData(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Sets the interactable's data table row handle. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Interactable Data"))
	static void SetInteractableData(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const FDataTableRowHandle& NewData);

#pragma endregion

#pragma region CompatibleTags

	/** Returns the gameplay tags this interactable exposes to interactors. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Tags",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactable Compatible Tags"))
	static FGameplayTagContainer GetInteractableCompatibleTags(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Replaces the full compatible tag container. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Tags",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Interactable Compatible Tags"))
	static void SetInteractableCompatibleTags(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const FGameplayTagContainer& Tags);

	/** Adds a single compatible tag. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Tags",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Add Interactable Compatible Tag"))
	static void AddInteractableCompatibleTag(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const FGameplayTag& Tag);

	/** Adds multiple compatible tags. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Tags",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Add Interactable Compatible Tags"))
	static void AddInteractableCompatibleTags(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const FGameplayTagContainer& Tags);

	/** Removes a single compatible tag. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Tags",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Remove Interactable Compatible Tag"))
	static void RemoveInteractableCompatibleTag(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const FGameplayTag& Tag);

	/** Removes multiple compatible tags. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Tags",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Remove Interactable Compatible Tags"))
	static void RemoveInteractableCompatibleTags(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const FGameplayTagContainer& Tags);

	/** Clears all compatible tags. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Tags",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Clear Interactable Compatible Tags"))
	static void ClearInteractableCompatibleTags(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

#pragma endregion

#pragma region Setup

	/** Returns true if the interactable will auto-discover and register collision/highlight components. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Setup",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Does Interactable Auto Setup"))
	static bool DoesInteractableAutoSetup(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Sets the auto-setup mode. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Setup",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Interactable Auto Setup"))
	static void SetInteractableAutoSetup(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const ESetupType NewSetupType);

	/** Re-runs the full interactable setup (discovers components and re-applies collision/highlight). */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Setup",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Set Interactable Defaults"))
	static void SetInteractableDefaults(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

#pragma endregion

#pragma region Highlight

	/** Starts the highlight effect on all registered highlightable meshes. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Highlight",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Start Interactable Highlight"))
	static void StartInteractableHighlight(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Stops the highlight effect on all registered highlightable meshes. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Highlight",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Stop Interactable Highlight"))
	static void StopInteractableHighlight(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns the highlight type (PostProcessing or OverlayMaterial). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Highlight",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactable Highlight Type"))
	static EHighlightType GetInteractableHighlightType(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Sets the highlight type. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Highlight",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Interactable Highlight Type"))
	static void SetInteractableHighlightType(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const EHighlightType NewHighlightType);

	/** Returns the overlay material used for OverlayMaterial highlight mode. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Highlight",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactable Highlight Material"))
	static UMaterialInterface* GetInteractableHighlightMaterial(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Sets the overlay material. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Highlight",
		meta=(MounteaSetter),
		meta=(DisplayName="Interactable - Set Interactable Highlight Material"))
	static void SetInteractableHighlightMaterial(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		UMaterialInterface* NewMaterial);

#pragma endregion

#pragma region CollisionComponents

	/** Returns all registered collision primitives. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Collision",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactable Collision Components"))
	static TArray<UPrimitiveComponent*> GetInteractableCollisionComponents(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Adds a primitive to the collision set and binds it. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Collision",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Add Interactable Collision Component"))
	static void AddInteractableCollisionComponent(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		UPrimitiveComponent* CollisionComp);

	/** Removes a primitive from the collision set and restores its pre-interaction state. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Collision",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Remove Interactable Collision Component"))
	static void RemoveInteractableCollisionComponent(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		UPrimitiveComponent* CollisionComp);

	/** Returns all registered highlightable mesh components. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Highlight",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactable Highlightable Components"))
	static TArray<UMeshComponent*> GetInteractableHighlightableComponents(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Adds a mesh to the highlightable set. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Highlight",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Add Interactable Highlightable Component"))
	static void AddInteractableHighlightableComponent(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		UMeshComponent* HighlightableComp);

	/** Removes a mesh from the highlightable set. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Highlight",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Remove Interactable Highlightable Component"))
	static void RemoveInteractableHighlightableComponent(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		UMeshComponent* HighlightableComp);

#pragma endregion

#pragma region IgnoredClasses

	/** Returns the list of classes ignored for overlap detection. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Ignored Classes"))
	static TArray<TSoftClassPtr<UObject>> GetIgnoredClasses(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Adds a class to the ignore list. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Add Ignored Class"))
	static void AddIgnoredClass(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const TSoftClassPtr<UObject>& IgnoredClass);

	/** Removes a class from the ignore list. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Remove Ignored Class"))
	static void RemoveIgnoredClass(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const TSoftClassPtr<UObject>& IgnoredClass);

#pragma endregion

#pragma region Dependencies

	/** Returns interactable dependencies (interactables that must complete before this one activates). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactable Dependencies"))
	static TArray<TScriptInterface<IMounteaInteractableInterface>> GetInteractableDependencies(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Adds an interactable dependency. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Add Interactable Dependency"))
	static void AddInteractableDependency(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const TScriptInterface<IMounteaInteractableInterface>& Dependency);

	/** Removes an interactable dependency. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Remove Interactable Dependency"))
	static void RemoveInteractableDependency(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const TScriptInterface<IMounteaInteractableInterface>& Dependency);

#pragma endregion

#pragma region Debug

	/** Shows or hides the interactable's widget component. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Debug",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Toggle Interactable Widget Visibility"))
	static void ToggleInteractableWidgetVisibility(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const bool bVisible);

	/** Returns the debug settings for this interactable. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Debug",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Get Interactable Debug Settings"))
	static FDebugSettings GetInteractableDebugSettings(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Toggles runtime and editor debug modes on the interactable. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactable|Debug",
		meta=(MounteaCommand),
		meta=(DisplayName="Interactable - Toggle Interactable Debug"))
	static void ToggleInteractableDebug(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

#pragma endregion

#pragma region LegacyPorted

	/** Returns the plugin's settings singleton. Ported from the pre-refactor
	 *  UMounteaInteractionFunctionLibrary::GetInteractionSettings and
	 *  UMounteaInteractionSystemBFL::GetInteractionSystemSettings, both now deprecated redirects. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Settings",
		meta=(MounteaGetter),
		meta=(DisplayName="Interaction Settings - Get Interaction System Settings"))
	static UMounteaInteractionSystemSettings* GetInteractionSystemSettings();

	/** Returns the configured default interaction-widget refresh frequency (seconds). Ported from
	 *  the pre-refactor UMounteaInteractionFunctionLibrary::GetDefaultWidgetUpdateFrequency. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Settings",
		meta=(MounteaGetter),
		meta=(DisplayName="Interaction Settings - Get Default Widget Update Frequency"))
	static float GetDefaultWidgetUpdateFrequency();

	/** Returns the configured default interactable data table. Ported from the pre-refactor
	 *  UMounteaInteractionFunctionLibrary::GetInteractableDefaultDataTable. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Settings",
		meta=(MounteaGetter),
		meta=(DisplayName="Interaction Settings - Get Interactable Default Data Table"))
	static UDataTable* GetInteractableDefaultDataTable();

	/** Returns the configured default interaction widget class. Ported from the pre-refactor
	 *  UMounteaInteractionFunctionLibrary::GetInteractableDefaultWidgetClass. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Settings",
		meta=(MounteaGetter),
		meta=(DisplayName="Interaction Settings - Get Interactable Default Widget Class"))
	static TSubclassOf<UUserWidget> GetInteractableDefaultWidgetClass();

	/** Returns whether editor debug output is enabled in plugin settings. Ported from the
	 *  pre-refactor UMounteaInteractionFunctionLibrary::IsEditorDebugEnabled. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Settings",
		meta=(MounteaGetter),
		meta=(DisplayName="Interaction Settings - Is Editor Debug Enabled"))
	static bool IsEditorDebugEnabled();

	/** Finds the first mesh component tagged with Tag on Source. Ported from the pre-refactor
	 *  UMounteaInteractionSystemBFL::FindMeshByTag. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Setup",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Find Mesh By Tag"))
	static UMeshComponent* FindMeshByTag(const FName Tag, const AActor* Source);

	/** Finds the first mesh component named Name on Source. Ported from the pre-refactor
	 *  UMounteaInteractionSystemBFL::FindMeshByName. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Setup",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Find Mesh By Name"))
	static UMeshComponent* FindMeshByName(const FName Name, const AActor* Source);

	/** Finds the first primitive component tagged with Tag on Source. Ported from the pre-refactor
	 *  UMounteaInteractionSystemBFL::FindPrimitiveByTag. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Setup",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Find Primitive By Tag"))
	static UPrimitiveComponent* FindPrimitiveByTag(const FName Tag, const AActor* Source);

	/** Finds the first primitive component named Name on Source. Ported from the pre-refactor
	 *  UMounteaInteractionSystemBFL::FindPrimitiveByName. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable|Setup",
		meta=(MounteaGetter),
		meta=(DisplayName="Interactable - Find Primitive By Name"))
	static UPrimitiveComponent* FindPrimitiveByName(const FName Name, const AActor* Source);

#pragma endregion
};
