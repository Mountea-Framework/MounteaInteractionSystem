﻿// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentBase.h"

#include "Helpers/InteractionHelpers.h"

UActorInteractableComponentBase::UActorInteractableComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;

	bToggleDebug = false;
	bInteractableAutoActivate = false;
	bInteractableAutoSetup = false;

	InteractableState = EInteractableStateV2::EIS_Asleep;
	DefaultInteractableState = EInteractableStateV2::EIS_Asleep;
	InteractionWeight = 1;

	InteractionOwner = GetOwner();
}

void UActorInteractableComponentBase::BeginPlay()
{
	Super::BeginPlay();

	OnInteractorOverlapped.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableBeginOverlapEvent);
	OnInteractorStopOverlap.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableStopOverlapEvent);
	OnInteractorTraced.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableTracedEvent);

	OnInteractableCollisionChannelChanged.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableCollisionChannelChangedEvent);

	SetState(DefaultInteractableState);

	AutoSetup();
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
	const EInteractableStateV2 CachedState = GetState();

	SetState(EInteractableStateV2::EIS_Active);

	switch (CachedState)
	{
		case EInteractableStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactable Component is already Active"));
			break;
		case EInteractableStateV2::EIS_Awake:
			ErrorMessage.Append(TEXT("Interactable Component has been Activated"));
			return true;
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Cooldown:
		case EInteractableStateV2::EIS_Completed:
		case EInteractableStateV2::EIS_Disabled:
			ErrorMessage.Append(TEXT("Interactable Component cannot be Activated"));
			break;
		case EInteractableStateV2::Default: 
		default:
			ErrorMessage.Append(TEXT("Interactable Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UActorInteractableComponentBase::WakeUpInteractable(FString& ErrorMessage)
{
	const EInteractableStateV2 CachedState = GetState();

	SetState(EInteractableStateV2::EIS_Awake);

	switch (CachedState)
	{
		case EInteractableStateV2::EIS_Awake:
			ErrorMessage.Append(TEXT("Interactable Component is already Awake"));
			break;
		case EInteractableStateV2::EIS_Active:
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Cooldown:
		case EInteractableStateV2::EIS_Disabled:
			ErrorMessage.Append(TEXT("Interactable Component has been Awaken"));
			return true;
		case EInteractableStateV2::EIS_Completed:
			ErrorMessage.Append(TEXT("Interactable Component cannot be Awaken"));
			break;
		case EInteractableStateV2::Default: 
		default:
			ErrorMessage.Append(TEXT("Interactable Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UActorInteractableComponentBase::SnoozeInteractable(FString& ErrorMessage)
{
	const EInteractableStateV2 CachedState = GetState();

	SetState(EInteractableStateV2::EIS_Asleep);

	switch (CachedState)
	{
		case EInteractableStateV2::EIS_Asleep:
			ErrorMessage.Append(TEXT("Interactable Component is already Asleep"));
			break;
		case EInteractableStateV2::EIS_Awake:
		case EInteractableStateV2::EIS_Active:
		case EInteractableStateV2::EIS_Disabled:
			ErrorMessage.Append(TEXT("Interactable Component has been Awaken"));
			return true;
		case EInteractableStateV2::EIS_Cooldown:
			// TODO: reset cooldown
			ErrorMessage.Append(TEXT("Interactable Component has been Awaken"));
			return true;
		case EInteractableStateV2::EIS_Completed:
			ErrorMessage.Append(TEXT("Interactable Component cannot be Awaken"));
			break;
		case EInteractableStateV2::Default: 
		default:
			ErrorMessage.Append(TEXT("Interactable Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UActorInteractableComponentBase::CompleteInteractable(FString& ErrorMessage)
{
	const EInteractableStateV2 CachedState = GetState();

	SetState(EInteractableStateV2::EIS_Asleep);

	switch (CachedState)
	{
		case EInteractableStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactable Component is Completed"));
			return true;
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Awake:
		case EInteractableStateV2::EIS_Disabled:
		case EInteractableStateV2::EIS_Cooldown:
			ErrorMessage.Append(TEXT("Interactable Component cannot be Completed"));
			break;
		case EInteractableStateV2::EIS_Completed:
			ErrorMessage.Append(TEXT("Interactable Component is already Completed"));
			break;
		case EInteractableStateV2::Default: 
		default:
			ErrorMessage.Append(TEXT("Interactable Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

void UActorInteractableComponentBase::DeactivateInteractable()
{
	SetState(EInteractableStateV2::EIS_Disabled);
}

EInteractableStateV2 UActorInteractableComponentBase::GetState() const
{
	return EInteractableStateV2::EIS_Asleep;
}

void UActorInteractableComponentBase::SetState(const EInteractableStateV2 NewState)
{
	switch (NewState)
	{
		case EInteractableStateV2::EIS_Active:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Awake:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Active:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Disabled:
				case EInteractableStateV2::Default:
				default: break;
			}
			break;
		case EInteractableStateV2::EIS_Awake:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Active:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Disabled:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Awake:
				case EInteractableStateV2::Default: 
				default: break;
			}
			break;
		case EInteractableStateV2::EIS_Asleep:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Active:
				case EInteractableStateV2::EIS_Awake:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Disabled:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::Default: 
				default: break;
			}
			break;
		case EInteractableStateV2::EIS_Cooldown:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Active:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Awake:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Disabled:
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::Default: 
				default: break;
			}
			break;
		case EInteractableStateV2::EIS_Completed:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Active:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Awake:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Disabled:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::Default: 
				default: break;
			}
			break;
		case EInteractableStateV2::EIS_Disabled:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Active:
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Awake:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Asleep:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Disabled:
				case EInteractableStateV2::Default: 
				default: break;
			}
			break;
		case EInteractableStateV2::Default: 
		default: break;
	}
}

TScriptInterface<IActorInteractorInterface> UActorInteractableComponentBase::GetInteractor() const
{
	return Interactor;
}

void UActorInteractableComponentBase::SetInteractor(const TScriptInterface<IActorInteractorInterface> NewInteractor)
{
	Interactor = NewInteractor;
}

int32 UActorInteractableComponentBase::GetInteractableWeight() const
{
	return InteractionWeight;
}

void UActorInteractableComponentBase::SetInteractableWeight(const int32 NewWeight)
{
	InteractionWeight = FMath::Max(0, NewWeight);

	OnInteractableWeightChanged.Broadcast(InteractionWeight);
}

AActor* UActorInteractableComponentBase::GetInteractableOwner() const
{
	return InteractionOwner;
}

void UActorInteractableComponentBase::SetInteractableOwner(AActor* NewOwner)
{
	InteractionOwner = NewOwner;
}

ECollisionChannel UActorInteractableComponentBase::GetCollisionChannel() const
{
	return CollisionChannel;
}

void UActorInteractableComponentBase::SetCollisionChannel(const ECollisionChannel& NewChannel)
{
	CollisionChannel = NewChannel;

	OnInteractableCollisionChannelChanged.Broadcast(CollisionChannel);
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

void UActorInteractableComponentBase::AutoSetup()
{
	if (!DoesAutoSetup()) return;

	/**
	 * TODO:
	 * - Add all parent Primitive Comps to Collision Shapes
	 * - Add all parent Mesh Comps to Highlightable Meshes
	 * - Bind Events
	 */
}
