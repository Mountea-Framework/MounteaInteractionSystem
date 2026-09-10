// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Tests/AutomationCommon.h"
#include "Tests/MounteaInteractionTestAccess.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/Interactable/MounteaInteractableComponentPress.h"
#include "Components/Interactable/MounteaInteractableComponentAutomatic.h"
#include "Components/Interactable/MounteaInteractableComponentHold.h"
#include "Components/Interactable/MounteaInteractableComponentMash.h"
#include "Components/Interactor/MounteaInteractorComponentOverlap.h"
#include "Helpers/MounteaInteractableStatics.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Interfaces/MounteaInteractorInterface.h"

// Full-pipeline "does a real interaction actually start and complete" scenarios, driven end-to-end
// (RefreshFocus -> StartInteraction -> RequestInteractorJoin -> InteractionStarted ->
// CompleteInteractable) rather than calling individual hooks directly, plus the mirror-image
// "actor destroyed mid-interaction" scenarios. These caught three previously-undetected production
// bugs in the core pipeline (not just gaps in test coverage) — see the comment on each failing test
// below for specifics. Helpers reused from MounteaInteractionWorldTest.cpp's own namespace pattern.

namespace MounteaInteractionGameplayScenarioTest
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

// Regression test — exposed a production bug: nothing in the real join/InteractionStarted pipeline
// ever transitions InteractableState to EIS_Active (only Hover's cursor-driven path calls
// ActivateInteractable). CompleteInteractable_Implementation early-outs with "Not active" whenever
// InteractableState != EIS_Active, so a Press interactable driven through the real
// StartInteraction -> RequestInteractorJoin -> InteractionStarted -> CompleteInteractable chain
// never actually completes — state stays stuck at Awake forever, even though
// MounteaInteractableComponentPress.cpp's own InteractionStarted_Implementation does call
// CompleteInteractable immediately. Affects Press/Automatic/Hold/Mash identically; only Hover is
// unaffected (it calls Execute_ActivateInteractable itself before completing).
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionPressCompletesLifecycleTest,
	"Mountea.InteractionSystem.World.Pipeline.PressCompletesToLifecycleEnd",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionPressCompletesLifecycleTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionGameplayScenarioTest;

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
	IMounteaInteractableInterface::Execute_SetLifecycleMode(Interactable, EInteractableLifecycle::EIL_OnlyOnce);

	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);

	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Interactable));

	TestEqual(TEXT("Precondition: Press interactable should be focused after detection"),
		IMounteaInteractorInterface::Execute_GetFocusedActor(Interactor), InteractableOwner);

	// Real full pipeline: locks selection -> RequestInteractorJoin -> InteractionStarted ->
	// (Press-specific) CompleteInteractable, all in one call.
	IMounteaInteractorInterface::Execute_StartInteraction(Interactor, 0.f);

	TestEqual(TEXT("A completed once-only Press interaction must end in EIS_Completed"),
		IMounteaInteractableInterface::Execute_GetState(Interactable), EInteractableStateV3::EIS_Completed);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Same underlying bug as above, exercised through Automatic's timer-driven completion path instead
