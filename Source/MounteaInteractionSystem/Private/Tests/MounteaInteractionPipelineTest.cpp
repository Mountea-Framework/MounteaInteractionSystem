// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/Interactable/MounteaInteractableComponentPress.h"
#include "Components/Interactor/MounteaInteractorComponentOverlap.h"
#include "Helpers/MounteaInteractableStatics.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Interfaces/MounteaInteractorInterface.h"

// Regression test for the Phase 1 pipeline fix: RequestInteractorJoin_Implementation must not
// let the same interactor occupy two slots on a re-entrant/duplicate join call.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionDuplicateJoinGuardTest,
	"Mountea.InteractionSystem.Pipeline.DuplicateJoinIsRejected",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionDuplicateJoinGuardTest::RunTest(const FString& Parameters)
{
	UMounteaInteractableComponentPress* interactable = NewObject<UMounteaInteractableComponentPress>(GetTransientPackage());
	UMounteaInteractorComponentOverlap* interactor = NewObject<UMounteaInteractorComponentOverlap>(GetTransientPackage());

	TestNotNull(TEXT("Interactable should construct"), interactable);
	TestNotNull(TEXT("Interactor should construct"), interactor);
	if (!interactable || !interactor) return false;

	TScriptInterface<IMounteaInteractableInterface> interactableScript;
	interactableScript.SetObject(interactable);
	interactableScript.SetInterface(interactable);

	TScriptInterface<IMounteaInteractorInterface> interactorScript;
	interactorScript.SetObject(interactor);
	interactorScript.SetInterface(interactor);

	IMounteaInteractableInterface::Execute_RequestInteractorJoin(interactable, interactorScript);
	IMounteaInteractableInterface::Execute_RequestInteractorJoin(interactable, interactorScript);

	const int32 slotCount = UMounteaInteractableStatics::GetInteractorSlotCount(interactableScript);
	TestEqual(TEXT("A duplicate join call must not create a second slot for the same interactor"), slotCount, 1);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
