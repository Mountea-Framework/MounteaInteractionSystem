// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractorComponentOverlap.h"

#include "Helpers/ActorInteractionPluginLog.h"
#include "Helpers/MounteaInteractionSystemBFL.h"

UActorInteractorComponentOverlap::UActorInteractorComponentOverlap()
{
}

void UActorInteractorComponentOverlap::BeginPlay()
{
	SetupInteractorOverlap();
	Super::BeginPlay();
}

void UActorInteractorComponentOverlap::ProcessStateChanges()
{
	Super::ProcessStateChanges();

	switch (Execute_GetState(this))
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

void UActorInteractorComponentOverlap::SetupInteractorOverlap()
{
	for (auto Itr : OverrideCollisionComponents)
	{
		if (const auto Comp = UMounteaInteractionSystemBFL::FindPrimitiveByName(Itr, GetOwner()))
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

void UActorInteractorComponentOverlap::AddCollisionComponent_Implementation(UPrimitiveComponent* CollisionComponent)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[AddCollisionComponent] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (!CollisionComponent) return;

		if (CollisionShapes.Contains(CollisionComponent)) return;

		CollisionShapes.Add(CollisionComponent);

		OnCollisionShapeAdded.Broadcast(CollisionComponent);
	}
	else
	{
		AddCollisionComponent_Server(CollisionComponent);
	}
}

void UActorInteractorComponentOverlap::AddCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[AddCollisionComponents] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		for (const auto& Itr : CollisionComponents)
		{
			AddCollisionComponent(Itr);
		}
	}
	else
	{
		AddCollisionComponents_Server(CollisionComponents);
	}
}

void UActorInteractorComponentOverlap::RemoveCollisionComponent_Implementation(UPrimitiveComponent* CollisionComponent)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[RemoveCollisionComponent] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (!CollisionComponent) return;

		if (!CollisionShapes.Contains(CollisionComponent)) return;

		CollisionShapes.Remove(CollisionComponent);

		OnCollisionShapeRemoved.Broadcast(CollisionComponent);
	}
	else
	{
		RemoveCollisionComponent_Server(CollisionComponent);
	}
}

void UActorInteractorComponentOverlap::RemoveCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[RemoveCollisionComponents] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		for (const auto& Itr : CollisionComponents)
		{
			RemoveCollisionComponent(Itr);
		}
	}
	else
	{
		RemoveCollisionComponents_Server(CollisionComponents);
	}
}

void UActorInteractorComponentOverlap::AddCollisionComponent_Server_Implementation(UPrimitiveComponent* CollisionComponent)
{
	AddCollisionComponent(CollisionComponent);
}

void UActorInteractorComponentOverlap::AddCollisionComponents_Server_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents)
{
	AddCollisionComponents(CollisionComponents);
}

void UActorInteractorComponentOverlap::RemoveCollisionComponent_Server_Implementation(UPrimitiveComponent* CollisionComponent)
{
	RemoveCollisionComponent(CollisionComponent);
}

void UActorInteractorComponentOverlap::RemoveCollisionComponents_Server_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents)
{
	RemoveCollisionComponents(CollisionComponents);
}

TSet<UPrimitiveComponent*> UActorInteractorComponentOverlap::GetCollisionComponents() const
{
	return CollisionShapes;
}