// of Press's immediate one — confirms the bug isn't Press-specific.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionAutomaticCompletesAfterTimerTest,
	"Mountea.InteractionSystem.World.Pipeline.AutomaticCompletesAfterTimer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionAutomaticCompletesAfterTimerTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionGameplayScenarioTest;

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

	UMounteaInteractableComponentAutomatic* Interactable = AddInteractableComponent<UMounteaInteractableComponentAutomatic>(InteractableOwner);
	IMounteaInteractableInterface::Execute_SetLifecycleMode(Interactable, EInteractableLifecycle::EIL_OnlyOnce);
	IMounteaInteractableInterface::Execute_SetInteractionPeriod(Interactable, 0.2f);

	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);
	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Interactable));

	IMounteaInteractorInterface::Execute_StartInteraction(Interactor, 0.f);

	// Join happened, but the completion timer hasn't fired yet — should not be Completed yet.
	TestNotEqual(TEXT("Precondition: Automatic must not complete before its timer fires"),
		IMounteaInteractableInterface::Execute_GetState(Interactable), EInteractableStateV3::EIS_Completed);

	// Several smaller ticks rather than one large one — FTimerManager::Tick() in this harness
	// reliably fires a due timer across incremental ticks; a single tick whose DeltaTime alone
	// exceeds the timer's remaining time was observed to not fire it (test-harness ordering
	// quirk, not a gameplay issue — real gameplay never single-steps a 0.3s frame).
	for (int32 i = 0; i < 4; ++i)
	{
		WorldWrapper.TickTestWorld(0.1f);
	}

	TestEqual(TEXT("A completed once-only Automatic interaction must end in EIS_Completed once its timer fires"),
		IMounteaInteractableInterface::Execute_GetState(Interactable), EInteractableStateV3::EIS_Completed);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// GetInteractionProgress() itself (server-time delta / InteractionPeriod) is independent of the
// EIS_Active bug above, so this half is expected to pass — isolates that the timing math is sound
// even while completion itself is broken. The completion assertion at the end shares the same bug.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionHoldProgressAndCompletionTest,
	"Mountea.InteractionSystem.World.Pipeline.HoldProgressTracksElapsedThenCompletes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionHoldProgressAndCompletionTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionGameplayScenarioTest;

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

	UMounteaInteractableComponentHold* Interactable = AddInteractableComponent<UMounteaInteractableComponentHold>(InteractableOwner);
	IMounteaInteractableInterface::Execute_SetLifecycleMode(Interactable, EInteractableLifecycle::EIL_OnlyOnce);
	IMounteaInteractableInterface::Execute_SetInteractionPeriod(Interactable, 1.f);

	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);
	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Interactable));

	IMounteaInteractorInterface::Execute_StartInteraction(Interactor, 0.f);

	// Incremental ticks — see the comment in AutomaticCompletesAfterTimer for why a single large
	// tick isn't reliable for firing a due FTimerManager timer in this harness.
	for (int32 i = 0; i < 5; ++i)
	{
		WorldWrapper.TickTestWorld(0.1f);
	}

	const float MidProgress = IMounteaInteractableInterface::Execute_GetInteractionProgress(Interactable);
	TestTrue(TEXT("Progress halfway through a 1s Hold should be roughly 0.5"),
		MidProgress > 0.3f && MidProgress < 0.7f);

	for (int32 i = 0; i < 6; ++i)
	{
		WorldWrapper.TickTestWorld(0.1f);
	}

	TestEqual(TEXT("A completed once-only Hold interaction must end in EIS_Completed once its timer fires"),
		IMounteaInteractableInterface::Execute_GetState(Interactable), EInteractableStateV3::EIS_Completed);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Regression test — exposed a second production bug, independent of the EIS_Active one above:
