// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/Interactable/MounteaInteractableComponentPress.h"
#include "Components/Interactor/MounteaInteractorComponentOverlap.h"
#include "Components/Interactor/MounteaInteractorComponentBase.h"
#include "Helpers/MounteaInteractableStatics.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Interfaces/MounteaInteractorInterface.h"

// Grants tests direct access to UMounteaInteractorComponentBase::bSelectionLocked/LockedAction
// (see the friend declaration on that class) so the two tests below can set up a "mid-interaction"
// precondition without driving the full RefreshFocus/ActionList detection pipeline (which needs a
// real physics world and is out of scope for these pure-logic tests).
//
// NOTE on scope: these tests exercise the exact release calls MounteaInteractorComponentBase's
// and MounteaInteractableComponentBase's EndPlay() overrides invoke (RequestInteractorLeave /
// ForceReleaseInteraction) — not EndPlay() itself. UActorComponent::EndPlay() asserts
// check(bHasBegunPlay), which requires the component to have gone through real registration +
// BeginPlay in an actual UWorld; a bare NewObject() component can't satisfy that (confirmed by an
// actual crash during initial authoring — this comment exists so nobody "fixes" that by just
// adding a BeginPlay() call and hits the same check(bRegistered) wall one step earlier). Testing
// the real EndPlay-triggers-release wiring needs a registered-actor/minimal-world test harness,
// which is a larger, separate investment.
struct FMounteaInteractionTestAccess
{
	static void ForceLock(UMounteaInteractorComponentBase* Interactor, const TScriptInterface<IMounteaInteractableInterface>& Interactable)
	{
		Interactor->bSelectionLocked = true;
		Interactor->LockedAction = Interactable;
	}
};

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
