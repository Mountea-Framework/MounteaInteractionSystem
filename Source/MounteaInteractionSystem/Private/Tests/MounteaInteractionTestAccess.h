// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/Interactor/MounteaInteractorComponentBase.h"
#include "Components/Interactable/MounteaInteractableComponentBase.h"
#include "Interfaces/MounteaInteractableInterface.h"

// Shared across the Tests/ folder — grants direct access to state/methods that have no public
// interface equivalent (see the friend declarations on both component base classes), so tests can
// set up preconditions (a locked interactor, a populated DetectedSet, a tag requirement) without
// driving the full physical detection pipeline (overlap volumes / traces), which needs a real
// physics world and is out of scope for these tests.
//
// NOTE on scope: AddDetected/RefreshFocus exercise the real focus-selection logic once a
// precondition is injected directly — they are not a substitute for testing the detection
// subclasses (Overlap/Trace/Mouse) themselves, which still need PIE/physics to verify.
struct FMounteaInteractionTestAccess
{
	static void ForceLock(UMounteaInteractorComponentBase* Interactor, const TScriptInterface<IMounteaInteractableInterface>& Interactable)
	{
		Interactor->bSelectionLocked = true;
		Interactor->LockedAction = Interactable;
	}

	static void AddDetected(UMounteaInteractorComponentBase* Interactor, const TScriptInterface<IMounteaInteractableInterface>& Interactable)
	{
		Interactor->AddDetected(Interactable);
	}

	static void RefreshFocus(UMounteaInteractorComponentBase* Interactor)
	{
		Interactor->RefreshFocus();
	}

	static void SetMaxCandidatesPerActor(UMounteaInteractorComponentBase* Interactor, const int32 Max)
	{
		Interactor->MaxCandidatesPerActor = Max;
	}

	static void SetHighlightedActionIndex(UMounteaInteractorComponentBase* Interactor, const int32 Index)
	{
		Interactor->HighlightedActionIndex = Index;
	}

	static bool IsBlockedBySiblingInteractable(UMounteaInteractableComponentBase* Interactable)
	{
		return Interactable->IsBlockedBySiblingInteractable();
	}

	static void SetRequiredInteractorTags(UMounteaInteractableComponentBase* Interactable, const FGameplayTagContainer& Tags)
	{
		Interactable->RequiredInteractorTags = Tags;
	}

	static void SetExcludedInteractorTags(UMounteaInteractableComponentBase* Interactable, const FGameplayTagContainer& Tags)
	{
		Interactable->ExcludedInteractorTags = Tags;
	}

	// Simulates "replication already delivered this value" — sets the raw member directly, bypassing
	// SetState/etc., so a test can pre-seed state BEFORE BeginPlay() runs and confirm a non-authority
	// BeginPlay() does not stomp it back to DefaultInteractableState/LifecycleCount.
	static void SetReplicatedStateForTest(UMounteaInteractableComponentBase* Interactable, EInteractableStateV3 State, int32 RemainingLifecycle)
	{
		Interactable->InteractableState = State;
		Interactable->RemainingLifecycleCount = RemainingLifecycle;
	}

	// Same idea for the interactor side: pre-seed InteractorState BEFORE BeginPlay() to confirm a
	// non-authority BeginPlay() does not stomp it back to DefaultInteractorState.
	static void SetReplicatedStateForTest(UMounteaInteractorComponentBase* Interactor, EInteractorStateV3 State)
	{
		Interactor->InteractorState = State;
	}

	// Lets a test set a distinctive, non-default LifecycleCount BEFORE BeginPlay() runs, so the
	// authority BeginPlay() init path (RemainingLifecycleCount = LifecycleCount) is provably
	// exercised rather than coincidentally matching its own default value.
	static void SetLifecycleCount(UMounteaInteractableComponentBase* Interactable, const int32 Count)
	{
		Interactable->LifecycleCount = Count;
	}

	// Same idea for the interactor side: a distinctive DefaultInteractorState set BEFORE
	// BeginPlay() proves the authority init path (InteractorState = DefaultInteractorState) ran.
	static void SetDefaultInteractorState(UMounteaInteractorComponentBase* Interactor, const EInteractorStateV3 State)
	{
		Interactor->DefaultInteractorState = State;
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
