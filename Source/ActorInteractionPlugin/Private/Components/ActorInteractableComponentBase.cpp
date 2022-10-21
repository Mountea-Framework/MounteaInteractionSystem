// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentBase.h"

#include "Helpers/InteractionHelpers.h"

UActorInteractableComponentBase::UActorInteractableComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UActorInteractableComponentBase::BeginPlay()
{
	Super::BeginPlay();
}

void UActorInteractableComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}



bool UActorInteractableComponentBase::DoesAutoActive() const
{
	return bInteractableAutoActivate;
}

void UActorInteractableComponentBase::ToggleAutoActivate(const bool NewValue)
{
	bInteractableAutoActivate = NewValue;
}

bool UActorInteractableComponentBase::DoesAutoSetup() const
{
	return bInteractableAutoSetup;
}

void UActorInteractableComponentBase::ToggleAutoSetup(const bool NewValue)
{
	bInteractableAutoSetup = NewValue;
}

bool UActorInteractableComponentBase::ActivateInteractable(FString& ErrorMessage)
{
	return false;
}

bool UActorInteractableComponentBase::WakeUpInteractable(FString& ErrorMessage)
{
	return false;
}

bool UActorInteractableComponentBase::SuppressInteractable(FString& ErrorMessage)
{
	return false;
}

void UActorInteractableComponentBase::DeactivateInteractable()
{
}

EInteractableState UActorInteractableComponentBase::GetState() const
{
	return EInteractableState::EIS_Standby;
}

void UActorInteractableComponentBase::SetState(const EInteractableState& NewState)
{
	
}

TScriptInterface<IActorInteractorInterface> UActorInteractableComponentBase::GetInteractor() const
{
	return nullptr;
}

void UActorInteractableComponentBase::SetInteractor(const TScriptInterface<IActorInteractorInterface> NewInteractor)
{
}

int32 UActorInteractableComponentBase::GetInteractableWeight() const
{
	return 0;
}

void UActorInteractableComponentBase::SetInteractableWeight(const int32 NewWeight)
{
}

UObject* UActorInteractableComponentBase::GetInteractableOwner() const
{
	return nullptr;
}

void UActorInteractableComponentBase::SetInteractableOwner(const UObject* NewOwner)
{
}

ECollisionChannel UActorInteractableComponentBase::GetCollisionChannel() const
{
	return CollisionChannel;
}

void UActorInteractableComponentBase::SetCollisionChannel(const ECollisionChannel& NewChannel)
{
}

TArray<UPrimitiveComponent*> UActorInteractableComponentBase::GetCollisionComponents() const
{
	return CollisionComponents;
}

void UActorInteractableComponentBase::AddCollisionComponent(const UPrimitiveComponent* CollisionComp)
{
}

void UActorInteractableComponentBase::AddCollisionComponents(const TArray<UPrimitiveComponent*> NewCollisionComponents)
{
}

void UActorInteractableComponentBase::RemoveCollisionComponent(const UPrimitiveComponent* CollisionComp)
{
}

void UActorInteractableComponentBase::RemoveCollisionComponents(const TArray<UPrimitiveComponent*> RemoveCollisionComponents)
{
}

TArray<UMeshComponent*> UActorInteractableComponentBase::GetHighlightableComponents() const
{
	return HighlightableMeshComponents;
}

void UActorInteractableComponentBase::AddHighlightableComponent(const UMeshComponent* HighlightableComp)
{
}

void UActorInteractableComponentBase::AddHighlightableComponents(const TArray<UMeshComponent*> HighlightableComponents)
{
}

void UActorInteractableComponentBase::RemoveHighlightableComponent(const UMeshComponent* HighlightableComp)
{
}

void UActorInteractableComponentBase::RemoveHighlightableComponents(const TArray<UMeshComponent*> HighlightableComponents)
{
}

UMeshComponent* UActorInteractableComponentBase::FindMeshByTag(const FName Tag) const
{
	return nullptr;
}

UPrimitiveComponent* UActorInteractableComponentBase::FindPrimitiveByTag(const FName Tag) const
{
	return nullptr;
}

TArray<FName> UActorInteractableComponentBase::GetCollisionOverrides() const
{
	return CollisionOverrides;
}

void UActorInteractableComponentBase::AddCollisionOverride(const FName Tag)
{
}

void UActorInteractableComponentBase::AddCollisionOverrides(const TArray<FName> Tags)
{
}

void UActorInteractableComponentBase::RemoveCollisionOverride(const FName Tag)
{
}

void UActorInteractableComponentBase::RemoveCollisionOverrides(const TArray<FName> Tags)
{
}

TArray<FName> UActorInteractableComponentBase::GetHighlightableOverrides() const
{
	return HighlightableOverrides;
}

void UActorInteractableComponentBase::AddHighlightableOverride(const FName Tag)
{
}

void UActorInteractableComponentBase::AddHighlightableOverrides(const TArray<FName> Tags)
{
}

void UActorInteractableComponentBase::RemoveHighlightableOverride(const FName Tag)
{
}

void UActorInteractableComponentBase::RemoveHighlightableOverrides(const TArray<FName> Tags)
{
}

