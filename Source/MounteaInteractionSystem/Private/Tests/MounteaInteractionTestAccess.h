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

	static void SetRequiredInteractorTags(UMounteaInteractableComponentBase* Interactable, const FGameplayTagContainer& Tags)
	{
		Interactable->RequiredInteractorTags = Tags;
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
