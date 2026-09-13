// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Components/Interactor/MounteaInteractorComponentBase.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Helpers/MounteaInteractionStatics.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Subsystems/MounteaInteractionWorldSubsystem.h"
#include "Helpers/MounteaInteractionSystemLog.h"
#include "Helpers/MounteaInteractionSystemSettings.h"
#include "Settings/MounteaInteractorRuntimeConfig.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "MounteaInteractorComponentBase"

UMounteaInteractorComponentBase::UMounteaInteractorComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UMounteaInteractorComponentBase::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Execute_SetDefaults(this);
		InteractorState = DefaultInteractorState;
	}
}

void UMounteaInteractorComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// A destroyed/disconnected interactor must release its slot - otherwise the interactable's
	// ActiveInteractorSlots keeps a phantom entry forever, soft-locking MaxInteractors=1 objects.
	if (bSelectionLocked && LockedAction.GetInterface())
	{
		TScriptInterface<IMounteaInteractorInterface> self;
		self.SetObject(this);
		self.SetInterface(this);
		IMounteaInteractableInterface::Execute_RequestInteractorLeave(LockedAction.GetObject(), self);
	}

	ClearDetected();
	Super::EndPlay(EndPlayReason);
}

void UMounteaInteractorComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInteractorComponentBase, InteractorState);
	DOREPLIFETIME(UMounteaInteractorComponentBase, CollisionChannel);
	DOREPLIFETIME(UMounteaInteractorComponentBase, DefaultInteractorState);
	DOREPLIFETIME(UMounteaInteractorComponentBase, InteractorTag);
	DOREPLIFETIME(UMounteaInteractorComponentBase, SafetyTraceSetup);
	DOREPLIFETIME(UMounteaInteractorComponentBase, ListOfIgnoredActors);
	DOREPLIFETIME(UMounteaInteractorComponentBase, InteractionDependencies);

	DOREPLIFETIME_CONDITION(UMounteaInteractorComponentBase, HighlightedActionIndex, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractorComponentBase, bSelectionLocked,       COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractorComponentBase, LockedAction,           COND_OwnerOnly);
}

#pragma region IMounteaInteractorInterface_FocusAPI

AActor* UMounteaInteractorComponentBase::GetFocusedActor_Implementation() const
{
	return FocusedActor.Get();
}

TArray<FInteractionActionCandidate> UMounteaInteractorComponentBase::GetActionList_Implementation() const
{
	return ActionList;
}

int32 UMounteaInteractorComponentBase::GetHighlightedActionIndex_Implementation() const
{
	return HighlightedActionIndex;
}

TScriptInterface<IMounteaInteractableInterface> UMounteaInteractorComponentBase::GetLockedAction_Implementation() const
{
	return LockedAction;
}

bool UMounteaInteractorComponentBase::IsSelectionLocked_Implementation() const
{
	return bSelectionLocked;
}

void UMounteaInteractorComponentBase::CycleAction_Implementation(const int32 Direction)
{
	if (bSelectionLocked || ActionList.IsEmpty()) return;

	// Apply and broadcast locally first for instant UI feedback - waiting on a server round-trip
	// for "next/previous" would feel laggy. The server mirrors this below and its own replicated
	// HighlightedActionIndex (COND_OwnerOnly) will correct this client if the two ever disagree.
	HighlightedActionIndex = (HighlightedActionIndex + Direction + ActionList.Num()) % ActionList.Num();
	OnHighlightedActionChanged.Broadcast(HighlightedActionIndex);
	Execute_OnHighlightedActionChangedEvent(this, HighlightedActionIndex);

	if (GetOwner() && !GetOwner()->HasAuthority())
		CycleAction_Server(Direction);
}

#pragma endregion

#pragma region IMounteaInteractorInterface_Interaction

