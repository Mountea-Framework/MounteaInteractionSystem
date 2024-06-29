// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractorComponentOverlap.h"

#include "Helpers/ActorInteractionPluginLog.h"
#include "Helpers/MounteaInteractionSystemBFL.h"
#include "Interfaces/ActorInteractableInterface.h"

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

void UActorInteractorComponentOverlap::ProcessOverlap_Server_Implementation(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& SweepResult, const bool bOverlapStarted)
{
	ProcessOverlap(OverlappedComponent, OtherActor, OtherComp, SweepResult, bOverlapStarted);
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

	// TODO: Bind Overlap and End Overlap here!

	FCollisionShapeCache CachedValues;
	CachedValues.bGenerateOverlapEvents = Component->GetGenerateOverlapEvents();
	CachedValues.CollisionEnabled = Component->GetCollisionEnabled();
	CachedValues.CollisionResponse = ECR_Overlap;
	
	CachedCollisionShapesSettings.Add(Component, CachedValues);
	
	Component->SetGenerateOverlapEvents(true);
	Component->SetCollisionResponseToChannel(CollisionChannel, ECollisionResponse::ECR_Overlap);

	Component->OnComponentBeginOverlap.		AddUniqueDynamic(this, &UActorInteractorComponentOverlap::StartInteractorOverlap);
	Component->OnComponentEndOverlap.		AddUniqueDynamic(this, &UActorInteractorComponentOverlap::StopInteractorOverlap);

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

	Component->OnComponentBeginOverlap.		RemoveDynamic(this, &UActorInteractorComponentOverlap::StartInteractorOverlap);
	Component->OnComponentEndOverlap.		RemoveDynamic(this, &UActorInteractorComponentOverlap::StopInteractorOverlap);
}

void UActorInteractorComponentOverlap::ProcessOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& SweepResult, const bool bOverlapStarted)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[ProcessOverlap] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (!Execute_CanInteract(this))
		{
			return;
		}
		
		if (!OtherActor)
		{
			return;
		}

		if (!OtherActor->Implements<UActorInteractableInterface>())
		{
			return;
		}

		if (bOverlapStarted)
		{
			HandleStartOverlap(OverlappedComponent, OtherActor, OtherComp, SweepResult);
		}
		else
		{
			HandleEndOverlap(OverlappedComponent, OtherActor, OtherComp);
		}
	}
	else
	{
		ProcessOverlap_Server(OverlappedComponent, OtherActor, OtherComp, SweepResult, bOverlapStarted);
	}
}

void UActorInteractorComponentOverlap::StartInteractorOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[StartInteractorOverlap] No Owner!"))
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		ProcessOverlap(OverlappedComponent, OtherActor, OtherComp, SweepResult, true);
	}
	else
	{
		StartInteractorOverlap_Server(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
}

void UActorInteractorComponentOverlap::StopInteractorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[StopInteractorOverlap] No Owner!"))
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		ProcessOverlap(OverlappedComponent, OtherActor, OtherComp, FHitResult(), false);
	}
	else
	{
		StopInteractorOverlap_Server(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	}
}

void UActorInteractorComponentOverlap::HandleStartOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& HitResult)
{
	if (!OtherActor)
	{
		LOG_ERROR(TEXT("[HandleStartOverlap] OtherActor is null!"));
		return;
	}
	
	if (!PrimitiveComponent)
	{
		LOG_ERROR(TEXT("[HandleStartOverlap] PrimitiveComponent is null!"));
		return;
	}

	if (!OtherComp)
	{
		LOG_ERROR(TEXT("[HandleStartOverlap] OtherComp is null!"));
		return;
	}
	
	TScriptInterface<IActorInteractableInterface> currentlyActiveInteractable = Execute_GetActiveInteractable(this);
	TScriptInterface<IActorInteractableInterface> tempInteractable = nullptr;
	
	TArray<UActorComponent*> interactableComponents = OtherActor->GetComponentsByInterface(UActorInteractableInterface::StaticClass());
	int32 highestWeight = -1;

	for (const auto& Component : interactableComponents)
	{
		TScriptInterface<IActorInteractableInterface> InteractableComponent = TScriptInterface<IActorInteractableInterface>(Component);

		if (!InteractableComponent->Execute_CanInteract(Component))
		{
			continue;
		}

		ECollisionChannel componentCollisionChannel = InteractableComponent->Execute_GetCollisionChannel(Component);
		if (componentCollisionChannel != Execute_GetResponseChannel(this))
		{
			continue;
		}

		if (PrimitiveComponent->GetCollisionResponseToChannel(componentCollisionChannel) == ECR_Ignore)
		{
			continue;
		}

		if (!InteractableComponent->Execute_CanBeTriggered(InteractableComponent.GetObject()))
		{
			continue;
		}

		int32 ComponentWeight = InteractableComponent->Execute_GetInteractableWeight(Component);
		if (ComponentWeight > highestWeight)
		{
			highestWeight = ComponentWeight;
			tempInteractable = InteractableComponent;
		}
	}

	if (!tempInteractable.GetObject())
	{
		LOG_WARNING(TEXT("[HandleStartOverlap] No valid interactable components found!"));
		return;
	}

	if (currentlyActiveInteractable.GetObject())
	{
		if (currentlyActiveInteractable.GetObject() == OtherActor)
		{
			LOG_INFO(TEXT("[HandleStartOverlap] Currently active interactable is the same as the actor!"));
			return;
		}
		
		int32 localWeight	= tempInteractable->					Execute_GetInteractableWeight(OtherActor);
		int32 activeWeight	= currentlyActiveInteractable->	Execute_GetInteractableWeight(currentlyActiveInteractable.GetObject());

		if (localWeight < activeWeight)
		{
			LOG_INFO(TEXT("[HandleStartOverlap] Local interactable weight is less than active interactable weight!"));
			return;
		}
	}

	// TODO: Do one last safety check and line trace from HitResult (or GetOwner) towards OtherActor based on Visibility to avoid overlapping through walls
	// if 
}


void UActorInteractorComponentOverlap::HandleEndOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	
}

void UActorInteractorComponentOverlap::StartInteractorOverlap_Server_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	StartInteractorOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void UActorInteractorComponentOverlap::StopInteractorOverlap_Server_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	StopInteractorOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
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
