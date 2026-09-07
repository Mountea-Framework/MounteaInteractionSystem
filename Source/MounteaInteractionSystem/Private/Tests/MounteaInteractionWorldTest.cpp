// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Tests/AutomationCommon.h"
#include "Tests/MounteaInteractionTestAccess.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "NativeGameplayTags.h"
#include "Components/Interactable/MounteaInteractableComponentPress.h"
#include "Components/Interactable/MounteaInteractableComponentMash.h"
#include "Components/Interactor/MounteaInteractorComponentOverlap.h"
#include "Helpers/MounteaInteractableStatics.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Interfaces/MounteaInteractorInterface.h"

// Tests in this file spin up a real, minimal UWorld (via the engine's own FTestWorldWrapper —
// Engine module, Runtime-safe) so spawned actors go through actual registration/BeginPlay/EndPlay,
// unlike the bare NewObject() components used elsewhere in this Tests/ folder. This closes the
// coverage gap those other tests explicitly call out: EndPlay() itself, and anything that depends
// on a real actor transform (distance-based focus selection), was previously untested.
//
// Detection (Overlap/Trace/Mouse discovering an interactable via physics) still isn't exercised
// here — these tests inject DetectedSet entries directly via FMounteaInteractionTestAccess and
// focus on what happens once something is detected, not on detection itself.

namespace MounteaInteractionWorldTest
{
	static AActor* SpawnActorWithRoot(UWorld* World, const FVector& Location)
	{
		AActor* Actor = World->SpawnActor<AActor>();
		if (!Actor) return nullptr;

		USceneComponent* Root = NewObject<USceneComponent>(Actor);
		Actor->SetRootComponent(Root);
		Root->RegisterComponent();
		Actor->SetActorLocation(Location);
		return Actor;
	}

	template<typename TComponent>
	static TComponent* AddInteractableComponent(AActor* Owner)
	{
		TComponent* Comp = NewObject<TComponent>(Owner, TComponent::StaticClass());
		Owner->AddInstanceComponent(Comp);
		Comp->RegisterComponent();
		return Comp;
	}

	template<typename TComponent>
	static TScriptInterface<IMounteaInteractableInterface> ToInteractableScript(TComponent* Comp)
	{
		TScriptInterface<IMounteaInteractableInterface> Script;
		Script.SetObject(Comp);
		Script.SetInterface(Comp);
		return Script;
	}

	static TScriptInterface<IMounteaInteractorInterface> ToInteractorScript(UMounteaInteractorComponentOverlap* Comp)
	{
		TScriptInterface<IMounteaInteractorInterface> Script;
		Script.SetObject(Comp);
		Script.SetInterface(Comp);
		return Script;
	}
}