void UMounteaInteractorComponentBase::StartInteraction_Implementation(const float StartTime)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		StartInteraction_Server(StartTime);
		return;
	}

	if (ActionList.IsEmpty()) return;

	TScriptInterface<IMounteaInteractorInterface> self;
	self.SetObject(this);
	self.SetInterface(this);

	if (bSelectionLocked)
	{
		// Guard exception: repeat presses on the already-locked action forward straight through
		// to InteractionStarted instead of re-joining. Required for Mash (each physical key press
		// is a new InteractionStarted call - see MounteaInteractableComponentMash.cpp's own
		// comment). Safe for the other types because their InteractionStarted_Implementation
		// overrides are idempotent while already active: Hold guards before calling Super:: (see
		// MounteaInteractableComponentHold.cpp), and Press/Automatic are naturally idempotent
		// since CompleteInteractable_Implementation early-outs once InteractableState != Active.
		if (LockedAction.GetInterface())
		{
			IMounteaInteractableInterface::Execute_InteractionStarted(LockedAction.GetObject(), StartTime, self);
			OnInteractionKeyPressed.Broadcast(StartTime);
			Execute_OnInteractionKeyPressedEvent(this, StartTime);
		}
		return;
	}

	if (!Execute_CanInteract(this)) return;

	const FInteractionActionCandidate& candidate = ActionList[FMath::Clamp(HighlightedActionIndex, 0, ActionList.Num() - 1)];
	if (!candidate.Interactable.GetInterface()) return;

	bSelectionLocked = true;
	LockedAction = candidate.Interactable;

	IMounteaInteractableInterface::Execute_RequestInteractorJoin(candidate.Interactable.GetObject(), self);

	OnInteractionKeyPressed.Broadcast(StartTime);
	Execute_OnInteractionKeyPressedEvent(this, StartTime);
}

void UMounteaInteractorComponentBase::StopInteraction_Implementation(const float StopTime)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		StopInteraction_Server(StopTime);
		return;
	}

	if (!bSelectionLocked) return;

	TScriptInterface<IMounteaInteractorInterface> self;
	self.SetObject(this);
	self.SetInterface(this);

	if (LockedAction.GetInterface())
	{
		IMounteaInteractableInterface::Execute_RequestInteractorLeave(LockedAction.GetObject(), self);
	}

	bSelectionLocked = false;
	LockedAction = nullptr;

	OnInteractionKeyReleased.Broadcast(StopTime);
	Execute_OnInteractionKeyReleasedEvent(this, StopTime);
}

void UMounteaInteractorComponentBase::ForceReleaseInteraction_Implementation()
{
	// Called by an interactable's EndPlay when it's destroyed while occupied. Clears local lock
	// state only - never calls back into the (mid-teardown) interactable, unlike StopInteraction.
	if (!bSelectionLocked) return;

	bSelectionLocked = false;
	LockedAction = nullptr;

	OnInteractionKeyReleased.Broadcast(-1.f);
	Execute_OnInteractionKeyReleasedEvent(this, -1.f);
}

bool UMounteaInteractorComponentBase::CanInteract_Implementation() const
{
	return InteractorState == EInteractorStateV3::EIS_Scanning || InteractorState == EInteractorStateV3::EIS_Active;
}

bool UMounteaInteractorComponentBase::IsValidInteractor_Implementation() const
{
	return Execute_CanInteract(this) && !DetectedSet.IsEmpty();
}

bool UMounteaInteractorComponentBase::HasInteractable_Implementation() const
{
	return FocusedActor.IsValid() && !ActionList.IsEmpty();
}

#pragma endregion

#pragma region IMounteaInteractorInterface_StateMachine

bool UMounteaInteractorComponentBase::ActivateInteractor_Implementation(FString& ErrorMessage)
{
	if (InteractorState == EInteractorStateV3::EIS_Active)
	{
		ErrorMessage = TEXT("Already active");
		return false;
	}
	if (InteractorState == EInteractorStateV3::EIS_Disabled)
	{
		ErrorMessage = TEXT("Cannot activate from Disabled state");
		return false;
	}

	Execute_SetState(this, EInteractorStateV3::EIS_Active);
	return true;
}

bool UMounteaInteractorComponentBase::EnableInteractor_Implementation(FString& ErrorMessage)
{
	if (InteractorState == EInteractorStateV3::EIS_Scanning)
	{
		ErrorMessage = TEXT("Already enabled");
		return false;
	}

	Execute_SetState(this, EInteractorStateV3::EIS_Scanning);
	return true;
}

