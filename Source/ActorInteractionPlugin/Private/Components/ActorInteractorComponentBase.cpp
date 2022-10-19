// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractorComponentBase.h"

#include "Helpers/InteractionHelpers.h"

UActorInteractorComponentBase::UActorInteractorComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UActorInteractorComponentBase::BeginPlay()
{
	Super::BeginPlay();

	OnInteractableFound.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractableFoundEvent);
	OnInteractableLost.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractableLostEvent);
	OnInteractionKeyPressed.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractionKeyPressedEvent);
	OnInteractionKeyReleased.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractionKeyReleasedEvent);
}

void UActorInteractorComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UActorInteractorComponentBase::OnInteractableFoundEvent_Implementation(const TScriptInterface<IActorInteractableInterface>& FoundInteractable)
{
	SetActiveInteractable(FoundInteractable);
}

void UActorInteractorComponentBase::OnInteractableLostEvent_Implementation(const TScriptInterface<IActorInteractableInterface>& LostInteractable)
{
	SetActiveInteractable(nullptr);
}

void UActorInteractorComponentBase::StartInteraction()
{
}

void UActorInteractorComponentBase::StopInteraction()
{
}

bool UActorInteractorComponentBase::ActivateInteractor(FString& ErrorMessage)
{
	return true;
}

void UActorInteractorComponentBase::DeactivateInteractor()
{
}

void UActorInteractorComponentBase::TickInteraction(const float DeltaTime)
{
}

ECollisionChannel UActorInteractorComponentBase::GetResponseChannel() const
{
	return CollisionChannel;
}

void UActorInteractorComponentBase::SetResponseChannel(const ECollisionChannel NewResponseChannel)
{
	CollisionChannel = NewResponseChannel;
}

EInteractorState UActorInteractorComponentBase::GetState() const
{
	return InteractorState;
}

void UActorInteractorComponentBase::SetState(const EInteractorState NewState)
{
	InteractorState = NewState;
}

bool UActorInteractorComponentBase::DoesAutoActivate() const
{
	return bDoesAutoActivate;
}

void UActorInteractorComponentBase::SetDoesAutoActivate(const bool bNewAutoActivate)
{
	bDoesAutoActivate = bNewAutoActivate;
}

FKey UActorInteractorComponentBase::GetInteractionKey() const
{
	return InteractionKey;
}

void UActorInteractorComponentBase::SetInteractionKey(const FKey NewInteractorKey)
{
	InteractionKey = NewInteractorKey;
}

void UActorInteractorComponentBase::SetActiveInteractable(const TScriptInterface<IActorInteractableInterface>& NewInteractable)
{
	ActiveInteractable = NewInteractable;
}

TScriptInterface<IActorInteractableInterface> UActorInteractorComponentBase::GetActiveInteractable() const
{
	return ActiveInteractable;
}

