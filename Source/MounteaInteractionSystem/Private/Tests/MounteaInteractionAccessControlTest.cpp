// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Tests/AutomationCommon.h"
#include "Tests/MounteaInteractionTestAccess.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "NativeGameplayTags.h"
#include "Components/Interactable/MounteaInteractableComponentPress.h"
#include "Components/Interactor/MounteaInteractorComponentOverlap.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Interfaces/MounteaInteractorInterface.h"

// Tests in this file cover "who is allowed to interact with what" — gameplay tag whitelist/
// blacklist gating, and the physical line-of-sight validation (PerformSafetyTrace) that an Overlap
// interactor now runs by default. Both mechanisms are checked at focus time (RefreshFocus), which
// is where they're actually wired — a rejected candidate never earns focus/an attention widget in
// the first place, rather than being focusable-but-rejected-on-join.

namespace MounteaInteractionAccessControlTest
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
		// Without an explicit attachment, a newly registered USceneComponent (UWidgetComponent
		// included) has no parent to inherit a transform from, so it registers at the world origin
		// regardless of where SetActorLocation() put the owning actor — invisible for tests that
		// only check actor identity, but fatal for anything (like a real physics trace) that cares
		// where the component's own collision actually sits. Interactor components (UActorComponent,
		// not scene components) have no attachment concept at all, hence the constexpr guard.
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

	// Spawns an actor with a solid UBoxComponent blocking every channel — PerformSafetyTrace
	// resolves its channel from the project's own collision-channel config (a custom
	// "InteractionTrace" channel if configured, ECC_Visibility as a hardcoded fallback otherwise),
	// so blocking everything keeps this deterministic regardless of that project-specific setup.
	static AActor* SpawnBlockingWall(UWorld* World, const FVector& Location, const FVector& BoxExtent)
	{
		AActor* Actor = World->SpawnActor<AActor>();
		if (!Actor) return nullptr;

		UBoxComponent* Box = NewObject<UBoxComponent>(Actor);
		Actor->SetRootComponent(Box);
		Box->SetBoxExtent(BoxExtent);
		Box->RegisterComponent();
		Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Box->SetCollisionResponseToAllChannels(ECR_Block);
		Actor->SetActorLocation(Location);
		return Actor;
	}
}

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_AccessTest_NPC, "Mountea.Test.NPC");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_AccessTest_Player, "Mountea.Test.Player");

