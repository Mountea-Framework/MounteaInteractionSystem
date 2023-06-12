// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractorComponentOverlap.h"

UActorInteractorComponentOverlap::UActorInteractorComponentOverlap()
{
}

void UActorInteractorComponentOverlap::BeginPlay()
{
	SetupInteractorOverlap();
	Super::BeginPlay();
}

void UActorInteractorComponentOverlap::SetState(const EInteractorStateV2 NewState)
{
	Super::SetState(NewState);

	switch (GetState())
	{
		case EInteractorStateV2::EIS_Active: 
		case EInteractorStateV2::EIS_Awake:
			BindCollisions();
			break;
		case EInteractorStateV2::EIS_Asleep:
		case EInteractorStateV2::EIS_Suppressed: 
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::Default: 
		default:
			UnbindCollisions();
			break;
	}
}

UPrimitiveComponent* UActorInteractorComponentOverlap::FindComponentByName(const FName& CollisionComponentName)
{
	if (!GetOwner()) return nullptr;
	
	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), Components);

	for (const auto& Itr : Components)
	{
		if (Itr && Itr->GetName().Equals(CollisionComponentName.ToString()))
		{
			return Cast<UPrimitiveComponent>(Itr);
		}
	}

	return nullptr;
}

void UActorInteractorComponentOverlap::SetupInteractorOverlap()
{
	for (auto Itr : OverrideCollisionComponents)
	{
		if (const auto Comp = FindComponentByName(Itr))
		{
			AddCollisionComponent(Comp);
		}
	}
}

void UActorInteractorComponentOverlap::BindCollisions()
{
	for (const auto& Itr : CollisionShapes)
	{
		BindCollision(Itr);
	}
}

void UActorInteractorComponentOverlap::BindCollision(UPrimitiveComponent* Component)
{
	if (!Component) return;

	FCollisionShapeCache CachedValues;
	CachedValues.bGenerateOverlapEvents = Component->GetGenerateOverlapEvents();
	CachedValues.CollisionEnabled = Component->GetCollisionEnabled();
	CachedValues.CollisionResponse = ECR_Overlap;
	
	CachedCollisionShapesSettings.Add(Component, CachedValues);
	
	Component->SetGenerateOverlapEvents(true);
	Component->SetCollisionResponseToChannel(CollisionChannel, ECollisionResponse::ECR_Overlap);

	switch (Component->GetCollisionEnabled())
	{
		case ECollisionEnabled::NoCollision:
			Component->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			break;
		case ECollisionEnabled::QueryOnly:
		case ECollisionEnabled::PhysicsOnly:
		case ECollisionEnabled::QueryAndPhysics:
		default: break;
	}
}

void UActorInteractorComponentOverlap::UnbindCollisions()
{
	for (const auto& Itr : CollisionShapes)
	{
		UnbindCollision(Itr);
	}
}

void UActorInteractorComponentOverlap::UnbindCollision(UPrimitiveComponent* Component)
{
	if(!Component) return;

	if (CachedCollisionShapesSettings.Find(Component))
	{
		Component->SetGenerateOverlapEvents(CachedCollisionShapesSettings[Component].bGenerateOverlapEvents);
		Component->SetCollisionEnabled(CachedCollisionShapesSettings[Component].CollisionEnabled);
		Component->SetCollisionResponseToChannel(CollisionChannel, CachedCollisionShapesSettings[Component].CollisionResponse);
	}
	else
	{
		Component->SetGenerateOverlapEvents(true);
		Component->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Component->SetCollisionResponseToChannel(CollisionChannel, ECollisionResponse::ECR_Overlap);
	}
}

void UActorInteractorComponentOverlap::AddCollisionComponent(UPrimitiveComponent* CollisionComponent)
{
	if (!CollisionComponent) return;

	if (CollisionShapes.Contains(CollisionComponent)) return;

	CollisionShapes.Add(CollisionComponent);

	OnCollisionShapeAdded.Broadcast(CollisionComponent);
}

void UActorInteractorComponentOverlap::AddCollisionComponents(TArray<UPrimitiveComponent*> CollisionComponents)
{
	for (const auto& Itr : CollisionComponents)
	{
		AddCollisionComponent(Itr);
	}
}

void UActorInteractorComponentOverlap::RemoveCollisionComponent(UPrimitiveComponent* CollisionComponent)
{
	if (!CollisionComponent) return;

	if (!CollisionShapes.Contains(CollisionComponent)) return;

	CollisionShapes.Remove(CollisionComponent);

	OnCollisionShapeRemoved.Broadcast(CollisionComponent);
}

void UActorInteractorComponentOverlap::RemoveCollisionComponents(TArray<UPrimitiveComponent*> CollisionComponents)
{
	for (const auto& Itr : CollisionComponents)
	{
		RemoveCollisionComponent(Itr);
	}
}

TArray<UPrimitiveComponent*> UActorInteractorComponentOverlap::GetCollisionComponents() const
{ return CollisionShapes; }