// Regression test for Phase 2: closes the previously-documented gap where only the inner release
// calls (RequestInteractorLeave / ForceReleaseInteraction) were tested, never EndPlay() itself.
// Destroying a locked interactor's owner must route through the real AActor::EndPlay() ->
// UMounteaInteractorComponentBase::EndPlay() chain and release the interactable's slot.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionRealEndPlayReleasesSlotTest,
	"Mountea.InteractionSystem.World.EndPlay.InteractorDestroyReleasesSlot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionRealEndPlayReleasesSlotTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionWorldTest;

	FTestWorldWrapper WorldWrapper;
	if (!WorldWrapper.CreateTestWorld(EWorldType::Game))
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}
	UWorld* World = WorldWrapper.GetTestWorld();
	if (!World)
	{
		AddError(TEXT("Test world is null"));
		return false;
	}
	WorldWrapper.BeginPlayInTestWorld();

	AActor* InteractableOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	AActor* InteractorOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!InteractableOwner || !InteractorOwner)
	{
		AddError(TEXT("Failed to spawn test actors"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractableComponentPress* Interactable = AddInteractableComponent<UMounteaInteractableComponentPress>(InteractableOwner);
	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);

	const TScriptInterface<IMounteaInteractableInterface> InteractableScript = ToInteractableScript(Interactable);
	const TScriptInterface<IMounteaInteractorInterface> InteractorScript = ToInteractorScript(Interactor);

	IMounteaInteractableInterface::Execute_RequestInteractorJoin(Interactable, InteractorScript);
	FMounteaInteractionTestAccess::ForceLock(Interactor, InteractableScript);

	TestEqual(TEXT("Precondition: interactable should have one occupied slot"),
		UMounteaInteractableStatics::GetInteractorSlotCount(InteractableScript), 1);

	// Real destruction — routes through AActor::EndPlay(), not a direct method call.
	InteractorOwner->Destroy();

	TestEqual(TEXT("Destroying the interactor's owner must release its slot via real EndPlay()"),
		UMounteaInteractableStatics::GetInteractorSlotCount(InteractableScript), 0);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Reverse direction of the above: destroying an occupied interactable's owner must route through
// real EndPlay() and clear the occupying interactor's lock (ForceReleaseInteraction), or that
// player could never start another interaction for the rest of the session.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionRealEndPlayClearsLockTest,
	"Mountea.InteractionSystem.World.EndPlay.InteractableDestroyClearsLock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionRealEndPlayClearsLockTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionWorldTest;

	FTestWorldWrapper WorldWrapper;
	if (!WorldWrapper.CreateTestWorld(EWorldType::Game))
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}
	UWorld* World = WorldWrapper.GetTestWorld();
	if (!World)
	{
		AddError(TEXT("Test world is null"));
		return false;
	}
	WorldWrapper.BeginPlayInTestWorld();

	AActor* InteractableOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	AActor* InteractorOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!InteractableOwner || !InteractorOwner)
	{
		AddError(TEXT("Failed to spawn test actors"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractableComponentPress* Interactable = AddInteractableComponent<UMounteaInteractableComponentPress>(InteractableOwner);
	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);

	const TScriptInterface<IMounteaInteractableInterface> InteractableScript = ToInteractableScript(Interactable);
	const TScriptInterface<IMounteaInteractorInterface> InteractorScript = ToInteractorScript(Interactor);

	IMounteaInteractableInterface::Execute_RequestInteractorJoin(Interactable, InteractorScript);
	FMounteaInteractionTestAccess::ForceLock(Interactor, InteractableScript);

	TestTrue(TEXT("Precondition: interactor should be locked"),
		IMounteaInteractorInterface::Execute_IsSelectionLocked(Interactor));

	InteractableOwner->Destroy();

	TestFalse(TEXT("Destroying the interactable's owner must clear the interactor's lock via real EndPlay()"),
		IMounteaInteractorInterface::Execute_IsSelectionLocked(Interactor));

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Regression test for Tier 1 focus selection with real actor transforms: two equal-weight
// interactables should resolve in favor of the closer one, exercising RefreshFocus()'s actual
// FVector::DistSquared comparison rather than assuming it works from reading the code.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionFocusPicksClosestTest,
	"Mountea.InteractionSystem.World.Focus.EqualWeightPicksClosest",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionFocusPicksClosestTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionWorldTest;

	FTestWorldWrapper WorldWrapper;
	if (!WorldWrapper.CreateTestWorld(EWorldType::Game))
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}
	UWorld* World = WorldWrapper.GetTestWorld();
	if (!World)
	{
		AddError(TEXT("Test world is null"));
		return false;
	}
	WorldWrapper.BeginPlayInTestWorld();

	AActor* NearActor = SpawnActorWithRoot(World, FVector(100.f, 0.f, 0.f));
	AActor* FarActor = SpawnActorWithRoot(World, FVector(1000.f, 0.f, 0.f));
	AActor* InteractorOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!NearActor || !FarActor || !InteractorOwner)
	{
		AddError(TEXT("Failed to spawn test actors"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractableComponentPress* NearInteractable = AddInteractableComponent<UMounteaInteractableComponentPress>(NearActor);
	UMounteaInteractableComponentPress* FarInteractable = AddInteractableComponent<UMounteaInteractableComponentPress>(FarActor);
	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);

	// Add the farther one first so a pass would require a real distance comparison, not just
	// "first candidate wins."
	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(FarInteractable));
	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(NearInteractable));

	AActor* Focused = IMounteaInteractorInterface::Execute_GetFocusedActor(Interactor);
	TestEqual(TEXT("Interactor should focus the closer of two equal-weight interactables"), Focused, NearActor);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Regression test for Phase 6: MaxCandidatesPerActor must actually cap RefreshFocus()'s rebuilt