bool UMounteaInteractorComponentBase::SuppressInteractor_Implementation(FString& ErrorMessage)
{
	if (InteractorState == EInteractorStateV3::EIS_Suppressed)
	{
		ErrorMessage = TEXT("Already suppressed");
		return false;
	}

	Execute_SetState(this, EInteractorStateV3::EIS_Suppressed);
	return true;
}

void UMounteaInteractorComponentBase::DeactivateInteractor_Implementation()
{
	Execute_SetState(this, EInteractorStateV3::EIS_Disabled);
}

EInteractorStateV3 UMounteaInteractorComponentBase::GetState_Implementation() const
{
	return InteractorState;
}

void UMounteaInteractorComponentBase::SetState_Implementation(const EInteractorStateV3 NewState)
{
	if (InteractorState == NewState) return;

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		SetState_Server(NewState);
		return;
	}

	InteractorState = NewState;
	ProcessStateChanged();
}

EInteractorStateV3 UMounteaInteractorComponentBase::GetDefaultState_Implementation() const
{
	return DefaultInteractorState;
}

void UMounteaInteractorComponentBase::SetDefaultState_Implementation(const EInteractorStateV3 NewState)
{
	DefaultInteractorState = NewState;
}

#pragma endregion

#pragma region IMounteaInteractorInterface_Properties

ECollisionChannel UMounteaInteractorComponentBase::GetResponseChannel_Implementation() const
{
	return CollisionChannel.GetValue();
}

void UMounteaInteractorComponentBase::SetResponseChannel_Implementation(const ECollisionChannel NewResponseChannel)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		SetResponseChannel_Server(NewResponseChannel);
		return;
	}
	CollisionChannel = NewResponseChannel;
	OnCollisionChanged.Broadcast(CollisionChannel.GetValue());
	Execute_OnInteractorCollisionChangedEvent(this, CollisionChannel);
}

FGameplayTag UMounteaInteractorComponentBase::GetInteractorTag_Implementation() const
{
	return InteractorTag;
}

void UMounteaInteractorComponentBase::SetInteractorTag_Implementation(const FGameplayTag& NewInteractorTag)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		SetInteractorTag_Server(NewInteractorTag);
		return;
	}
	InteractorTag = NewInteractorTag;
	OnInteractorTagChanged.Broadcast(NewInteractorTag);
	Execute_OnInteractorStateChangedEvent(this, InteractorState);
}

bool UMounteaInteractorComponentBase::DoesAutoActivate_Implementation() const
{
	return DefaultInteractorState == EInteractorStateV3::EIS_Active;
}

AActor* UMounteaInteractorComponentBase::GetOwningActor_Implementation() const
{
	return GetOwner();
}

FString UMounteaInteractorComponentBase::ToString_Implementation() const
{
	return FString::Printf(TEXT("Interactor: %s | State: %s | Detected: %d | Focused: %s"),
		*GetName(),
		*StaticEnum<EInteractorStateV3>()->GetNameStringByValue(static_cast<int64>(InteractorState)),
		DetectedSet.Num(),
		*GetNameSafe(FocusedActor.Get()));
}

void UMounteaInteractorComponentBase::SetDefaults_Implementation()
{
	const UMounteaInteractionSystemSettings* settings = GetDefault<UMounteaInteractionSystemSettings>();
	const UMounteaInteractorRuntimeConfig* config = settings ? settings->GetInteractorConfig() : nullptr;
	if (!config) return;

	DefaultInteractorState = config->DefaultInteractorState;
	InteractorTag = config->InteractorTag;
	SafetyTraceSetup = config->SafetyTracingSetup;
	MaxCandidatesPerActor = config->MaxCandidatesPerActor;

	bool bFound = false;
	const ECollisionChannel resolvedChannel = UMounteaInteractionStatics::ResolveInteractionChannel(config->OverlapCollisionChannelName, bFound);
	if (bFound)
		CollisionChannel = resolvedChannel;
}

#pragma endregion

#pragma region IMounteaInteractorInterface_SafetyTrace

FSafetyTracingSetup UMounteaInteractorComponentBase::GetSafetyTracingSetup_Implementation() const
{
	return SafetyTraceSetup;
}