// StopInteraction_Implementation (MounteaInteractorComponentBase.cpp) only ever calls
// RequestInteractorLeave on the interactable — it never calls Execute_InteractionStopped. Grepping
// the whole module confirms Execute_InteractionStopped has exactly one call site anywhere
// (MounteaInteractableComponentHover.cpp's cursor-exit handler) — the join/leave/EndPlay pipeline
// used by Press/Automatic/Hold/Mash never reaches it. Concretely: releasing the interaction key
// mid-Hold never clears InteractionStartServerTime (that reset lives in the base
// InteractionStopped_Implementation) and never clears Timer_Hold (that lives in Hold's own
// InteractionStopped_Implementation override) — so a released Hold silently keeps ticking toward
// completion in the background. This test only checks the InteractionStartServerTime half, since
// it's observable through the public interface (GetInteractionProgress) without needing new
// friend-access into Hold's protected Timer_Hold.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionHoldReleaseResetsProgressTest,
	"Mountea.InteractionSystem.World.Pipeline.HoldReleaseBeforeCompletionResetsProgress",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionHoldReleaseResetsProgressTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionGameplayScenarioTest;

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

	UMounteaInteractableComponentHold* Interactable = AddInteractableComponent<UMounteaInteractableComponentHold>(InteractableOwner);
	IMounteaInteractableInterface::Execute_SetInteractionPeriod(Interactable, 10.f);

	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);
	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Interactable));

	IMounteaInteractorInterface::Execute_StartInteraction(Interactor, 0.f);
	WorldWrapper.TickTestWorld(2.f);

	TestTrue(TEXT("Precondition: progress should be nonzero partway through a held interaction"),
		IMounteaInteractableInterface::Execute_GetInteractionProgress(Interactable) > 0.f);

	// Player releases the interaction key before the hold completes.
	IMounteaInteractorInterface::Execute_StopInteraction(Interactor, 2.f);

	TestEqual(TEXT("Releasing before completion must drop the interactor's slot"),
		UMounteaInteractableStatics::GetInteractorSlotCount(ToInteractableScript(Interactable)), 0);

	TestEqual(TEXT("Releasing before completion must reset progress back to zero (InteractionStopped should fire)"),
		IMounteaInteractableInterface::Execute_GetInteractionProgress(Interactable), 0.f);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Regression test — exposed a fourth production issue: StartInteraction_Implementation sets
