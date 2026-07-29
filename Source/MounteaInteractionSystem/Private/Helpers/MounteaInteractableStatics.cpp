// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Helpers/MounteaInteractableStatics.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Components/MeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInterface.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Interfaces/MounteaInteractorInterface.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableStatics"

int32 UMounteaInteractableStatics::GetInteractorSlotCount(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return 0;
	return Interactable->Execute_GetActiveInteractorSlots(Interactable.GetObject()).Num();
}

int32 UMounteaInteractableStatics::GetMaxInteractors(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return 0;
	return Interactable->Execute_GetMaxInteractors(Interactable.GetObject());
}

bool UMounteaInteractableStatics::IsSlotAvailable(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return false;
	return Interactable->Execute_IsSlotAvailable(Interactable.GetObject());
}

TArray<FInteractorSlot> UMounteaInteractableStatics::GetActiveInteractorSlots(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return {};
	return Interactable->Execute_GetActiveInteractorSlots(Interactable.GetObject());
}

bool UMounteaInteractableStatics::IsInteractorInSlot(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactable.GetObject() || !Interactor.GetObject()) 
		return false;

	const TArray<FInteractorSlot> slots = Interactable->Execute_GetActiveInteractorSlots(Interactable.GetObject());
	for (const FInteractorSlot& slot : slots)
	{
		if (slot.Interactor.GetObject() == Interactor.GetObject())
			return true;
	}
	return false;
}

float UMounteaInteractableStatics::GetInteractionProgress(
	const UObject* WorldContext,
	const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return 0.f;
	return Interactable->Execute_GetInteractionProgress(Interactable.GetObject());
}

float UMounteaInteractableStatics::GetInteractionPeriod(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return 0.f;
	return Interactable->Execute_GetInteractionPeriod(Interactable.GetObject());
}

int32 UMounteaInteractableStatics::GetRemainingLifecycleCount(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return 0;
	return Interactable->Execute_GetRemainingLifecycleCount(Interactable.GetObject());
}

EInteractableStateV3 UMounteaInteractableStatics::GetInteractableState(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return EInteractableStateV3::EIS_Disabled;
	return Interactable->Execute_GetState(Interactable.GetObject());
}

bool UMounteaInteractableStatics::CanInteractableBeTriggered(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return false;
	return Interactable->Execute_CanBeTriggered(Interactable.GetObject());
}

bool UMounteaInteractableStatics::IsInteractableInteracting(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return false;
	return Interactable->Execute_IsInteracting(Interactable.GetObject());
}

bool UMounteaInteractableStatics::IsStandaloneInteraction(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return false;
	return Interactable->Execute_IsStandaloneInteraction(Interactable.GetObject());
}

FInteractableWidgetRanges UMounteaInteractableStatics::GetWidgetRanges(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return FInteractableWidgetRanges();
	return Interactable->Execute_GetWidgetRanges(Interactable.GetObject());
}

int32 UMounteaInteractableStatics::GetInteractableWeight(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject()) 
		return 0;
	return Interactable->Execute_GetInteractableWeight(Interactable.GetObject());
}

void UMounteaInteractableStatics::SetInteractableState(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const EInteractableStateV3 NewState)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetState(Interactable.GetObject(), NewState);
}

bool UMounteaInteractableStatics::ActivateInteractable(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	FString& ErrorMessage)
{
	if (!Interactable.GetObject())
	{
		ErrorMessage = TEXT("Invalid interactable."); 
		return false;
	}
	return Interactable->Execute_ActivateInteractable(Interactable.GetObject(), ErrorMessage);
}

void UMounteaInteractableStatics::DeactivateInteractable(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_DeactivateInteractable(Interactable.GetObject());
}

bool UMounteaInteractableStatics::WakeUpInteractable(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	FString& ErrorMessage)
{
	if (!Interactable.GetObject())
	{
		ErrorMessage = TEXT("Invalid interactable."); 
		return false;
	}
	return Interactable->Execute_WakeUpInteractable(Interactable.GetObject(), ErrorMessage);
}

void UMounteaInteractableStatics::SetInteractionPeriod(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const float NewPeriod)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetInteractionPeriod(Interactable.GetObject(), NewPeriod);
}

void UMounteaInteractableStatics::SetWidgetRanges(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const FInteractableWidgetRanges& NewRanges)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetWidgetRanges(Interactable.GetObject(), NewRanges);
}

void UMounteaInteractableStatics::SetStandaloneInteraction(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const bool bStandalone)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetStandaloneInteraction(Interactable.GetObject(), bStandalone);
}

void UMounteaInteractableStatics::RequestInteractorJoin(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactable.GetObject() || !Interactor.GetObject())
		return;
	Interactable->Execute_RequestInteractorJoin(Interactable.GetObject(), Interactor);
}