void UMounteaInteractorComponentBase::SetSafetyTracingSetup_Implementation(const FSafetyTracingSetup& NewSafetyTracingSetup)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		SetSafetyTracingSetup_Server(NewSafetyTracingSetup);
		return;
	}
	SafetyTraceSetup = NewSafetyTracingSetup;
}

bool UMounteaInteractorComponentBase::PerformSafetyTrace_Implementation(const AActor* InteractableActor)
{
	if (!IsValid(InteractableActor) || SafetyTraceSetup.SafetyTracingMode == ESafetyTracingMode::ESTM_None)
		return true;
	if (!GetOwner() || !GetWorld()) 
		return true;

	FHitResult hitResult;
	const FVector start = GetOwner()->GetActorLocation();
	const FVector end = InteractableActor->GetActorLocation();

	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());

	const FName traceChannelName = SafetyTraceSetup.OverrideTraceChannelName.IsNone() ? FName("InteractionTrace") : SafetyTraceSetup.OverrideTraceChannelName;
	bool bTraceChannelFound = false;
	const ECollisionChannel traceChannel = UMounteaInteractionStatics::ResolveInteractionChannel(traceChannelName, bTraceChannelFound);
	const ECollisionChannel resolvedTraceChannel = bTraceChannelFound ? traceChannel : ECC_Visibility;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, resolvedTraceChannel, params);
	if (!bHit)
		return true;

	return hitResult.GetActor() == InteractableActor;
}

#pragma endregion

#pragma region IMounteaInteractorInterface_IgnoredActors

void UMounteaInteractorComponentBase::AddIgnoredActor_Implementation(AActor* IgnoredActor)
{
	if (!IsValid(IgnoredActor)) return;
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		AddIgnoredActor_Server(IgnoredActor);
		return;
	}
	ListOfIgnoredActors.AddUnique(IgnoredActor);
	OnIgnoredActorAdded.Broadcast(IgnoredActor);
}

void UMounteaInteractorComponentBase::AddIgnoredActors_Implementation(const TArray<AActor*>& IgnoredActors)
{
	for (AActor* a : IgnoredActors) Execute_AddIgnoredActor(this, a);
}

void UMounteaInteractorComponentBase::RemoveIgnoredActor_Implementation(AActor* UnignoredActor)
{
	if (!IsValid(UnignoredActor)) return;
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		RemoveIgnoredActor_Server(UnignoredActor);
		return;
	}
	ListOfIgnoredActors.Remove(UnignoredActor);
	OnIgnoredActorRemoved.Broadcast(UnignoredActor);
}

void UMounteaInteractorComponentBase::RemoveIgnoredActors_Implementation(const TArray<AActor*>& UnignoredActors)
{
	for (AActor* a : UnignoredActors) 
		Execute_RemoveIgnoredActor(this, a);
}

TArray<AActor*> UMounteaInteractorComponentBase::GetIgnoredActors_Implementation() const
{
	TArray<AActor*> out;
	for (auto a : ListOfIgnoredActors) { if (IsValid(a)) out.Add(a); }
	return out;
}

#pragma endregion

#pragma region IMounteaInteractorInterface_Dependencies

void UMounteaInteractorComponentBase::AddInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractorInterface>& InteractionDependency)
{
	if (!InteractionDependency.GetInterface()) return;
	InteractionDependencies.AddUnique(InteractionDependency);
}

void UMounteaInteractorComponentBase::RemoveInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractorInterface>& InteractionDependency)
{
	InteractionDependencies.Remove(InteractionDependency);
}

TArray<TScriptInterface<IMounteaInteractorInterface>> UMounteaInteractorComponentBase::GetInteractionDependencies_Implementation() const
{
	return InteractionDependencies;
}

void UMounteaInteractorComponentBase::ProcessDependencies_Implementation()
{
	for (const TScriptInterface<IMounteaInteractorInterface>& dep : InteractionDependencies)
	{
		if (!dep.GetInterface()) continue;
		const EInteractorStateV3 depState = IMounteaInteractorInterface::Execute_GetState(dep.GetObject());
		if (depState == EInteractorStateV3::EIS_Suppressed)
		{
			FString err;
			Execute_SuppressInteractor(this, err);
			return;
		}
	}
}

