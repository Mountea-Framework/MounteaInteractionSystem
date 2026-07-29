// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Components/Interactor/MounteaInteractorComponentOverlap.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Helpers/MounteaInteractionStatics.h"
#include "Helpers/MounteaInteractionSystemSettings.h"
#include "Settings/MounteaInteractorRuntimeConfig.h"

#define LOCTEXT_NAMESPACE "MounteaInteractorComponentOverlap"

UMounteaInteractorComponentOverlap::UMounteaInteractorComponentOverlap()
{
}

void UMounteaInteractorComponentOverlap::BeginPlay()
{
	Super::BeginPlay();
	SetupInteractorOverlap();
}

void UMounteaInteractorComponentOverlap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMounteaInteractorComponentOverlap, CollisionShapes);
}

FString UMounteaInteractorComponentOverlap::ToString_Implementation() const
{
	return FString::Printf(TEXT("%s | Shapes: %d"), *Super::ToString_Implementation(), CollisionShapes.Num());
}

#pragma region Setup

void UMounteaInteractorComponentOverlap::SetupInteractorOverlap()
{
	if (!GetOwner())
				return;

	if (OverrideCollisionComponents.IsEmpty())
	{
		TArray<UPrimitiveComponent*> prims;
		GetOwner()->GetComponents<UPrimitiveComponent>(prims);
		AddCollisionComponents_Implementation(prims);
	}
	else
	{
		TArray<UPrimitiveComponent*> prims;
		GetOwner()->GetComponents<UPrimitiveComponent>(prims);
		for (UPrimitiveComponent* prim : prims)
		{
			if (IsValid(prim) && OverrideCollisionComponents.Contains(prim->GetFName()))
			{
				AddCollisionComponent_Implementation(prim);
			}
		}
	}
}

void UMounteaInteractorComponentOverlap::BindCollisions()
{
	for (UPrimitiveComponent* Prim : CollisionShapes)
	{
		BindCollision(Prim);
	}
}

void UMounteaInteractorComponentOverlap::BindCollision(UPrimitiveComponent* Component)
{
	if (!IsValid(Component))
				return;
	Component->OnComponentBeginOverlap.AddUniqueDynamic(this, &UMounteaInteractorComponentOverlap::StartInteractorOverlap);
	Component->OnComponentEndOverlap.AddUniqueDynamic(this, &UMounteaInteractorComponentOverlap::StopInteractorOverlap);
}

void UMounteaInteractorComponentOverlap::UnbindCollisions()
{
	for (UPrimitiveComponent* Prim : CollisionShapes)
	{
		UnbindCollision(Prim);
	}
}

void UMounteaInteractorComponentOverlap::UnbindCollision(UPrimitiveComponent* Component)
{
	if (!IsValid(Component))
				return;
	Component->OnComponentBeginOverlap.RemoveDynamic(this, &UMounteaInteractorComponentOverlap::StartInteractorOverlap);
	Component->OnComponentEndOverlap.RemoveDynamic(this, &UMounteaInteractorComponentOverlap::StopInteractorOverlap);
}

#pragma endregion

#pragma region CollisionManagement

void UMounteaInteractorComponentOverlap::AddCollisionComponent_Implementation(UPrimitiveComponent* CollisionComponent)
{
	if (!IsValid(CollisionComponent) || CollisionShapes.Contains(CollisionComponent))
				return;

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		AddCollisionComponent_Server(CollisionComponent);
		return;
	}

	FCollisionShapeCache cache;
	cache.bGenerateOverlapEvents = CollisionComponent->GetGenerateOverlapEvents();
	cache.CollisionEnabled       = CollisionComponent->GetCollisionEnabled();
	cache.ResponseContainer      = CollisionComponent->GetCollisionResponseToChannels();
	CachedCollisionShapesSettings.Add(CollisionComponent, cache);

	// Resolve the overlap channel from the interactor runtime config
	FName overlapChannelName = TEXT("InteractionOverlap");
	if (const UMounteaInteractionSystemSettings* settings = GetDefault<UMounteaInteractionSystemSettings>())
	{
		if (const UMounteaInteractorRuntimeConfig* cfg = settings->GetInteractorConfig())
			overlapChannelName = cfg->OverlapCollisionChannelName;
	}

	bool bFound = false;
	const ECollisionChannel overlapChannel = UMounteaInteractionStatics::ResolveInteractionChannel(overlapChannelName, bFound);

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	if (bFound) CollisionComponent->SetCollisionResponseToChannel(overlapChannel, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);

	CollisionShapes.Add(CollisionComponent);
	BindCollision(CollisionComponent);

	OnCollisionShapeAdded.Broadcast(CollisionComponent);
}

