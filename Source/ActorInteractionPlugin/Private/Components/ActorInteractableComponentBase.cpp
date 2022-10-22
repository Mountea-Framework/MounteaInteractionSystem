// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentBase.h"

#include "Helpers/InteractionHelpers.h"

UActorInteractableComponentBase::UActorInteractableComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;

	bToggleDebug = false;
	bInteractableAutoActivate = false;
	bInteractableAutoSetup = false;

	InteractableState = EInteractableState::EIS_Disabled;
	InteractionWeight = 1;
}

void UActorInteractableComponentBase::BeginPlay()
{
	Super::BeginPlay();

	OnInteractorOverlapped.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableBeginOverlapEvent);
	OnInteractorStopOverlap.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableStopOverlapEvent);
	OnInteractorTraced.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableTracedEvent);
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

void UActorInteractableComponentBase::OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/**
	 * TODO
	 * Validation
	 * If Valid, then Broadcast
	 */
	
	OnInteractorOverlapped.Broadcast(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void UActorInteractableComponentBase::OnInteractableStopOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/**
	 * TODO
	 * Validation
	 * If Valid, then Broadcast
	 */

	OnInteractorStopOverlap.Broadcast(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void UActorInteractableComponentBase::OnInteractableTraced(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	/**
	 * TODO
	 * Validation
	 * If Valid, then Broadcast
	 */

	OnInteractorTraced.Broadcast(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

void UActorInteractableComponentBase::BindCollisionEvents(UPrimitiveComponent* PrimitiveComponent) const
{
	if (!PrimitiveComponent) return;

	PrimitiveComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableBeginOverlap);
	PrimitiveComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableStopOverlap);
	PrimitiveComponent->OnComponentHit.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableTraced);
}

void UActorInteractableComponentBase::UnbindCollisionEvents(UPrimitiveComponent* PrimitiveComponent) const
{
	if(!PrimitiveComponent) return;

	PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UActorInteractableComponentBase::OnInteractableBeginOverlap);
	PrimitiveComponent->OnComponentEndOverlap.RemoveDynamic(this, &UActorInteractableComponentBase::OnInteractableStopOverlap);
	PrimitiveComponent->OnComponentHit.RemoveDynamic(this, &UActorInteractableComponentBase::OnInteractableTraced);
}