#pragma endregion

#pragma region IMounteaInteractorInterface_Debug

void UMounteaInteractorComponentBase::ToggleDebug_Implementation()
{
	DebugSettings.DebugMode = !DebugSettings.DebugMode;
}

FDebugSettings UMounteaInteractorComponentBase::GetDebugSettings_Implementation() const
{
	return DebugSettings;
}

void UMounteaInteractorComponentBase::OnInteractorComponentActivated_Implementation(UActorComponent* Component, bool bReset)
{
	if (DefaultInteractorState == EInteractorStateV3::EIS_Active)
	{
		FString err;
		Execute_ActivateInteractor(this, err);
	}
}

void UMounteaInteractorComponentBase::ConsumeInput_Implementation(UInputAction* ConsumedInput)
{
}

#pragma endregion

#pragma region FocusSystem

void UMounteaInteractorComponentBase::RefreshFocus()
{
	AActor* bestActor = nullptr;
	TScriptInterface<IMounteaInteractableInterface> bestInteractable;
	int32 bestWeight = -1;
	float bestDistanceSq = TNumericLimits<float>::Max();

	const FVector selfLoc = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;

	// Promotes the existing join-time tag gates (RequestInteractorJoin_Implementation) to focus
	// time: a tag-incompatible interactable should never earn focus/an attention widget only to
	// reject the player at the last second when they actually try to join.
	auto isTagCompatible = [this](UObject* InteractableObject) -> bool
	{
		const FGameplayTagContainer requiredTags = IMounteaInteractableInterface::Execute_GetRequiredInteractorTags(InteractableObject);
		const FGameplayTagContainer excludedTags = IMounteaInteractableInterface::Execute_GetExcludedInteractorTags(InteractableObject);
		if (requiredTags.IsEmpty() && excludedTags.IsEmpty()) return true;

		FGameplayTagContainer interactorTags;
		interactorTags.AddTag(Execute_GetInteractorTag(this));

		if (!requiredTags.IsEmpty() && !interactorTags.HasAny(requiredTags))
			return false;

		if (!excludedTags.IsEmpty() && interactorTags.HasAny(excludedTags))
			return false;

		return true;
	};

	for (const TScriptInterface<IMounteaInteractableInterface>& interactable : DetectedSet)
	{
		if (!interactable.GetInterface())
			continue;

		AActor* interactableOwner = IMounteaInteractableInterface::Execute_GetInteractableOwner(interactable.GetObject());
		if (!IsValid(interactableOwner))
			continue;

		if (!Execute_PerformSafetyTrace(this, interactableOwner))
			continue;

		if (!isTagCompatible(interactable.GetObject()))
			continue;

		const int32 weight = IMounteaInteractableInterface::Execute_GetInteractableWeight(interactable.GetObject());
		const float distSq = FVector::DistSquared(selfLoc, interactableOwner->GetActorLocation());

		const bool bBetter = weight > bestWeight || (weight == bestWeight && distSq < bestDistanceSq);
		if (bBetter)
		{
			bestWeight = weight;
			bestDistanceSq = distSq;
			bestActor = interactableOwner;
			bestInteractable = interactable;
		}
	}

	const bool bFocusChanged = bestActor != FocusedActor.Get();

	if (bFocusChanged)
	{
		if (UMounteaInteractionWorldSubsystem* subsystem = GetWorld() ? GetWorld()->GetSubsystem<UMounteaInteractionWorldSubsystem>() : nullptr)
		{
			if (FocusedActor.IsValid())
				subsystem->ReleaseAttentionWidget(FocusedActor.Get());
			if (IsValid(bestActor))
				subsystem->AssignAttentionWidget(bestActor);
		}

		if (bSelectionLocked && LockedAction.GetInterface())
		{
			AActor* lockedOwner = IMounteaInteractableInterface::Execute_GetInteractableOwner(LockedAction.GetObject());
			if (lockedOwner != bestActor)
			{
				bSelectionLocked = false;
				LockedAction = nullptr;
			}
		}

		FocusedActor = bestActor;
		OnFocusedActorChanged.Broadcast(FocusedActor.Get());
		Execute_OnFocusedActorChangedEvent(this, FocusedActor.Get());
	}

	// Runs on RefreshFocus()'s existing call cadence (overlap begin/end, each detection
	// subclass's own timer) instead of adding a new Tick. Every detected interactable gets its
	// own hosted widget gated independently by its own WidgetRanges - not just the focused one.
	for (const TScriptInterface<IMounteaInteractableInterface>& interactable : DetectedSet)
	{
		if (!interactable.GetInterface()) 
			continue;

		AActor* interactableOwner = IMounteaInteractableInterface::Execute_GetInteractableOwner(interactable.GetObject());
		if (!IsValid(interactableOwner)) 
			continue;

		const FInteractableWidgetRanges ranges = IMounteaInteractableInterface::Execute_GetWidgetRanges(interactable.GetObject());
		const float distSq = FVector::DistSquared(selfLoc, interactableOwner->GetActorLocation());

		IMounteaInteractableInterface::Execute_ToggleWidgetVisibility(interactable.GetObject(), distSq <= FMath::Square(ranges.InteractionRange));
	}

	// Uses the winning (highest-weight) interactable's own WidgetRanges as the actor-level
	// threshold - an actor with multiple interactable components could have differently
	// configured ranges per component, so the one that actually won Tier 1 governs.
	if (bestInteractable.GetInterface())
	{
		if (UMounteaInteractionWorldSubsystem* subsystem = GetWorld() ? GetWorld()->GetSubsystem<UMounteaInteractionWorldSubsystem>() : nullptr)
		{
			const FInteractableWidgetRanges ranges = IMounteaInteractableInterface::Execute_GetWidgetRanges(bestInteractable.GetObject());
			const bool bInInteractionRange = bestDistanceSq <= FMath::Square(ranges.InteractionRange);
			const bool bInAttentionRange = bestDistanceSq <= FMath::Square(ranges.AttentionRange);

			const EInteractionWidgetMode mode = bInInteractionRange ? EInteractionWidgetMode::EWM_Interaction
				: bInAttentionRange ? EInteractionWidgetMode::EWM_Attention : EInteractionWidgetMode::EWM_None;

			subsystem->UpdateAttentionWidgetMode(bestActor, mode);
		}
	}

	TArray<FInteractionActionCandidate> newActionList;

	if (FocusedActor.IsValid())
	{
		TArray<UActorComponent*> interactableComps;
		FocusedActor->GetComponents(interactableComps);

		const FVector focusedLoc = FocusedActor->GetActorLocation();

		for (UActorComponent* comp : interactableComps)
		{
			if (!comp || !comp->GetClass()->ImplementsInterface(UMounteaInteractableInterface::StaticClass())) 
				continue;

			TScriptInterface<IMounteaInteractableInterface> interactable;
			interactable.SetObject(comp);
			interactable.SetInterface(Cast<IMounteaInteractableInterface>(comp));

			if (!IMounteaInteractableInterface::Execute_CanBeTriggered(comp))
				continue;

			if (!isTagCompatible(comp))
				continue;

			FInteractionActionCandidate candidate;
			candidate.Interactable = interactable;
			candidate.CachedWeight = IMounteaInteractableInterface::Execute_GetInteractableWeight(comp);
			candidate.CachedDistanceSq = FVector::DistSquared(selfLoc, focusedLoc);

			newActionList.Add(candidate);
		}

		newActionList.Sort([](const FInteractionActionCandidate& A, const FInteractionActionCandidate& B)
		{
			return A.CachedWeight > B.CachedWeight;
		});

		if (newActionList.Num() > MaxCandidatesPerActor)
			newActionList.SetNum(MaxCandidatesPerActor);
	}

	const bool bListChanged = newActionList != ActionList;
	ActionList = MoveTemp(newActionList);

	if (ActionList.IsEmpty())
	{
		HighlightedActionIndex = 0;
	}
	else
	{
		HighlightedActionIndex = FMath::Clamp(HighlightedActionIndex, 0, ActionList.Num() - 1);

		if (ActionList[0].Interactable.GetInterface() && IMounteaInteractableInterface::Execute_IsStandaloneInteraction(ActionList[0].Interactable.GetObject()))
			HighlightedActionIndex = 0;
	}

	if (bListChanged)
	{
		OnActionListChanged.Broadcast(ActionList);
		Execute_OnActionListChangedEvent(this, ActionList);
	}
}

