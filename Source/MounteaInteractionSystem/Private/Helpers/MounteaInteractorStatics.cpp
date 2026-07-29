// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Helpers/MounteaInteractorStatics.h"
#include "Engine/World.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "Interfaces/MounteaInteractorInterface.h"
#include "Interfaces/MounteaInteractableInterface.h"

#define LOCTEXT_NAMESPACE "MounteaInteractorStatics"

AActor* UMounteaInteractorStatics::GetFocusedActor(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return nullptr;
	return Interactor->Execute_GetFocusedActor(Interactor.GetObject());
}

int32 UMounteaInteractorStatics::GetActionCount(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return 0;
	return Interactor->Execute_GetActionList(Interactor.GetObject()).Num();
}

TArray<FInteractionActionCandidate> UMounteaInteractorStatics::GetActionList(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject()) 
		return {};
	return Interactor->Execute_GetActionList(Interactor.GetObject());
}

FInteractionActionCandidate UMounteaInteractorStatics::GetHighlightedAction(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject()) 
		return FInteractionActionCandidate();

	const TArray<FInteractionActionCandidate> list = Interactor->Execute_GetActionList(Interactor.GetObject());
	const int32 index = Interactor->Execute_GetHighlightedActionIndex(Interactor.GetObject());

	if (!list.IsValidIndex(index))
		return FInteractionActionCandidate();
	return list[index];
}

int32 UMounteaInteractorStatics::GetHighlightedActionIndex(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return 0;
	return Interactor->Execute_GetHighlightedActionIndex(Interactor.GetObject());
}

TScriptInterface<IMounteaInteractableInterface> UMounteaInteractorStatics::GetLockedAction(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return TScriptInterface<IMounteaInteractableInterface>();
	return Interactor->Execute_GetLockedAction(Interactor.GetObject());
}

bool UMounteaInteractorStatics::IsSelectionLocked(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return false;
	return Interactor->Execute_IsSelectionLocked(Interactor.GetObject());
}

bool UMounteaInteractorStatics::HasInteractable(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return false;
	return Interactor->Execute_HasInteractable(Interactor.GetObject());
}

EInteractorStateV3 UMounteaInteractorStatics::GetInteractorState(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return EInteractorStateV3::EIS_Disabled;
	return Interactor->Execute_GetState(Interactor.GetObject());
}

bool UMounteaInteractorStatics::CanInteract(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return false;
	return Interactor->Execute_CanInteract(Interactor.GetObject());
}

ECollisionChannel UMounteaInteractorStatics::GetInteractorCollisionChannel(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return ECC_Visibility;
	return Interactor->Execute_GetResponseChannel(Interactor.GetObject());
}

FGameplayTag UMounteaInteractorStatics::GetInteractorTag(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return FGameplayTag::EmptyTag;
	return Interactor->Execute_GetInteractorTag(Interactor.GetObject());
}

void UMounteaInteractorStatics::CycleInteractorAction(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	const int32 Direction)
{
	if (!Interactor.GetObject())
		return;
	Interactor->Execute_CycleAction(Interactor.GetObject(), Direction);
}

void UMounteaInteractorStatics::StartInteractorInteraction(
	const UObject* WorldContext,
	const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return;

	float serverTime = 0.f;
	if (WorldContext)
	{
		const UWorld* world = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
		if (world) 
			serverTime = world->GetTimeSeconds();
	}

	Interactor->Execute_StartInteraction(Interactor.GetObject(), serverTime);
}

void UMounteaInteractorStatics::StopInteractorInteraction(
	const UObject* WorldContext,
	const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return;

	float serverTime = 0.f;
	if (WorldContext)
	{
		const UWorld* world = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
		if (world)
			serverTime = world->GetTimeSeconds();
	}

	Interactor->Execute_StopInteraction(Interactor.GetObject(), serverTime);
}

bool UMounteaInteractorStatics::ActivateInteractor(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	FString& ErrorMessage)
{
	if (!Interactor.GetObject())
	{
		ErrorMessage = TEXT("Invalid interactor."); 
		return false;
	}
	return Interactor->Execute_ActivateInteractor(Interactor.GetObject(), ErrorMessage);
}

void UMounteaInteractorStatics::DeactivateInteractor(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return;
	Interactor->Execute_DeactivateInteractor(Interactor.GetObject());
}

bool UMounteaInteractorStatics::SuppressInteractor(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	FString& ErrorMessage)
{
	if (!Interactor.GetObject())
	{
		ErrorMessage = TEXT("Invalid interactor."); 
		return false;
	}
	return Interactor->Execute_SuppressInteractor(Interactor.GetObject(), ErrorMessage);
}

void UMounteaInteractorStatics::SetInteractorCollisionChannel(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	const ECollisionChannel NewChannel)
{
	if (!Interactor.GetObject())
		return;
	Interactor->Execute_SetResponseChannel(Interactor.GetObject(), NewChannel);
}

