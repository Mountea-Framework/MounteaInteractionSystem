// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "MounteaInteractionStatics.generated.h"

class IMounteaInteractableInterface;
class IMounteaInteractorInterface;

/**
 * General-purpose interaction system static helpers.
 * Functions tagged meta=(MounteaGetter) are read-only queries.
 * Functions tagged meta=(MounteaValidate) expand to bool exec pins.
 * Functions tagged meta=(MounteaCommand) are action triggers.
 * Functions tagged meta=(MounteaSetter) mutate state.
 */
UCLASS()
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractionStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Returns the current server world time in seconds. Safe to call from clients. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Statics",
		meta=(MounteaGetter, DefaultToSelf="WorldContext", HidePin="WorldContext"))
	static float GetServerTime(const UObject* WorldContext);

	/**
	 * Returns true if the given interactable has at least one open slot AND the interactor
	 * passes tag validation. Use to guard RequestInteractorJoin calls.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Statics",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"))
	static bool CanInteractorJoinSlot(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/**
	 * Sorts the action candidate list by weight descending (highest weight first).
	 * Tie-break: closer distance (smaller DistanceSq) wins.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Statics",
		meta=(MounteaCommand))
	static void SortActionCandidates(UPARAM(ref) TArray<FInteractionActionCandidate>& Candidates);

	/**
	 * Finds all IMounteaInteractableInterface components on Actor and returns them as
	 * FInteractionActionCandidate entries, computing distance from ViewOrigin.
	 * Candidates are sorted by weight descending before returning.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Statics",
		meta=(MounteaGetter))
	static TArray<FInteractionActionCandidate> BuildActionCandidatesForActor(
		const AActor* Actor,
		const FVector& ViewOrigin);

	/**
	 * From a set of detected interactable components, selects the best actor to focus.
	 * "Best" = the actor whose highest-weight component wins; ties broken by squared distance
	 * from ViewOrigin. Returns nullptr if DetectedInteractables is empty.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Statics",
		meta=(MounteaGetter))
	static AActor* SelectBestFocusActor(
		const TArray<TScriptInterface<IMounteaInteractableInterface>>& DetectedInteractables,
		const FVector& ViewOrigin);

	/**
	 * Returns true if the given interactable component's owner actor is within
	 * the specified squared distance from ViewOrigin.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Statics",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"))
	static bool IsInteractableInRange(
		const TScriptInterface<IMounteaInteractableInterface>& Interactable,
		const FVector& ViewOrigin,
		const float RangeCm);

	/** Returns true if the object implements IMounteaInteractableInterface. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Statics",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"))
	static bool IsValidInteractable(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Returns true if the object implements IMounteaInteractorInterface. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Statics",
		meta=(MounteaValidate, ExpandBoolAsExecs="ReturnValue"))
	static bool IsValidInteractor(const TScriptInterface<IMounteaInteractorInterface>& Interactor);

	/**
	 * Finds the first IMounteaInteractableInterface component on the actor.
	 * Returns null if none found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Statics",
		meta=(MounteaGetter))
	static TScriptInterface<IMounteaInteractableInterface> GetFirstInteractableFromActor(const AActor* Actor);

	/**
	 * Finds the first IMounteaInteractorInterface component on the actor.
	 * Returns null if none found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Statics",
		meta=(MounteaGetter))
	static TScriptInterface<IMounteaInteractorInterface> GetFirstInteractorFromActor(const AActor* Actor);

	/**
	 * Resolves a collision channel by display name using the project's collision profile.
	 * Returns ECC_MAX if the channel name is not registered.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Statics",
		meta=(MounteaGetter))
	static ECollisionChannel ResolveInteractionChannel(const FName& ChannelName, bool& bFound);

	/** Resolves a channel by name and logs a warning if not found. Returns ECC_MAX on failure. */
	static ECollisionChannel ResolveChannelOrWarn(const FName& ChannelName);
};