void UMounteaInteractorComponentBase::AddDetected(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetInterface()) 
		return;

	if (AActor* owner = IMounteaInteractableInterface::Execute_GetInteractableOwner(Interactable.GetObject()))
	{
		for (const TObjectPtr<AActor>& ignored : ListOfIgnoredActors)
		{
			if (ignored == owner) 
				return;
		}
	}

	DetectedSet.AddUnique(Interactable);
	RefreshFocus();
}

void UMounteaInteractorComponentBase::RemoveDetected(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	DetectedSet.Remove(Interactable);
	RefreshFocus();
}

void UMounteaInteractorComponentBase::ClearDetected()
{
	DetectedSet.Empty();

	if (FocusedActor.IsValid())
	{
		if (UMounteaInteractionWorldSubsystem* subsystem = GetWorld() ? GetWorld()->GetSubsystem<UMounteaInteractionWorldSubsystem>() : nullptr)
		{
			subsystem->ReleaseAttentionWidget(FocusedActor.Get());
		}
	}

	FocusedActor.Reset();
	ActionList.Empty();
	HighlightedActionIndex = 0;

	if (bSelectionLocked)
	{
		bSelectionLocked = false;
		LockedAction = nullptr;
	}

	OnFocusedActorChanged.Broadcast(nullptr);
	OnActionListChanged.Broadcast(ActionList);
}

