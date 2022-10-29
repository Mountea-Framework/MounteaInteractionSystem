// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractionComponentOverlap.h"

#include "Interfaces/ActorInteractorInterface.h"

UActorInteractionComponentOverlap::UActorInteractionComponentOverlap()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UActorInteractionComponentOverlap::BeginPlay()
{
	Super::BeginPlay();

	OnInteractorOverlapped.AddUniqueDynamic(this, &UActorInteractionComponentOverlap::OnInteractableBeginOverlapEvent);
	OnInteractorStopOverlap.AddUniqueDynamic(this, &UActorInteractionComponentOverlap::OnInteractableStopOverlapEvent);
}

void UActorInteractionComponentOverlap::OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnInteractableBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (IsInteracting()) return;
	if (!OtherActor) return;

	TArray<UActorComponent*> InteractorComponents = OtherActor->GetComponentsByInterface(UActorInteractorInterface::StaticClass());

	if (InteractorComponents.Num() == 0) return;
	
	for (const auto Itr : InteractorComponents)
	{
		TScriptInterface<IActorInteractorInterface> FoundInteractor;
		if (IgnoredClasses.Contains(Itr->StaticClass())) continue;
		
		FoundInteractor = Itr;
		FoundInteractor.SetObject(Itr);
		FoundInteractor.SetInterface(Cast<IActorInteractorInterface>(Itr));

		switch (FoundInteractor->GetState())
		{
			case EInteractorStateV2::EIS_Active:
			case EInteractorStateV2::EIS_Awake:
				if (FoundInteractor->GetResponseChannel() != GetCollisionChannel()) continue;
				OnInteractorOverlapped.Broadcast(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
				OnInteractorFound.Broadcast(FoundInteractor);
				break;
			case EInteractorStateV2::EIS_Asleep:
			case EInteractorStateV2::EIS_Suppressed:
			case EInteractorStateV2::EIS_Disabled:
			case EInteractorStateV2::Default:
				break;
		}
	}
}

void UActorInteractionComponentOverlap::OnInteractableStopOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractableStopOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (!OtherActor) return;

	TArray<UActorComponent*> InteractorComponents = OtherActor->GetComponentsByInterface(UActorInteractorInterface::StaticClass());

	if (InteractorComponents.Num() == 0) return;

	for (const auto Itr : InteractorComponents)
	{
		if (Itr == GetInteractor().GetObject())
		{
			GetInteractor()->GetOnInteractableLostHandle().Broadcast(this);
			
			OnInteractorLost.Broadcast(GetInteractor());
			
			SetInteractor(nullptr);
			
			OnInteractorStopOverlap.Broadcast(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
			
			return;
		}
	}
}

void UActorInteractionComponentOverlap::BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const
{
	Super::BindCollisionShape(PrimitiveComponent);

	if (!PrimitiveComponent) return;

	PrimitiveComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &UActorInteractionComponentOverlap::OnInteractableBeginOverlap);
	PrimitiveComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &UActorInteractionComponentOverlap::OnInteractableStopOverlap);
}

void UActorInteractionComponentOverlap::UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const
{
	Super::UnbindCollisionShape(PrimitiveComponent);

	if (!PrimitiveComponent) return;

	PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UActorInteractionComponentOverlap::OnInteractableBeginOverlap);
	PrimitiveComponent->OnComponentEndOverlap.RemoveDynamic(this, &UActorInteractionComponentOverlap::OnInteractableStopOverlap);
}