// ActionList, not just exist as an unread config field.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionMaxCandidatesCapTest,
	"Mountea.InteractionSystem.World.Focus.MaxCandidatesPerActorCapsActionList",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionMaxCandidatesCapTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionWorldTest;

	FTestWorldWrapper WorldWrapper;
	if (!WorldWrapper.CreateTestWorld(EWorldType::Game))
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}
	UWorld* World = WorldWrapper.GetTestWorld();
	if (!World)
	{
		AddError(TEXT("Test world is null"));
		return false;
	}
	WorldWrapper.BeginPlayInTestWorld();

	AActor* InteractableOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	AActor* InteractorOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!InteractableOwner || !InteractorOwner)
	{
		AddError(TEXT("Failed to spawn test actors"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);
	FMounteaInteractionTestAccess::SetMaxCandidatesPerActor(Interactor, 2);

	// Three interactable components on the same actor — one more than the cap.
	for (int32 i = 0; i < 3; ++i)
	{
		UMounteaInteractableComponentPress* Comp = AddInteractableComponent<UMounteaInteractableComponentPress>(InteractableOwner);
		FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Comp));
	}

	const TArray<FInteractionActionCandidate> ActionList = IMounteaInteractorInterface::Execute_GetActionList(Interactor);
	TestEqual(TEXT("ActionList must respect MaxCandidatesPerActor even when more candidates exist"), ActionList.Num(), 2);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_MounteaTest_InteractorA, "Mountea.Test.InteractorA");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_MounteaTest_InteractorB, "Mountea.Test.InteractorB");

// Regression test for Phase 6: the join-time RequiredInteractorTags gate is promoted to focus
// time — a tag-incompatible interactable must never win focus, not just get rejected on join.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionTagGateFocusTest,
	"Mountea.InteractionSystem.World.Focus.TagMismatchExcludesFromFocus",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionTagGateFocusTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionWorldTest;

	FTestWorldWrapper WorldWrapper;
	if (!WorldWrapper.CreateTestWorld(EWorldType::Game))
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}
	UWorld* World = WorldWrapper.GetTestWorld();
	if (!World)
	{
		AddError(TEXT("Test world is null"));
		return false;
	}
	WorldWrapper.BeginPlayInTestWorld();

	AActor* InteractableOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	AActor* InteractorOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!InteractableOwner || !InteractorOwner)
	{
		AddError(TEXT("Failed to spawn test actors"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractableComponentPress* Interactable = AddInteractableComponent<UMounteaInteractableComponentPress>(InteractableOwner);
	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);

	FGameplayTagContainer RequiredTags;
	RequiredTags.AddTag(TAG_MounteaTest_InteractorA.GetTag());
	FMounteaInteractionTestAccess::SetRequiredInteractorTags(Interactable, RequiredTags);

	// Mismatched tag first.
	IMounteaInteractorInterface::Execute_SetInteractorTag(Interactor, TAG_MounteaTest_InteractorB.GetTag());
	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Interactable));

	TestNull(TEXT("A tag-mismatched interactable must never win focus"),
		IMounteaInteractorInterface::Execute_GetFocusedActor(Interactor));

	// Matching tag should let it through once focus is re-evaluated.
	IMounteaInteractorInterface::Execute_SetInteractorTag(Interactor, TAG_MounteaTest_InteractorA.GetTag());
	FMounteaInteractionTestAccess::RefreshFocus(Interactor);

	TestEqual(TEXT("A tag-matched interactable should win focus once compatible"),
		IMounteaInteractorInterface::Execute_GetFocusedActor(Interactor), InteractableOwner);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// End-to-end regression test for Phase 1's core pipeline fix, specifically the Mash guard