#pragma endregion

#pragma region Networking

void UMounteaInteractorComponentBase::SetState_Server_Implementation(const EInteractorStateV3 NewState)
{
	Execute_SetState(this, NewState);
}

bool UMounteaInteractorComponentBase::SetState_Server_Validate(const EInteractorStateV3 NewState)
{
	return StaticEnum<EInteractorStateV3>()->IsValidEnumValue(static_cast<int64>(NewState));
}

void UMounteaInteractorComponentBase::StartInteraction_Server_Implementation(const float StartTime)
{
	Execute_StartInteraction(this, StartTime);
}

bool UMounteaInteractorComponentBase::StartInteraction_Server_Validate(const float StartTime)
{
	// StartTime is cosmetic only (actual InteractionStartServerTime is stamped server-side via
	// GetServerTime()) - the one cheap sanity check worth making is rejecting non-finite garbage
	// before it propagates into delegate broadcasts / UI.
	return FMath::IsFinite(StartTime);
}

void UMounteaInteractorComponentBase::StopInteraction_Server_Implementation(const float StopTime)
{
	Execute_StopInteraction(this, StopTime);
}

bool UMounteaInteractorComponentBase::StopInteraction_Server_Validate(const float StopTime)
{
	return FMath::IsFinite(StopTime);
}

void UMounteaInteractorComponentBase::CycleAction_Server_Implementation(const int32 Direction)
{
	if (bSelectionLocked || ActionList.IsEmpty()) return;

	// Recomputed independently from the server's own ActionList/HighlightedActionIndex - never
	// trusts a client-sent absolute index, only the raw +-1 direction (enforced by _Validate).
	HighlightedActionIndex = (HighlightedActionIndex + Direction + ActionList.Num()) % ActionList.Num();
	OnHighlightedActionChanged.Broadcast(HighlightedActionIndex);
	Execute_OnHighlightedActionChangedEvent(this, HighlightedActionIndex);
}

bool UMounteaInteractorComponentBase::CycleAction_Server_Validate(const int32 Direction)
{
	return Direction == -1 || Direction == 1;
}

void UMounteaInteractorComponentBase::SetResponseChannel_Server_Implementation(const ECollisionChannel NewResponseCollision)
{
	Execute_SetResponseChannel(this, NewResponseCollision);
}

bool UMounteaInteractorComponentBase::SetResponseChannel_Server_Validate(const ECollisionChannel NewResponseCollision)
{
	return static_cast<uint8>(NewResponseCollision) < static_cast<uint8>(ECC_MAX);
}