void UMounteaInteractorComponentOverlap::AddCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents)
{
	for (UPrimitiveComponent* comp : CollisionComponents)
	{
		AddCollisionComponent_Implementation(comp);
	}
}

void UMounteaInteractorComponentOverlap::RemoveCollisionComponent_Implementation(UPrimitiveComponent* CollisionComponent)
{
	if (!IsValid(CollisionComponent) || !CollisionShapes.Contains(CollisionComponent))
				return;

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		RemoveCollisionComponent_Server(CollisionComponent);
		return;
	}

	UnbindCollision(CollisionComponent);

	if (const FCollisionShapeCache* cache = CachedCollisionShapesSettings.Find(CollisionComponent))
	{
		CollisionComponent->SetCollisionEnabled(cache->CollisionEnabled.GetValue());
		CollisionComponent->SetGenerateOverlapEvents(cache->bGenerateOverlapEvents);
		CollisionComponent->SetCollisionResponseToChannels(cache->ResponseContainer);
		CachedCollisionShapesSettings.Remove(CollisionComponent);
	}

	CollisionShapes.Remove(CollisionComponent);
	OnCollisionShapeRemoved.Broadcast(CollisionComponent);
}

void UMounteaInteractorComponentOverlap::RemoveCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents)
{
	for (UPrimitiveComponent* comp : CollisionComponents)
	{
		RemoveCollisionComponent_Implementation(comp);
	}
}

TArray<UPrimitiveComponent*> UMounteaInteractorComponentOverlap::GetCollisionComponents() const
{
	TArray<UPrimitiveComponent*> out;
	for (auto p : CollisionShapes) { if (IsValid(p)) out.Add(p); }
	return out;
}

#pragma endregion

#pragma region OverlapCallbacks

void UMounteaInteractorComponentOverlap::StartInteractorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Execute_CanInteract(this))
				return;

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		StartInteractorOverlap_Server(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		return;
	}

	HandleStartOverlap(OtherActor);
}

void UMounteaInteractorComponentOverlap::StopInteractorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		StopInteractorOverlap_Server(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
		return;
	}

	HandleEndOverlap(OtherActor);
}

void UMounteaInteractorComponentOverlap::HandleStartOverlap(AActor* OtherActor)
{
	if (!IsValid(OtherActor))
				return;

	TArray<UActorComponent*> interactableComps;
	OtherActor->GetComponents(interactableComps);

	for (UActorComponent* comp : interactableComps)
	{
		if (!IsValid(comp) || !comp->GetClass()->ImplementsInterface(UMounteaInteractableInterface::StaticClass()))
				continue;

		TScriptInterface<IMounteaInteractableInterface> interactable;
		interactable.SetObject(comp);
		interactable.SetInterface(Cast<IMounteaInteractableInterface>(comp));

		AddDetected(interactable);
	}
}

void UMounteaInteractorComponentOverlap::HandleEndOverlap(AActor* OtherActor)
{
	if (!IsValid(OtherActor))
				return;

	TArray<UActorComponent*> interactableComps;
	OtherActor->GetComponents(interactableComps);

	for (UActorComponent* comp : interactableComps)
	{
		if (!IsValid(comp) || !comp->GetClass()->ImplementsInterface(UMounteaInteractableInterface::StaticClass()))
				continue;

		TScriptInterface<IMounteaInteractableInterface> interactable;
		interactable.SetObject(comp);
		interactable.SetInterface(Cast<IMounteaInteractableInterface>(comp));

		RemoveDetected(interactable);
	}
}

#pragma endregion

#pragma region Networking

void UMounteaInteractorComponentOverlap::AddCollisionComponent_Server_Implementation(UPrimitiveComponent* CollisionComponent)
{
	AddCollisionComponent_Implementation(CollisionComponent);
}

void UMounteaInteractorComponentOverlap::RemoveCollisionComponent_Server_Implementation(UPrimitiveComponent* CollisionComponent)
{
	RemoveCollisionComponent_Implementation(CollisionComponent);
}

void UMounteaInteractorComponentOverlap::StartInteractorOverlap_Server_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleStartOverlap(OtherActor);
}

void UMounteaInteractorComponentOverlap::StopInteractorOverlap_Server_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	HandleEndOverlap(OtherActor);
}

#pragma endregion

void UMounteaInteractorComponentOverlap::ProcessStateChanged()
{
	Super::ProcessStateChanged();

	const bool bShouldBeActive =
		InteractorState == EInteractorStateV3::EIS_Scanning ||
		InteractorState == EInteractorStateV3::EIS_Active;

	if (!bShouldBeActive)
	{
		ClearDetected();
		UnbindCollisions();
	}
	else
	{
		BindCollisions();
	}
}

#undef LOCTEXT_NAMESPACE
