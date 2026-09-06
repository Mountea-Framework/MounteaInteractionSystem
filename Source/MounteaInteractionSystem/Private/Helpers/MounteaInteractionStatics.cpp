// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Helpers/MounteaInteractionStatics.h"
#include "Engine/World.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameStateBase.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Interfaces/MounteaInteractorInterface.h"
#include "Helpers/MounteaInteractionSystemLog.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionStatics"

float UMounteaInteractionStatics::GetServerTime(const UObject* WorldContext)
{
	if (!WorldContext)
				return 0.f;

	const UWorld* world = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!world)
				return 0.f;

	if (const AGameStateBase* gameState = world->GetGameState())
	{
		return gameState->GetServerWorldTimeSeconds();
	}

	LOG_WARNING(TEXT("[MounteaInteractionStatics] GetServerTime: no GameState yet, falling back to local time"));
	return world->GetTimeSeconds();
}

bool UMounteaInteractionStatics::CanInteractorJoinSlot(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactable.GetObject() || !Interactor.GetObject())
				return false;
	if (!Interactable->Execute_IsSlotAvailable(Interactable.GetObject()))
				return false;
	if (!Interactable->Execute_CanBeTriggered(Interactable.GetObject()))
				return false;

	const FGameplayTagContainer requiredTags = Interactable->Execute_GetRequiredInteractorTags(Interactable.GetObject());
	if (!requiredTags.IsEmpty())
	{
		const FGameplayTag interactorTag = Interactor->Execute_GetInteractorTag(Interactor.GetObject());
		if (!requiredTags.HasTag(interactorTag))
				return false;
	}

	return true;
}

void UMounteaInteractionStatics::SortActionCandidates(TArray<FInteractionActionCandidate>& Candidates)
{
	Candidates.Sort([](const FInteractionActionCandidate& A, const FInteractionActionCandidate& B)
	{
		if (A.CachedWeight != B.CachedWeight)
				return A.CachedWeight > B.CachedWeight;
		return A.CachedDistanceSq < B.CachedDistanceSq;
	});
}

TArray<FInteractionActionCandidate> UMounteaInteractionStatics::BuildActionCandidatesForActor(
	const AActor* Actor,
	const FVector& ViewOrigin)
{
	TArray<FInteractionActionCandidate> candidates;
	if (!IsValid(Actor))
				return candidates;

	const float distSq = FVector::DistSquared(Actor->GetActorLocation(), ViewOrigin);

	TArray<UActorComponent*> interactableComps = Actor->GetComponentsByInterface(UMounteaInteractableInterface::StaticClass());
	for (UActorComponent* comp : interactableComps)
	{
		if (!IsValid(comp))
				continue;

		TScriptInterface<IMounteaInteractableInterface> interactable;
		interactable.SetObject(comp);
		interactable.SetInterface(Cast<IMounteaInteractableInterface>(comp));

		if (!interactable.GetInterface())
				continue;
		if (!IMounteaInteractableInterface::Execute_CanBeTriggered(comp))
				continue;

		FInteractionActionCandidate candidate;
		candidate.Interactable = interactable;
		candidate.CachedWeight = IMounteaInteractableInterface::Execute_GetInteractableWeight(comp);
		candidate.CachedDistanceSq = distSq;
		candidates.Add(candidate);
	}

	SortActionCandidates(candidates);
	return candidates;
}

