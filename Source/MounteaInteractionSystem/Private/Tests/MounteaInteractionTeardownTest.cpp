// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/Interactable/MounteaInteractableComponentPress.h"
#include "Components/Interactor/MounteaInteractorComponentOverlap.h"
#include "Components/Interactor/MounteaInteractorComponentBase.h"
#include "Helpers/MounteaInteractableStatics.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Interfaces/MounteaInteractorInterface.h"
#include "Tests/MounteaInteractionTestAccess.h"

// NOTE on scope: these tests exercise the exact release calls MounteaInteractorComponentBase's
// and MounteaInteractableComponentBase's EndPlay() overrides invoke (RequestInteractorLeave /
// ForceReleaseInteraction) directly - bare NewObject() components never go through real
// registration/BeginPlay, so UActorComponent::EndPlay() itself (which asserts check(bHasBegunPlay))
// can't be called on them. The real EndPlay()-triggers-release wiring, on actual registered actors
// in a minimal UWorld, is covered separately by MounteaInteractionWorldTest.cpp.

namespace MounteaInteractionTeardownTest
{
	static bool SetUpJoinedAndLockedPair(UMounteaInteractableComponentPress*& OutInteractable, UMounteaInteractorComponentOverlap*& OutInteractor,
		TScriptInterface<IMounteaInteractableInterface>& OutInteractableScript, TScriptInterface<IMounteaInteractorInterface>& OutInteractorScript)
	{
		OutInteractable = NewObject<UMounteaInteractableComponentPress>(GetTransientPackage());
		OutInteractor = NewObject<UMounteaInteractorComponentOverlap>(GetTransientPackage());
		if (!OutInteractable || !OutInteractor) return false;

		OutInteractableScript.SetObject(OutInteractable);
		OutInteractableScript.SetInterface(OutInteractable);

		OutInteractorScript.SetObject(OutInteractor);
		OutInteractorScript.SetInterface(OutInteractor);

		IMounteaInteractableInterface::Execute_RequestInteractorJoin(OutInteractable, OutInteractorScript);
		FMounteaInteractionTestAccess::ForceLock(OutInteractor, OutInteractableScript);
		return true;
	}
}

// Regression test for Phase 2: RequestInteractorLeave (what a destroyed interactor's EndPlay now
// calls) must actually release the slot, or a MaxInteractors=1 object (door, lever) stays
// soft-locked for every other player forever.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionRequestInteractorLeaveTest,
	"Mountea.InteractionSystem.Pipeline.RequestInteractorLeaveReleasesSlot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionRequestInteractorLeaveTest::RunTest(const FString& Parameters)
{
	UMounteaInteractableComponentPress* interactable = nullptr;
	UMounteaInteractorComponentOverlap* interactor = nullptr;
	TScriptInterface<IMounteaInteractableInterface> interactableScript;
	TScriptInterface<IMounteaInteractorInterface> interactorScript;

	if (!MounteaInteractionTeardownTest::SetUpJoinedAndLockedPair(interactable, interactor, interactableScript, interactorScript))
	{
		AddError(TEXT("Failed to construct test components"));
		return false;
	}

	TestEqual(TEXT("Precondition: interactor should hold one slot after joining"),
		UMounteaInteractableStatics::GetInteractorSlotCount(interactableScript), 1);

	IMounteaInteractableInterface::Execute_RequestInteractorLeave(interactable, interactorScript);

	TestEqual(TEXT("RequestInteractorLeave must release the slot, not leave a phantom entry"),
		UMounteaInteractableStatics::GetInteractorSlotCount(interactableScript), 0);

	return true;
}

// Regression test for Phase 2: ForceReleaseInteraction (what an interactable destroyed while
// occupied now calls on each of its interactors) must clear the interactor's lock, or that player
// can never start any other interaction for the rest of the session.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionForceReleaseInteractionTest,
	"Mountea.InteractionSystem.Pipeline.ForceReleaseInteractionClearsLock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionForceReleaseInteractionTest::RunTest(const FString& Parameters)
{
	UMounteaInteractableComponentPress* interactable = nullptr;
	UMounteaInteractorComponentOverlap* interactor = nullptr;
	TScriptInterface<IMounteaInteractableInterface> interactableScript;
	TScriptInterface<IMounteaInteractorInterface> interactorScript;

	if (!MounteaInteractionTeardownTest::SetUpJoinedAndLockedPair(interactable, interactor, interactableScript, interactorScript))
	{
		AddError(TEXT("Failed to construct test components"));
		return false;
	}

	TestTrue(TEXT("Precondition: interactor should be locked before it is force-released"),
		IMounteaInteractorInterface::Execute_IsSelectionLocked(interactor));

	IMounteaInteractorInterface::Execute_ForceReleaseInteraction(interactor);

	TestFalse(TEXT("ForceReleaseInteraction must clear the lock, or the interactor can never interact again"),
		IMounteaInteractorInterface::Execute_IsSelectionLocked(interactor));

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
