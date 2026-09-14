// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "MounteaInteractorStatics.generated.h"

class IMounteaInteractorInterface;
class IMounteaInteractableInterface;
class UInputAction;
class UInputMappingContext;

/**
 * Static helpers scoped to interactor components.
 * All focus/state/property queries and mutations for IMounteaInteractorInterface.
 */
UCLASS()
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractorStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

#pragma region FocusQueries

	/** Returns the actor currently focused by this interactor (Tier 1 selection). Null if nothing in range. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|Focus",
		meta=(MounteaGetter))
	static AActor* GetFocusedActor(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns the number of actions available on the focused actor (Tier 2 action list size). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|Focus",
		meta=(MounteaGetter))
	static int32 GetActionCount(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns the full Tier 2 action candidate list for the focused actor. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|Focus",
		meta=(MounteaGetter))
	static TArray<FInteractionActionCandidate> GetActionList(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns the currently highlighted action candidate. Returns default if list is empty. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|Focus",
		meta=(MounteaGetter))
	static FInteractionActionCandidate GetHighlightedAction(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns the currently highlighted action index within the action list. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|Focus",
		meta=(MounteaGetter))
	static int32 GetHighlightedActionIndex(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns the interactable currently being interacted with. Null if not interacting. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|Focus",
		meta=(MounteaGetter))
	static TScriptInterface<IMounteaInteractableInterface> GetLockedAction(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns true if an interaction is in progress (action selection is locked). */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor|Focus",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"))
	static bool IsSelectionLocked(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns true if the interactor has a focused actor with at least one action. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor|Focus",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"))
	static bool HasInteractable(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

#pragma endregion

#pragma region StateQueries

	/** Returns the current interactor state. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|State",
		meta=(MounteaGetter))
	static EInteractorStateV3 GetInteractorState(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns true if the interactor can currently begin an interaction. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor|State",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"))
	static bool CanInteract(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns the interactor's collision response channel. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|State",
		meta=(MounteaGetter))
	static ECollisionChannel GetInteractorCollisionChannel(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns the interactor's gameplay tag (used for slot validation on interactables). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|State",
		meta=(MounteaGetter))
	static FGameplayTag GetInteractorTag(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

#pragma endregion

#pragma region FocusActions

	/**
	 * Cycles the highlighted action index by Direction (+1 = next, -1 = previous).
	 * No-op when selection is locked or the action list has fewer than 2 entries.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor|Focus",
		meta=(MounteaCommand))
	static void CycleInteractorAction(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		const int32 Direction);

#pragma endregion

#pragma region InteractionActions

	/**
	 * Starts interaction with the currently highlighted action.
	 * Sets selection locked = true. Routes to server if not authority.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand, DefaultToSelf="WorldContext", HidePin="WorldContext"))
	static void StartInteractorInteraction(
		const UObject* WorldContext,
		const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/**
	 * Stops the current interaction and unlocks the action selection.
	 * Routes to server if not authority.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand, DefaultToSelf="WorldContext", HidePin="WorldContext"))
	static void StopInteractorInteraction(
		const UObject* WorldContext,
		const TScriptInterface<IMounteaInteractorInterface>& Interactor);

#pragma endregion

#pragma region StateMutations

	/** Attempts to activate the interactor (transition to Scanning). Populates ErrorMessage on failure. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor|State",
		meta=(MounteaCommand, ExpandBoolAsExecs="ReturnValue"))
	static bool ActivateInteractor(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		FString& ErrorMessage);

	/** Disables the interactor (stops all detection and interaction). */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor|State",
		meta=(MounteaCommand))
	static void DeactivateInteractor(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Suppresses the interactor (pauses detection without full disable). */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor|State",
		meta=(MounteaCommand, ExpandBoolAsExecs="ReturnValue"))
	static bool SuppressInteractor(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		FString& ErrorMessage);

	/** Sets the interactor's collision response channel. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor|State",
		meta=(MounteaSetter))
	static void SetInteractorCollisionChannel(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		const ECollisionChannel NewChannel);

	/** Sets the interactor's gameplay tag. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor|State",
		meta=(MounteaSetter))
	static void SetInteractorTag(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		const FGameplayTag& NewTag);

#pragma endregion

#pragma region IgnoredActors

	/** Adds an actor to the interactor's ignore list (excluded from detection). */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand))
	static void AddIgnoredActor(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		AActor* ActorToIgnore);

	/** Removes an actor from the interactor's ignore list. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand))
	static void RemoveIgnoredActor(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		AActor* ActorToUnignore);

	/** Returns the full list of actors currently ignored by this interactor. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(MounteaGetter))
	static TArray<AActor*> GetIgnoredActors(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Adds multiple actors to the ignore list in a single call. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand))
	static void AddIgnoredActors(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		const TArray<AActor*>& ActorsToIgnore);

	/** Removes multiple actors from the ignore list in a single call. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand))
	static void RemoveIgnoredActors(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		const TArray<AActor*>& ActorsToUnignore);

#pragma endregion

#pragma region Properties

	/** Returns true if the interactor activates automatically on BeginPlay. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(MounteaGetter))
	static bool DoesInteractorAutoActivate(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns the actor that owns this interactor component. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(MounteaGetter))
	static AActor* GetInteractorOwningActor(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Returns the safety tracing setup (LoS validation config). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(MounteaGetter))
	static FSafetyTracingSetup GetSafetyTracingSetup(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Replaces the safety tracing setup. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaSetter))
	static void SetSafetyTracingSetup(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		const FSafetyTracingSetup& NewSetup);

#pragma endregion

#pragma region Setup

	/** Re-runs the interactor setup (re-discovers collision shapes and re-applies channel config). */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand))
	static void SetInteractorDefaults(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Performs a line-of-sight safety trace toward the given actor. Returns false if blocked. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"))
	static bool PerformSafetyTrace(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		const AActor* InteractableActor);

	/** Consumes the given input action to suppress further interactor processing this frame. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand))
	static void ConsumeInteractorInput(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		UInputAction* ConsumedInput);

	/** Adds Interactor Runtime Config's Default Input Mapping Context (if assigned) to whichever
	 *  local player controls this interactor's owning actor, via AddMappingContext. Safe no-op if
	 *  unassigned, if the owner isn't a locally-controlled Pawn, or if no Enhanced Input subsystem is
	 *  available (server-only/AI-controlled actors). Not called automatically anywhere — call it
	 *  yourself (e.g. from BeginPlay, or on possession) wherever fits your project's flow. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand))
	static void ApplyDefaultInputMappingContext(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

#pragma endregion

#pragma region Dependencies

	/** Returns the list of interactor dependencies. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(MounteaGetter))
	static TArray<TScriptInterface<IMounteaInteractorInterface>> GetInteractorDependencies(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Adds an interactor dependency. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand))
	static void AddInteractorDependency(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		const TScriptInterface<IMounteaInteractorInterface>& Dependency);

	/** Removes an interactor dependency. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand))
	static void RemoveInteractorDependency(
		const TScriptInterface<IMounteaInteractorInterface>& Interactor,
		const TScriptInterface<IMounteaInteractorInterface>& Dependency);

	/** Evaluates all dependencies and transitions state accordingly. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(MounteaCommand))
	static void ProcessInteractorDependencies(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

#pragma endregion

#pragma region Debug

	/** Returns the debug settings for this interactor. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|Debug",
		meta=(MounteaGetter))
	static FDebugSettings GetInteractorDebugSettings(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/** Toggles runtime and editor debug modes on the interactor. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Interactor|Debug",
		meta=(MounteaCommand))
	static void ToggleInteractorDebug(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

#pragma endregion
};
