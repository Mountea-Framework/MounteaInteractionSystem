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
		// See MounteaInteractionAccessControlTest.cpp's copy of this helper for why the attach is
		// required: an unattached USceneComponent registers at the world origin, not the owner's
		// actual SetActorLocation(). Guarded since interactor components (UActorComponent) have no
		// attachment concept at all.
		if constexpr (TIsDerivedFrom<TComponent, USceneComponent>::IsDerived)
		{
			Comp->SetupAttachment(Owner->GetRootComponent());
		}
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

	// Deliberately NOT collinear with the interactor (e.g. both on +X) — since Overlap interactors
	// now run a real safety-trace line check by default (see UMounteaInteractorComponentOverlap's
	// constructor), a Near actor sitting directly between the interactor and a Far actor would
	// legitimately obstruct the trace to Far with its own widget collision. That's correct
	// behavior for the safety trace, but it's not what THIS test is about — this test is purely
	// about the distance tie-break, so Near and Far sit on separate axes with a clear line to each.
	AActor* NearActor = SpawnActorWithRoot(World, FVector(100.f, 0.f, 0.f));
	AActor* FarActor = SpawnActorWithRoot(World, FVector(0.f, 1000.f, 0.f));
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

// Regression test for a late-join replication bug: BeginPlay() used to unconditionally reset
// InteractableState/RemainingLifecycleCount to their design-time defaults, with no authority check.
// On a real remote client (a late joiner is the clearest case), the true current value has usually
// already arrived over the wire by the time BeginPlay() runs locally — so that reset silently
// discarded it, showing e.g. a mid-Hold door as fresh/Awake instead of its real in-progress state.
// A real second network connection is out of reach for this harness (FTestWorldWrapper has no
// NetDriver), so this simulates the essential shape of the bug directly: force the actor's Role to
// a non-authority role, pre-seed InteractableState/RemainingLifecycleCount as "already replicated"
// (bypassing BeginPlay entirely), then trigger a real BeginPlay() and confirm those values survive
// instead of being reset.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionNonAuthorityBeginPlayPreservesReplicatedStateTest,
	"Mountea.InteractionSystem.World.Replication.NonAuthorityBeginPlayPreservesInteractableState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionNonAuthorityBeginPlayPreservesReplicatedStateTest::RunTest(const FString& Parameters)
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

	// Simulate "this is a remote client's view of an actor owned/driven elsewhere" — the same
	// condition a late-joining client's copy of an in-progress interactable would be in.
	InteractableOwner->SetRole(ROLE_SimulatedProxy);

	UMounteaInteractableComponentPress* Interactable = NewObject<UMounteaInteractableComponentPress>(InteractableOwner, UMounteaInteractableComponentPress::StaticClass());
	Interactable->SetupAttachment(InteractableOwner->GetRootComponent());

	// Pre-seed as "already replicated" BEFORE registration triggers BeginPlay — DefaultInteractableState
	// is EIS_Awake and LifecycleCount defaults away from 2, so EIS_Active/2 are both unambiguously
	// NOT what a fresh BeginPlay() would produce if it (incorrectly) reset them.
	FMounteaInteractionTestAccess::SetReplicatedStateForTest(Interactable, EInteractableStateV3::EIS_Active, 2);

	// The world already began play, so registering now drives a real BeginPlay() for this component
	// (HandleRegisterComponentWithWorld), exactly like a component added to an already-running world.
	InteractableOwner->AddInstanceComponent(Interactable);
	Interactable->RegisterComponent();

	TestEqual(TEXT("A non-authority BeginPlay() must not reset an already-replicated InteractableState back to its default"),
		IMounteaInteractableInterface::Execute_GetState(Interactable), EInteractableStateV3::EIS_Active);
	TestEqual(TEXT("A non-authority BeginPlay() must not reset an already-replicated RemainingLifecycleCount back to LifecycleCount"),
		IMounteaInteractableInterface::Execute_GetRemainingLifecycleCount(Interactable), 2);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Same fix, interactor side: a non-authority BeginPlay() must not reset an already-replicated