// bSelectionLocked = true optimistically, client-side, BEFORE the join is confirmed. When a second
// interactor's join is rejected (slot full — MaxInteractors=1, first interactor already joined),
// nothing listens to OnInteractorRejected to undo that local lock. The rejected interactor is left
// permanently "locked" onto an interactable it was never actually granted a slot on.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionRejectedJoinDoesNotLockInteractorTest,
	"Mountea.InteractionSystem.World.Pipeline.RejectedJoinDoesNotLockInteractor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionRejectedJoinDoesNotLockInteractorTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionGameplayScenarioTest;

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
	AActor* FirstInteractorOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	AActor* SecondInteractorOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!InteractableOwner || !FirstInteractorOwner || !SecondInteractorOwner)
	{
		AddError(TEXT("Failed to spawn test actors"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractableComponentHold* Interactable = AddInteractableComponent<UMounteaInteractableComponentHold>(InteractableOwner);
	IMounteaInteractableInterface::Execute_SetInteractionPeriod(Interactable, 10.f);
	// MaxInteractors defaults to 1, which is exactly what this test needs.

	UMounteaInteractorComponentOverlap* FirstInteractor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(FirstInteractorOwner);
	UMounteaInteractorComponentOverlap* SecondInteractor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(SecondInteractorOwner);

	FMounteaInteractionTestAccess::AddDetected(FirstInteractor, ToInteractableScript(Interactable));
	FMounteaInteractionTestAccess::AddDetected(SecondInteractor, ToInteractableScript(Interactable));

	IMounteaInteractorInterface::Execute_StartInteraction(FirstInteractor, 0.f);
	TestEqual(TEXT("Precondition: first interactor should hold the only available slot"),
		UMounteaInteractableStatics::GetInteractorSlotCount(ToInteractableScript(Interactable)), 1);

	// Second interactor's join must be rejected — the slot is already taken.
	IMounteaInteractorInterface::Execute_StartInteraction(SecondInteractor, 0.f);

	TestEqual(TEXT("A rejected second join must not add a slot"),
		UMounteaInteractableStatics::GetInteractorSlotCount(ToInteractableScript(Interactable)), 1);

	TestFalse(TEXT("A rejected interactor must not stay locked onto the interactable it failed to join"),
		IMounteaInteractorInterface::Execute_IsSelectionLocked(SecondInteractor));

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Confirms the current, undocumented-but-expected behavior for an actor hosting several different
// interactable types at once (e.g. a car: Hold to enter as driver, Mash to honk the horn,
// Automatic to auto-unlock on approach): all of them appear as separate Tier-2 action-list entries,
// and starting one has zero effect on its siblings' state or slots — each component is fully
// independent. There is currently NO actor-level "one interaction blocks the others" rule; that is
// a separate, not-yet-implemented feature (see MounteaInteractionSecondInteractorJoinsSiblingWhileFirstActiveTest
// below, which documents that gap explicitly).
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionMixedInteractableTypesIndependentTest,
	"Mountea.InteractionSystem.World.Pipeline.MixedInteractableTypesOnSameActorAreIndependent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionMixedInteractableTypesIndependentTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionGameplayScenarioTest;

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

	UMounteaInteractableComponentAutomatic* Automatic = AddInteractableComponent<UMounteaInteractableComponentAutomatic>(InteractableOwner);
	IMounteaInteractableInterface::Execute_SetInteractionPeriod(Automatic, 10.f); // long enough not to fire mid-test
	UMounteaInteractableComponentHold* Hold = AddInteractableComponent<UMounteaInteractableComponentHold>(InteractableOwner);
	IMounteaInteractableInterface::Execute_SetInteractionPeriod(Hold, 10.f);
	UMounteaInteractableComponentMash* Mash = AddInteractableComponent<UMounteaInteractableComponentMash>(InteractableOwner);
	Mash->SetMinMashAmountRequired(10);

	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);

	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Automatic));
	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Hold));
	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Mash));

	const TArray<FInteractionActionCandidate> ActionList = IMounteaInteractorInterface::Execute_GetActionList(Interactor);
	TestEqual(TEXT("All three interactable types on the same actor must appear as separate action-list entries"),
		ActionList.Num(), 3);

	const int32 HoldIndex = ActionList.IndexOfByPredicate([Hold](const FInteractionActionCandidate& Candidate)
	{
		return Candidate.Interactable.GetObject() == Hold;
	});
	if (HoldIndex == INDEX_NONE)
	{
		AddError(TEXT("Precondition: Hold must be present in the action list"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	// Select and start only the Hold entry.
	FMounteaInteractionTestAccess::SetHighlightedActionIndex(Interactor, HoldIndex);
	IMounteaInteractorInterface::Execute_StartInteraction(Interactor, 0.f);

	TestEqual(TEXT("Starting Hold must make it — and only it — EIS_Active"),
		IMounteaInteractableInterface::Execute_GetState(Hold), EInteractableStateV3::EIS_Active);
	TestEqual(TEXT("Sibling Automatic on the same actor must be untouched by Hold starting"),
		IMounteaInteractableInterface::Execute_GetState(Automatic), EInteractableStateV3::EIS_Awake);
	TestEqual(TEXT("Sibling Mash on the same actor must be untouched by Hold starting"),
		IMounteaInteractableInterface::Execute_GetState(Mash), EInteractableStateV3::EIS_Awake);
	TestEqual(TEXT("Sibling Automatic must have zero interactor slots — no cross-component join happened"),
		UMounteaInteractableStatics::GetInteractorSlotCount(ToInteractableScript(Automatic)), 0);
	TestEqual(TEXT("Sibling Mash must have zero interactor slots — no cross-component join happened"),
		UMounteaInteractableStatics::GetInteractorSlotCount(ToInteractableScript(Mash)), 0);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Regression test for the actor-level "blocked by" rule (IsBlockedBySiblingInteractable, called
// from RequestInteractorJoin_Implementation): a SECOND interactor must NOT be able to join and
// start a sibling interactable on the same actor while a FIRST interactor is already mid-
// interaction (EIS_Active) with a different component there. The hard rule: any Active sibling
// blocks joins to every other interactable on that actor until it finishes/cancels. Siblings stay
// selectable in Tier 2 the whole time — only the actual join is rejected, same as a full slot.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionSecondInteractorJoinsSiblingWhileFirstActiveTest,
	"Mountea.InteractionSystem.World.Pipeline.ActiveSiblingBlocksJoinOnOtherInteractable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionSecondInteractorJoinsSiblingWhileFirstActiveTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionGameplayScenarioTest;

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
	AActor* FirstInteractorOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	AActor* SecondInteractorOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!InteractableOwner || !FirstInteractorOwner || !SecondInteractorOwner)
	{
		AddError(TEXT("Failed to spawn test actors"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractableComponentHold* Hold = AddInteractableComponent<UMounteaInteractableComponentHold>(InteractableOwner);
	IMounteaInteractableInterface::Execute_SetInteractionPeriod(Hold, 10.f);
	UMounteaInteractableComponentMash* Mash = AddInteractableComponent<UMounteaInteractableComponentMash>(InteractableOwner);
	Mash->SetMinMashAmountRequired(10);

	UMounteaInteractorComponentOverlap* FirstInteractor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(FirstInteractorOwner);
	UMounteaInteractorComponentOverlap* SecondInteractor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(SecondInteractorOwner);

	// Detecting ONE sibling still focuses the whole actor, and Tier 2's RefreshFocus() rebuilds the
	// action list from EVERY IMounteaInteractableInterface component on the focused actor (not just
	// what was AddDetected) — so both interactors see [Hold, Mash] regardless of which one they were
	// told about. Explicitly select the intended entry by index rather than assuming index 0.
	FMounteaInteractionTestAccess::AddDetected(FirstInteractor, ToInteractableScript(Hold));
	int32 firstHoldIndex = IMounteaInteractorInterface::Execute_GetActionList(FirstInteractor).IndexOfByPredicate(
		[Hold](const FInteractionActionCandidate& Candidate) { return Candidate.Interactable.GetObject() == Hold; });
	FMounteaInteractionTestAccess::SetHighlightedActionIndex(FirstInteractor, firstHoldIndex);
	IMounteaInteractorInterface::Execute_StartInteraction(FirstInteractor, 0.f);

	TestEqual(TEXT("Precondition: Hold should be Active after the first interactor starts it"),
		IMounteaInteractableInterface::Execute_GetState(Hold), EInteractableStateV3::EIS_Active);

	// Second interactor targets the sibling Mash component on the SAME actor — must be rejected
	// because Hold is currently EIS_Active on a sibling.
	FMounteaInteractionTestAccess::AddDetected(SecondInteractor, ToInteractableScript(Mash));
	const int32 secondMashIndex = IMounteaInteractorInterface::Execute_GetActionList(SecondInteractor).IndexOfByPredicate(
		[Mash](const FInteractionActionCandidate& Candidate) { return Candidate.Interactable.GetObject() == Mash; });
	FMounteaInteractionTestAccess::SetHighlightedActionIndex(SecondInteractor, secondMashIndex);
	IMounteaInteractorInterface::Execute_StartInteraction(SecondInteractor, 0.f);

	TestFalse(TEXT("A second interactor must NOT stay locked onto a sibling interactable rejected due to actor-level blocking"),
		IMounteaInteractorInterface::Execute_IsSelectionLocked(SecondInteractor));
	TestEqual(TEXT("The blocked sibling's slot count must remain zero — the join must never have gone through"),
		UMounteaInteractableStatics::GetInteractorSlotCount(ToInteractableScript(Mash)), 0);

	// Once Hold finishes (stopped/canceled), the sibling should be joinable again.
	IMounteaInteractorInterface::Execute_StopInteraction(FirstInteractor, 1.f);
	TestNotEqual(TEXT("Precondition: Hold must no longer be Active after being stopped"),
		IMounteaInteractableInterface::Execute_GetState(Hold), EInteractableStateV3::EIS_Active);

	IMounteaInteractorInterface::Execute_StartInteraction(SecondInteractor, 1.f);
	TestTrue(TEXT("Once the blocking sibling is no longer Active, the second interactor must be able to join"),
		IMounteaInteractorInterface::Execute_IsSelectionLocked(SecondInteractor));
	TestEqual(TEXT("The previously-blocked sibling should now hold one slot"),
		UMounteaInteractableStatics::GetInteractorSlotCount(ToInteractableScript(Mash)), 1);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