// exception in StartInteraction_Implementation: drives the REAL flow (RefreshFocus -> focus ->
// StartInteraction -> RequestInteractorJoin -> InteractionStarted, then repeat locked
// StartInteraction calls forwarding straight to InteractionStarted) rather than injecting state
// directly, and verifies ActualMashAmount / GetInteractionProgress track real repeated presses.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionMashProgressEndToEndTest,
	"Mountea.InteractionSystem.World.Pipeline.MashProgressTracksRepeatedPresses",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionMashProgressEndToEndTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionWorldTest;

	FTestWorldWrapper WorldWrapper;
	if (!WorldWrapper.CreateTestWorld(EWorldType::Game))
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}
	UWorld* World = WorldWrapper.GetTestWorld();
	if (!World)
	{
		AddError(TEXT("Test world is null"));
		return false;
	}
	WorldWrapper.BeginPlayInTestWorld();

	AActor* InteractableOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	AActor* InteractorOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!InteractableOwner || !InteractorOwner)
	{
		AddError(TEXT("Failed to spawn test actors"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractableComponentMash* Mash = AddInteractableComponent<UMounteaInteractableComponentMash>(InteractableOwner);
	Mash->SetMinMashAmountRequired(3);

	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);

	// Detection: focuses the Mash interactable and populates ActionList (Tier 2) from it.
	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Mash));

	TestEqual(TEXT("Precondition: Mash interactable should be focused after detection"),
		IMounteaInteractorInterface::Execute_GetFocusedActor(Interactor), InteractableOwner);

	// First press: joins the slot and locks the selection (StartInteraction_Implementation's
	// not-yet-locked branch -> RequestInteractorJoin -> InteractionStarted).
	IMounteaInteractorInterface::Execute_StartInteraction(Interactor, 0.f);

	TestEqual(TEXT("First press should register one mash count"), Mash->GetActualMashAmount(), 1);

	// Two more repeat presses: locked branch forwards straight to InteractionStarted per press.
	IMounteaInteractorInterface::Execute_StartInteraction(Interactor, 0.1f);
	IMounteaInteractorInterface::Execute_StartInteraction(Interactor, 0.2f);

	TestEqual(TEXT("Three presses should reach MinMashAmountRequired"), Mash->GetActualMashAmount(), 3);
	TestEqual(TEXT("GetInteractionProgress should read 1.0 once MinMashAmountRequired is reached"),
		IMounteaInteractableInterface::Execute_GetInteractionProgress(Mash), 1.f);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Regression test for Phase 6: TriggerCooldown/PauseInteraction both early-out without a real,
// authoritative owner (GetOwner()->HasAuthority()), so bare NewObject() components can never
// exercise them — needs the world harness. Verifies the state transitions themselves; the timers
// they schedule (auto-resume/auto-wake) aren't ticked here, that's a separate concern.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionCooldownAndPauseStateTest,
	"Mountea.InteractionSystem.World.State.TriggerCooldownAndPauseTransitionState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionCooldownAndPauseStateTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionWorldTest;

	FTestWorldWrapper WorldWrapper;
	if (!WorldWrapper.CreateTestWorld(EWorldType::Game))
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}
	UWorld* World = WorldWrapper.GetTestWorld();
	if (!World)
	{
		AddError(TEXT("Test world is null"));
		return false;
	}
	WorldWrapper.BeginPlayInTestWorld();

	AActor* InteractableOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!InteractableOwner)
	{
		AddError(TEXT("Failed to spawn test actor"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractableComponentPress* Interactable = AddInteractableComponent<UMounteaInteractableComponentPress>(InteractableOwner);

	TestEqual(TEXT("Precondition: interactable should boot Awake"),
		IMounteaInteractableInterface::Execute_GetState(Interactable), EInteractableStateV3::EIS_Awake);

	const bool bTriggered = IMounteaInteractableInterface::Execute_TriggerCooldown(Interactable);
	TestTrue(TEXT("TriggerCooldown should succeed with a real, authoritative owner"), bTriggered);
	TestEqual(TEXT("TriggerCooldown should transition state to Cooldown"),
		IMounteaInteractableInterface::Execute_GetState(Interactable), EInteractableStateV3::EIS_Cooldown);

	FString Err;
	IMounteaInteractableInterface::Execute_WakeUpInteractable(Interactable, Err);

	const TScriptInterface<IMounteaInteractorInterface> NoInteractor;
	IMounteaInteractableInterface::Execute_PauseInteraction(Interactable, 5.f, NoInteractor);
	TestEqual(TEXT("PauseInteraction should transition state to Paused"),
		IMounteaInteractableInterface::Execute_GetState(Interactable), EInteractableStateV3::EIS_Paused);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