void UMounteaInteractableStatics::RequestInteractorLeave(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const TScriptInterface<IMounteaInteractorInterface>& Interactor)
{
	if (!Interactable.GetObject() || !Interactor.GetObject())
		return;
	Interactable->Execute_RequestInteractorLeave(Interactable.GetObject(), Interactor);
}

FGameplayTagContainer UMounteaInteractableStatics::GetRequiredInteractorTags(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return {};
	return Interactable->Execute_GetRequiredInteractorTags(Interactable.GetObject());
}

EInteractableStateV3 UMounteaInteractableStatics::GetDefaultInteractableState(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return EInteractableStateV3::EIS_Disabled;
	return Interactable->Execute_GetDefaultState(Interactable.GetObject());
}

void UMounteaInteractableStatics::SetDefaultInteractableState(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const EInteractableStateV3 NewState)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetDefaultState(Interactable.GetObject(), NewState);
}

bool UMounteaInteractableStatics::TriggerCooldown(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return false;
	return Interactable->Execute_TriggerCooldown(Interactable.GetObject());
}

void UMounteaInteractableStatics::PauseInteraction(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const float ExpirationTime,
	const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_PauseInteraction(Interactable.GetObject(), ExpirationTime, CausingInteractor);
}

void UMounteaInteractableStatics::SetInteractableWeight(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const int32 NewWeight)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetInteractableWeight(Interactable.GetObject(), NewWeight);
}

float UMounteaInteractableStatics::GetCooldownPeriod(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return 0.f;
	return Interactable->Execute_GetCooldownPeriod(Interactable.GetObject());
}

void UMounteaInteractableStatics::SetCooldownPeriod(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const float NewCooldownPeriod)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetCooldownPeriod(Interactable.GetObject(), NewCooldownPeriod);
}

EInteractableLifecycle UMounteaInteractableStatics::GetLifecycleMode(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return EInteractableLifecycle::EIL_Cycled;
	return Interactable->Execute_GetLifecycleMode(Interactable.GetObject());
}

void UMounteaInteractableStatics::SetLifecycleMode(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const EInteractableLifecycle NewMode)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetLifecycleMode(Interactable.GetObject(), NewMode);
}

int32 UMounteaInteractableStatics::GetLifecycleCount(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return 0;
	return Interactable->Execute_GetLifecycleCount(Interactable.GetObject());
}

void UMounteaInteractableStatics::SetLifecycleCount(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const int32 NewLifecycleCount)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetLifecycleCount(Interactable.GetObject(), NewLifecycleCount);
}

FText UMounteaInteractableStatics::GetInteractableName(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return FText::GetEmpty();
	return Interactable->Execute_GetInteractableName(Interactable.GetObject());
}

void UMounteaInteractableStatics::SetInteractableName(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const FText& NewName)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetInteractableName(Interactable.GetObject(), NewName);
}

FDataTableRowHandle UMounteaInteractableStatics::GetInteractableData(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return FDataTableRowHandle();
	return Interactable->Execute_GetInteractableData(Interactable.GetObject());
}

void UMounteaInteractableStatics::SetInteractableData(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const FDataTableRowHandle& NewData)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetInteractableData(Interactable.GetObject(), NewData);
}

FGameplayTagContainer UMounteaInteractableStatics::GetInteractableCompatibleTags(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return {};
	return Interactable->Execute_GetInteractableCompatibleTags(Interactable.GetObject());
}

void UMounteaInteractableStatics::SetInteractableCompatibleTags(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const FGameplayTagContainer& Tags)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetInteractableCompatibleTags(Interactable.GetObject(), Tags);
}

void UMounteaInteractableStatics::AddInteractableCompatibleTag(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const FGameplayTag& Tag)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_AddInteractableCompatibleTag(Interactable.GetObject(), Tag);
}

void UMounteaInteractableStatics::AddInteractableCompatibleTags(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const FGameplayTagContainer& Tags)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_AddInteractableCompatibleTags(Interactable.GetObject(), Tags);
}

void UMounteaInteractableStatics::RemoveInteractableCompatibleTag(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const FGameplayTag& Tag)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_RemoveInteractableCompatibleTag(Interactable.GetObject(), Tag);
}

void UMounteaInteractableStatics::RemoveInteractableCompatibleTags(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const FGameplayTagContainer& Tags)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_RemoveInteractableCompatibleTags(Interactable.GetObject(), Tags);
}

void UMounteaInteractableStatics::ClearInteractableCompatibleTags(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_ClearInteractableCompatibleTags(Interactable.GetObject());
}

bool UMounteaInteractableStatics::DoesInteractableAutoSetup(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return false;
	return Interactable->Execute_DoesAutoSetup(Interactable.GetObject());
}