AActor* UMounteaInteractionStatics::SelectBestFocusActor(
	const TArray<TScriptInterface<IMounteaInteractableInterface>>& DetectedInteractables,
	const FVector& ViewOrigin)
{
	AActor* bestActor = nullptr;
	int32 bestWeight = INT32_MIN;
	float bestDistSq = FLT_MAX;

	for (const TScriptInterface<IMounteaInteractableInterface>& Interactable : DetectedInteractables)
	{
		if (!Interactable.GetObject())
				continue;

		AActor* owner = Interactable->Execute_GetOwningActor(Interactable.GetObject());
		if (!IsValid(owner))
				continue;

		const int32 weight = Interactable->Execute_GetInteractableWeight(Interactable.GetObject());
		const float distSq = FVector::DistSquared(owner->GetActorLocation(), ViewOrigin);

		if (weight > bestWeight || (weight == bestWeight && distSq < bestDistSq))
		{
			bestActor = owner;
			bestWeight = weight;
			bestDistSq = distSq;
		}
	}

	return bestActor;
}

bool UMounteaInteractionStatics::IsInteractableInRange(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const FVector& ViewOrigin,
	const float RangeCm)
{
	if (!Interactable.GetObject())
				return false;

	AActor* owner = Interactable->Execute_GetOwningActor(Interactable.GetObject());
	if (!IsValid(owner))
				return false;

	return FVector::DistSquared(owner->GetActorLocation(), ViewOrigin) <= FMath::Square(RangeCm);
}

bool UMounteaInteractionStatics::IsValidInteractable(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	return Interactable.GetObject() != nullptr && Interactable.GetInterface() != nullptr;
}

bool UMounteaInteractionStatics::IsValidInteractor(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	return Interactor.GetObject() != nullptr && Interactor.GetInterface() != nullptr;
}

TScriptInterface<IMounteaInteractableInterface> UMounteaInteractionStatics::GetFirstInteractableFromActor(const AActor* Actor)
{
	TScriptInterface<IMounteaInteractableInterface> result;
	if (!IsValid(Actor))
				return result;

	TArray<UActorComponent*> comps = Actor->GetComponentsByInterface(UMounteaInteractableInterface::StaticClass());
	if (comps.IsEmpty())
				return result;

	UActorComponent* first = comps[0];
	result.SetObject(first);
	result.SetInterface(Cast<IMounteaInteractableInterface>(first));
	return result;
}

TScriptInterface<IMounteaInteractorInterface> UMounteaInteractionStatics::GetFirstInteractorFromActor(const AActor* Actor)
{
	TScriptInterface<IMounteaInteractorInterface> result;
	if (!IsValid(Actor))
				return result;

	TArray<UActorComponent*> comps = Actor->GetComponentsByInterface(UMounteaInteractorInterface::StaticClass());
	if (comps.IsEmpty())
				return result;

	UActorComponent* first = comps[0];
	result.SetObject(first);
	result.SetInterface(Cast<IMounteaInteractorInterface>(first));
	return result;
}

ECollisionChannel UMounteaInteractionStatics::ResolveInteractionChannel(const FName& ChannelName, bool& bFound)
{
	bFound = false;
	UCollisionProfile* profile = UCollisionProfile::Get();
	if (!profile || ChannelName.IsNone())
		return ECC_MAX;

	for (int32 i = static_cast<int32>(ECC_GameTraceChannel1); i <= static_cast<int32>(ECC_GameTraceChannel18); ++i)
	{
		if (profile->ReturnChannelNameFromContainerIndex(i) == ChannelName)
		{
			bFound = true;
			return static_cast<ECollisionChannel>(i);
		}
	}
	return ECC_MAX;
}

ECollisionChannel UMounteaInteractionStatics::ResolveChannelOrWarn(const FName& ChannelName)
{
	if (ChannelName.IsNone())
	{
		LOG_WARNING(TEXT("ResolveChannelOrWarn: ChannelName is None — returning ECC_MAX."));
		return ECC_MAX;
	}

	bool bFound = false;
	const ECollisionChannel channel = ResolveInteractionChannel(ChannelName, bFound);
	if (!bFound)
	{
		LOG_WARNING(TEXT("ResolveChannelOrWarn: Channel '%s' not registered in collision profile — returning ECC_MAX."), *ChannelName.ToString());
	}
	return channel;
}

#undef LOCTEXT_NAMESPACE