// InteractorState back to DefaultInteractorState (e.g. a late joiner seeing another player's
// interactor as freshly Scanning when the server has it Suppressed/Disabled for some gameplay
// reason).
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionNonAuthorityBeginPlayPreservesInteractorStateTest,
	"Mountea.InteractionSystem.World.Replication.NonAuthorityBeginPlayPreservesInteractorState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionNonAuthorityBeginPlayPreservesInteractorStateTest::RunTest(const FString& Parameters)
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

	AActor* InteractorOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!InteractorOwner)
	{
		AddError(TEXT("Failed to spawn test actor"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	InteractorOwner->SetRole(ROLE_SimulatedProxy);

	UMounteaInteractorComponentOverlap* Interactor = NewObject<UMounteaInteractorComponentOverlap>(InteractorOwner, UMounteaInteractorComponentOverlap::StaticClass());

	// DefaultInteractorState is EIS_Scanning — EIS_Suppressed is unambiguously not what a fresh
	// BeginPlay() would produce if it (incorrectly) reset InteractorState.
	FMounteaInteractionTestAccess::SetReplicatedStateForTest(Interactor, EInteractorStateV3::EIS_Suppressed);

	InteractorOwner->AddInstanceComponent(Interactor);
	Interactor->RegisterComponent();

	TestEqual(TEXT("A non-authority BeginPlay() must not reset an already-replicated InteractorState back to its default"),
		IMounteaInteractorInterface::Execute_GetState(Interactor), EInteractorStateV3::EIS_Suppressed);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// The two tests above prove a non-authority BeginPlay() doesn't stomp replicated state. This one
// proves the other half: the authority-side init those tests deliberately bypass (SetReplicatedStateForTest
// sets raw members before BeginPlay() ever runs) actually fires correctly with ZERO PlayerController,
// Pawn, or possession anywhere in the world — i.e. it works identically for an NPC as for a player
// pawn. HasAuthority() is a property of the actor's own ENetRole, not of player possession, and
// BeginPlay() runs automatically for every actor the moment it begins play — this is not something
// that needs a GameMode/GameState/Subsystem to broker.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionAuthorityBeginPlayInitializesWithoutPlayerControllerTest,
	"Mountea.InteractionSystem.World.Replication.AuthorityBeginPlayInitializesWithoutPlayerController",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionAuthorityBeginPlayInitializesWithoutPlayerControllerTest::RunTest(const FString& Parameters)
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

	TestNull(TEXT("No PlayerController should exist in this world — this simulates an NPC actor never possessed by any player"),
		World->GetFirstPlayerController());

	AActor* NpcActor = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!NpcActor)
	{
		AddError(TEXT("Failed to spawn test actor"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}
	// No SetRole() call — a locally spawned actor with no owning connection defaults to
	// ROLE_Authority, exactly like a level-placed or server-spawned NPC.

	UMounteaInteractableComponentPress* Interactable = NewObject<UMounteaInteractableComponentPress>(NpcActor, UMounteaInteractableComponentPress::StaticClass());
	Interactable->SetupAttachment(NpcActor->GetRootComponent());
	// Distinctive, non-default value: proves BeginPlay()'s RemainingLifecycleCount = LifecycleCount
	// assignment actually ran, rather than coincidentally matching a default.
	FMounteaInteractionTestAccess::SetLifecycleCount(Interactable, 7);

	UMounteaInteractorComponentOverlap* Interactor = NewObject<UMounteaInteractorComponentOverlap>(NpcActor, UMounteaInteractorComponentOverlap::StaticClass());
	// EIS_Disabled is not UMounteaInteractorComponentBase's own hardcoded default (EIS_Scanning) —
	// same "distinctive value" reasoning.
	FMounteaInteractionTestAccess::SetDefaultInteractorState(Interactor, EInteractorStateV3::EIS_Disabled);

	// Registering now (world already began play) drives a real BeginPlay() for both components —
	// no PlayerController, Pawn, or possession involved anywhere in this call chain.
	NpcActor->AddInstanceComponent(Interactable);
	Interactable->RegisterComponent();
	NpcActor->AddInstanceComponent(Interactor);
	Interactor->RegisterComponent();

	TestEqual(TEXT("Authority BeginPlay() must initialize RemainingLifecycleCount from LifecycleCount with no PlayerController involved"),
		IMounteaInteractableInterface::Execute_GetRemainingLifecycleCount(Interactable), 7);
	TestEqual(TEXT("Authority BeginPlay() must initialize InteractorState from DefaultInteractorState with no PlayerController involved"),
		IMounteaInteractorInterface::Execute_GetState(Interactor), EInteractorStateV3::EIS_Disabled);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