void UMounteaInteractableStatics::SetInteractableAutoSetup(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const ESetupType NewSetupType)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_ToggleAutoSetup(Interactable.GetObject(), NewSetupType);
}

void UMounteaInteractableStatics::SetInteractableDefaults(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetDefaults(Interactable.GetObject());
}

void UMounteaInteractableStatics::StartInteractableHighlight(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_StartHighlight(Interactable.GetObject());
}

void UMounteaInteractableStatics::StopInteractableHighlight(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_StopHighlight(Interactable.GetObject());
}

EHighlightType UMounteaInteractableStatics::GetInteractableHighlightType(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return EHighlightType::EHT_OverlayMaterial;
	return Interactable->Execute_GetHighlightType(Interactable.GetObject());
}

void UMounteaInteractableStatics::SetInteractableHighlightType(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const EHighlightType NewHighlightType)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetHighlightType(Interactable.GetObject(), NewHighlightType);
}

UMaterialInterface* UMounteaInteractableStatics::GetInteractableHighlightMaterial(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return nullptr;
	return Interactable->Execute_GetHighlightMaterial(Interactable.GetObject());
}

void UMounteaInteractableStatics::SetInteractableHighlightMaterial(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	UMaterialInterface* NewMaterial)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_SetHighlightMaterial(Interactable.GetObject(), NewMaterial);
}

TArray<UPrimitiveComponent*> UMounteaInteractableStatics::GetInteractableCollisionComponents(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return {};
	return Interactable->Execute_GetCollisionComponents(Interactable.GetObject());
}

void UMounteaInteractableStatics::AddInteractableCollisionComponent(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	UPrimitiveComponent* CollisionComp)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_AddCollisionComponent(Interactable.GetObject(), CollisionComp);
}

void UMounteaInteractableStatics::RemoveInteractableCollisionComponent(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	UPrimitiveComponent* CollisionComp)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_RemoveCollisionComponent(Interactable.GetObject(), CollisionComp);
}

TArray<UMeshComponent*> UMounteaInteractableStatics::GetInteractableHighlightableComponents(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return {};
	return Interactable->Execute_GetHighlightableComponents(Interactable.GetObject());
}

void UMounteaInteractableStatics::AddInteractableHighlightableComponent(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	UMeshComponent* HighlightableComp)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_AddHighlightableComponent(Interactable.GetObject(), HighlightableComp);
}

void UMounteaInteractableStatics::RemoveInteractableHighlightableComponent(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	UMeshComponent* HighlightableComp)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_RemoveHighlightableComponent(Interactable.GetObject(), HighlightableComp);
}

TArray<TSoftClassPtr<UObject>> UMounteaInteractableStatics::GetIgnoredClasses(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return {};
	return Interactable->Execute_GetIgnoredClasses(Interactable.GetObject());
}

void UMounteaInteractableStatics::AddIgnoredClass(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const TSoftClassPtr<UObject>& IgnoredClass)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_AddIgnoredClass(Interactable.GetObject(), IgnoredClass);
}

void UMounteaInteractableStatics::RemoveIgnoredClass(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const TSoftClassPtr<UObject>& IgnoredClass)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_RemoveIgnoredClass(Interactable.GetObject(), IgnoredClass);
}

TArray<TScriptInterface<IMounteaInteractableInterface>> UMounteaInteractableStatics::GetInteractableDependencies(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return {};
	return Interactable->Execute_GetInteractionDependencies(Interactable.GetObject());
}

void UMounteaInteractableStatics::AddInteractableDependency(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const TScriptInterface<IMounteaInteractableInterface>& Dependency)
{
	if (!Interactable.GetObject() || !Dependency.GetObject())
		return;
	Interactable->Execute_AddInteractionDependency(Interactable.GetObject(), Dependency);
}

void UMounteaInteractableStatics::RemoveInteractableDependency(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const TScriptInterface<IMounteaInteractableInterface>& Dependency)
{
	if (!Interactable.GetObject() || !Dependency.GetObject())
		return;
	Interactable->Execute_RemoveInteractionDependency(Interactable.GetObject(), Dependency);
}

void UMounteaInteractableStatics::ToggleInteractableWidgetVisibility(
	const TScriptInterface<IMounteaInteractableInterface>& Interactable,
	const bool bVisible)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_ToggleWidgetVisibility(Interactable.GetObject(), bVisible);
}

FDebugSettings UMounteaInteractableStatics::GetInteractableDebugSettings(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return FDebugSettings();
	return Interactable->Execute_GetDebugSettings(Interactable.GetObject());
}

void UMounteaInteractableStatics::ToggleInteractableDebug(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (!Interactable.GetObject())
		return;
	Interactable->Execute_ToggleDebug(Interactable.GetObject());
}

#undef LOCTEXT_NAMESPACE