// Regression test for the excluded-tags blacklist gate: an interactor carrying an excluded tag
// must never win focus on an otherwise-reachable interactable, even though nothing about
// RequiredInteractorTags rejects it (whitelist empty = accept all). This is the "NPC cannot use
// this, but a Player can" / "Team B cannot interact with Team A's vehicle" scenario.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionExcludedTagBlocksFocusTest,
	"Mountea.InteractionSystem.World.AccessControl.ExcludedTagBlocksFocus",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionExcludedTagBlocksFocusTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionAccessControlTest;

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
	AActor* NpcOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	AActor* PlayerOwner = SpawnActorWithRoot(World, FVector::ZeroVector);
	if (!InteractableOwner || !NpcOwner || !PlayerOwner)
	{
		AddError(TEXT("Failed to spawn test actors"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractableComponentPress* Interactable = AddInteractableComponent<UMounteaInteractableComponentPress>(InteractableOwner);

	FGameplayTagContainer ExcludedTags;
	ExcludedTags.AddTag(TAG_AccessTest_NPC.GetTag());
	FMounteaInteractionTestAccess::SetExcludedInteractorTags(Interactable, ExcludedTags);

	UMounteaInteractorComponentOverlap* NpcInteractor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(NpcOwner);
	IMounteaInteractorInterface::Execute_SetInteractorTag(NpcInteractor, TAG_AccessTest_NPC.GetTag());
	FMounteaInteractionTestAccess::AddDetected(NpcInteractor, ToInteractableScript(Interactable));

	TestNull(TEXT("An interactor carrying an excluded tag must never win focus"),
		IMounteaInteractorInterface::Execute_GetFocusedActor(NpcInteractor));

	UMounteaInteractorComponentOverlap* PlayerInteractor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(PlayerOwner);
	IMounteaInteractorInterface::Execute_SetInteractorTag(PlayerInteractor, TAG_AccessTest_Player.GetTag());
	FMounteaInteractionTestAccess::AddDetected(PlayerInteractor, ToInteractableScript(Interactable));

	TestEqual(TEXT("An interactor without the excluded tag should win focus normally"),
		IMounteaInteractorInterface::Execute_GetFocusedActor(PlayerInteractor), InteractableOwner);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Regression test: RequiredInteractorTags (whitelist) and ExcludedInteractorTags (blacklist) are
// independent gates checked together — an interactor must pass BOTH. Matches the "Team A can use
// this, but only if not additionally flagged Disabled/Hacked" style of layered access rule.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionWhitelistAndBlacklistBothApplyTest,
	"Mountea.InteractionSystem.World.AccessControl.WhitelistAndBlacklistBothApply",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionWhitelistAndBlacklistBothApplyTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionAccessControlTest;

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

	// Whitelist: requires Player. Blacklist: excludes Player too (a "Player, but not right now"
	// override — e.g. a console flagged both PlayerOnly and Compromised/Hacked).
	FGameplayTagContainer RequiredTags;
	RequiredTags.AddTag(TAG_AccessTest_Player.GetTag());
	FMounteaInteractionTestAccess::SetRequiredInteractorTags(Interactable, RequiredTags);

	FGameplayTagContainer ExcludedTags;
	ExcludedTags.AddTag(TAG_AccessTest_Player.GetTag());
	FMounteaInteractionTestAccess::SetExcludedInteractorTags(Interactable, ExcludedTags);

	UMounteaInteractorComponentOverlap* PlayerInteractor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);
	IMounteaInteractorInterface::Execute_SetInteractorTag(PlayerInteractor, TAG_AccessTest_Player.GetTag());
	FMounteaInteractionTestAccess::AddDetected(PlayerInteractor, ToInteractableScript(Interactable));

	TestNull(TEXT("Passing the whitelist does not matter if the same interactor also matches the blacklist"),
		IMounteaInteractorInterface::Execute_GetFocusedActor(PlayerInteractor));

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Regression test for PerformSafetyTrace, now enabled by default on UMounteaInteractorComponentOverlap
// (previously ESTM_None — an Overlap interactor had zero line-of-sight validation of its own):
// an interactable detected through a wall (overlap volumes don't respect occlusion) must never win
// focus, because the line trace from the interactor to the candidate hits the wall first instead of
// the candidate itself.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionSafetyTraceBlocksThroughWallTest,
	"Mountea.InteractionSystem.World.AccessControl.SafetyTraceBlocksDetectionThroughWall",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionSafetyTraceBlocksThroughWallTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionAccessControlTest;

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
	AActor* InteractableOwner = SpawnActorWithRoot(World, FVector(500.f, 0.f, 0.f));
	// A wall spanning Y/Z, sitting directly between the interactor and the interactable on X.
	AActor* Wall = SpawnBlockingWall(World, FVector(250.f, 0.f, 0.f), FVector(10.f, 200.f, 200.f));
	if (!InteractorOwner || !InteractableOwner || !Wall)
	{
		AddError(TEXT("Failed to spawn test actors"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractableComponentPress* Interactable = AddInteractableComponent<UMounteaInteractableComponentPress>(InteractableOwner);
	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);

	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Interactable));

	TestNull(TEXT("An interactable detected through a wall (overlap has no occlusion) must not win focus once the safety trace is blocked"),
		IMounteaInteractorInterface::Execute_GetFocusedActor(Interactor));

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

// Companion test: the exact same setup but with a clear line (no wall) must focus normally — proves
// the safety trace isn't just always rejecting, and that PerformSafetyTrace correctly recognizes a
// hit on the candidate itself as "not blocked" once nothing else is in the way.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionSafetyTraceAllowsClearLineTest,
	"Mountea.InteractionSystem.World.AccessControl.SafetyTraceAllowsDetectionWithClearLine",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionSafetyTraceAllowsClearLineTest::RunTest(const FString& Parameters)
{
	using namespace MounteaInteractionAccessControlTest;

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
	AActor* InteractableOwner = SpawnActorWithRoot(World, FVector(500.f, 0.f, 0.f));
	// Same "wall", but off to the side — well clear of the line between interactor and interactable.
	AActor* Wall = SpawnBlockingWall(World, FVector(250.f, 2000.f, 0.f), FVector(10.f, 200.f, 200.f));
	if (!InteractorOwner || !InteractableOwner || !Wall)
	{
		AddError(TEXT("Failed to spawn test actors"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	UMounteaInteractableComponentPress* Interactable = AddInteractableComponent<UMounteaInteractableComponentPress>(InteractableOwner);
	UMounteaInteractorComponentOverlap* Interactor = AddInteractableComponent<UMounteaInteractorComponentOverlap>(InteractorOwner);

	FMounteaInteractionTestAccess::AddDetected(Interactor, ToInteractableScript(Interactable));

	TestEqual(TEXT("A detected interactable with a clear line to it must win focus normally"),
		IMounteaInteractorInterface::Execute_GetFocusedActor(Interactor), InteractableOwner);

	WorldWrapper.ForwardErrorMessages(this);
	WorldWrapper.DestroyTestWorld(true);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
