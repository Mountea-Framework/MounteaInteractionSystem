// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractionComponentTrace.h"

#include "Interfaces/ActorInteractorInterface.h"

UActorInteractionComponentTrace::UActorInteractionComponentTrace()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UActorInteractionComponentTrace::BeginPlay()
{
	Super::BeginPlay();

	OnInteractorTraced.AddUniqueDynamic(this, &UActorInteractionComponentTrace::OnInteractableTracedEvent);
}

void UActorInteractionComponentTrace::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UActorInteractionComponentTrace::OnInteractableTraced(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnInteractableTraced(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

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
				OnInteractorTraced.Broadcast(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
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

void UActorInteractionComponentTrace::BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const
{
	Super::BindCollisionShape(PrimitiveComponent);

	if (!PrimitiveComponent) return;

	PrimitiveComponent->OnComponentHit.AddUniqueDynamic(this, &UActorInteractionComponentTrace::OnInteractableTraced);
}

void UActorInteractionComponentTrace::UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const
{
	Super::UnbindCollisionShape(PrimitiveComponent);

	if (!PrimitiveComponent) return;

	PrimitiveComponent->OnComponentHit.RemoveDynamic(this, &UActorInteractionComponentTrace::OnInteractableTraced);
}
