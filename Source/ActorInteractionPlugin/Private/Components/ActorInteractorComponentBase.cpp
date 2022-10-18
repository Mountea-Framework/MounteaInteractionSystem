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
	return ECC_Visibility;
}

void UActorInteractorComponentBase::SetResponseChannel(const ECollisionChannel NewResponseChannel)
{
}

EInteractorState UActorInteractorComponentBase::GetState() const
{
	return EInteractorState::EIS_Active;
}

void UActorInteractorComponentBase::SetState(const EInteractorState NewState)
{
}

bool UActorInteractorComponentBase::DoesAutoActivate() const
{
	return true;
}

void UActorInteractorComponentBase::SetAutoActivate(const bool bNewAutoActivate)
{
	Super::SetAutoActivate(bNewAutoActivate);
}

FKey UActorInteractorComponentBase::GetInteractionKey() const
{
	return FKey("F");
}

void UActorInteractorComponentBase::SetInteractionKey(const FKey NewInteractorKey)
{
}