void UMounteaInteractorComponentBase::SetDefaultState_Server_Implementation(const EInteractorStateV3 NewState)
{
	Execute_SetDefaultState(this, NewState);
}

bool UMounteaInteractorComponentBase::SetDefaultState_Server_Validate(const EInteractorStateV3 NewState)
{
	return StaticEnum<EInteractorStateV3>()->IsValidEnumValue(static_cast<int64>(NewState));
}

void UMounteaInteractorComponentBase::SetInteractorTag_Server_Implementation(const FGameplayTag& NewInteractorTag)
{
	Execute_SetInteractorTag(this, NewInteractorTag);
}

bool UMounteaInteractorComponentBase::SetInteractorTag_Server_Validate(const FGameplayTag& NewInteractorTag)
{
	// Nothing meaningful to reject here: FGameplayTag replicates as an index into the shared,
	// engine-wide tag table, so the wire format itself already rules out a forged/garbage tag -
	// and an empty tag (clearing it) is a legitimate value, not an attack.
	return true;
}

void UMounteaInteractorComponentBase::AddIgnoredActor_Server_Implementation(AActor* IgnoredActor)
{
	Execute_AddIgnoredActor(this, IgnoredActor);
}

bool UMounteaInteractorComponentBase::AddIgnoredActor_Server_Validate(AActor* IgnoredActor)
{
	return IgnoredActor != nullptr;
}

void UMounteaInteractorComponentBase::RemoveIgnoredActor_Server_Implementation(AActor* IgnoredActor)
{
	Execute_RemoveIgnoredActor(this, IgnoredActor);
}

bool UMounteaInteractorComponentBase::RemoveIgnoredActor_Server_Validate(AActor* IgnoredActor)
{
	return IgnoredActor != nullptr;
}

void UMounteaInteractorComponentBase::SetSafetyTracingSetup_Server_Implementation(const FSafetyTracingSetup& NewSafetyTracingSetup)
{
	Execute_SetSafetyTracingSetup(this, NewSafetyTracingSetup);
}

bool UMounteaInteractorComponentBase::SetSafetyTracingSetup_Server_Validate(const FSafetyTracingSetup& NewSafetyTracingSetup)
{
	return StaticEnum<ESafetyTracingMode>()->IsValidEnumValue(static_cast<int64>(NewSafetyTracingSetup.SafetyTracingMode));
}

void UMounteaInteractorComponentBase::BroadcastStateChanged()
{
	OnStateChanged.Broadcast(InteractorState);
	Execute_OnInteractorStateChangedEvent(this, InteractorState);
}

void UMounteaInteractorComponentBase::OnRep_InteractorState()
{
	// Engine-invoked on remote clients when InteractorState replicates in.
	ProcessStateChanged();
}

void UMounteaInteractorComponentBase::OnRep_HighlightedActionIndex()
{
	// Engine-invoked on the owning client when the server's authoritative value replicates in -
	// corrects any local misprediction from CycleAction_Implementation's immediate-feedback path.
	OnHighlightedActionChanged.Broadcast(HighlightedActionIndex);
	Execute_OnHighlightedActionChangedEvent(this, HighlightedActionIndex);
}

void UMounteaInteractorComponentBase::ProcessStateChanged()
{
	// Base: just the broadcast. Overlap/Trace/Mouse override this (and call Super::) to also
	// bind/unbind their detection - collision overlap events / trace timers - on the transition.
	BroadcastStateChanged();
}

#pragma endregion

#pragma region Editor

#if WITH_EDITOR

void UMounteaInteractorComponentBase::SetDefaultValues()
{
	Execute_SetDefaults(this);
	RequestEditorDefaults.Broadcast();
}

void UMounteaInteractorComponentBase::ResetDefaults()
{
	Execute_SetDefaults(this);
}

void UMounteaInteractorComponentBase::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

EDataValidationResult UMounteaInteractorComponentBase::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult validationResult = Super::IsDataValid(Context);
	return validationResult;
}

#endif

#pragma endregion

#undef LOCTEXT_NAMESPACE