void UMounteaInteractorStatics::SetInteractorTag(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	const FGameplayTag& NewTag)
{
	if (!Interactor.GetObject())
		return;
	Interactor->Execute_SetInteractorTag(Interactor.GetObject(), NewTag);
}

void UMounteaInteractorStatics::AddIgnoredActor(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	AActor* ActorToIgnore)
{
	if (!Interactor.GetObject() || !IsValid(ActorToIgnore))
		return;
	Interactor->Execute_AddIgnoredActor(Interactor.GetObject(), ActorToIgnore);
}

void UMounteaInteractorStatics::RemoveIgnoredActor(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	AActor* ActorToUnignore)
{
	if (!Interactor.GetObject() || !IsValid(ActorToUnignore))
		return;
	Interactor->Execute_RemoveIgnoredActor(Interactor.GetObject(), ActorToUnignore);
}

TArray<AActor*> UMounteaInteractorStatics::GetIgnoredActors(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return {};
	return Interactor->Execute_GetIgnoredActors(Interactor.GetObject());
}

void UMounteaInteractorStatics::AddIgnoredActors(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	const TArray<AActor*>& ActorsToIgnore)
{
	if (!Interactor.GetObject())
		return;
	Interactor->Execute_AddIgnoredActors(Interactor.GetObject(), ActorsToIgnore);
}

void UMounteaInteractorStatics::RemoveIgnoredActors(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	const TArray<AActor*>& ActorsToUnignore)
{
	if (!Interactor.GetObject())
		return;
	Interactor->Execute_RemoveIgnoredActors(Interactor.GetObject(), ActorsToUnignore);
}

bool UMounteaInteractorStatics::DoesInteractorAutoActivate(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return false;
	return Interactor->Execute_DoesAutoActivate(Interactor.GetObject());
}

AActor* UMounteaInteractorStatics::GetInteractorOwningActor(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return nullptr;
	return Interactor->Execute_GetOwningActor(Interactor.GetObject());
}

FSafetyTracingSetup UMounteaInteractorStatics::GetSafetyTracingSetup(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return FSafetyTracingSetup();
	return Interactor->Execute_GetSafetyTracingSetup(Interactor.GetObject());
}

void UMounteaInteractorStatics::SetSafetyTracingSetup(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	const FSafetyTracingSetup& NewSetup)
{
	if (!Interactor.GetObject())
		return;
	Interactor->Execute_SetSafetyTracingSetup(Interactor.GetObject(), NewSetup);
}

void UMounteaInteractorStatics::SetInteractorDefaults(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return;
	Interactor->Execute_SetDefaults(Interactor.GetObject());
}

bool UMounteaInteractorStatics::PerformSafetyTrace(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	const AActor* InteractableActor)
{
	if (!Interactor.GetObject())
		return false;
	return Interactor->Execute_PerformSafetyTrace(Interactor.GetObject(), InteractableActor);
}

void UMounteaInteractorStatics::ConsumeInteractorInput(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	UInputAction* ConsumedInput)
{
	if (!Interactor.GetObject())
		return;
	Interactor->Execute_ConsumeInput(Interactor.GetObject(), ConsumedInput);
}

TArray<TScriptInterface<IMounteaInteractorInterface>> UMounteaInteractorStatics::GetInteractorDependencies(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return {};
	return Interactor->Execute_GetInteractionDependencies(Interactor.GetObject());
}

void UMounteaInteractorStatics::AddInteractorDependency(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	const TScriptInterface<IMounteaInteractorInterface>& Dependency)
{
	if (!Interactor.GetObject() || !Dependency.GetObject())
		return;
	Interactor->Execute_AddInteractionDependency(Interactor.GetObject(), Dependency);
}

void UMounteaInteractorStatics::RemoveInteractorDependency(
	const TScriptInterface<IMounteaInteractorInterface>& Interactor,
	const TScriptInterface<IMounteaInteractorInterface>& Dependency)
{
	if (!Interactor.GetObject() || !Dependency.GetObject())
		return;
	Interactor->Execute_RemoveInteractionDependency(Interactor.GetObject(), Dependency);
}

void UMounteaInteractorStatics::ProcessInteractorDependencies(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return;
	Interactor->Execute_ProcessDependencies(Interactor.GetObject());
}

FDebugSettings UMounteaInteractorStatics::GetInteractorDebugSettings(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return FDebugSettings();
	return Interactor->Execute_GetDebugSettings(Interactor.GetObject());
}

void UMounteaInteractorStatics::ToggleInteractorDebug(const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactor.GetObject())
		return;
	Interactor->Execute_ToggleDebug(Interactor.GetObject());
}

#undef LOCTEXT_NAMESPACE
