// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractorComponentBase.h"

#include "Helpers/InteractionHelpers.h"

UActorInteractorComponentBase::UActorInteractorComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;

	InteractionKeyPerPlatform.Add((TEXT("Windows")), FKey("E"));
	InteractionKeyPerPlatform.Add((TEXT("Mac")), FKey("E"));
	InteractionKeyPerPlatform.Add((TEXT("PS4")), FKey("Gamepad Face Button Down"));
	InteractionKeyPerPlatform.Add((TEXT("XboxOne")), FKey("Gamepad Face Button Down"));
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
	if (CanInteract())
	{
		// TODO
	}
	else
	{
		return;
	}
}

void UActorInteractorComponentBase::StopInteraction()
{
	// TODO
}

bool UActorInteractorComponentBase::ActivateInteractor(FString& ErrorMessage)
{
	switch (InteractorState)
	{
		case EInteractorState::EIS_Disabled:
		case EInteractorState::EIS_Suppressed:
			ErrorMessage.Append(TEXT("Interactor Component has been Activated"));
			InteractorState = EInteractorState::EIS_StandBy;
			return true;
		case EInteractorState::EIS_StandBy:
		case EInteractorState::EIS_Active:
			ErrorMessage.Append(TEXT("Interactor Component is already Active"));
			break;
		case EInteractorState::Default:
		default:
			ErrorMessage.Append(TEXT("Interactor Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

void UActorInteractorComponentBase::DeactivateInteractor()
{
	InteractorState = EInteractorState::EIS_Disabled;
}

bool UActorInteractorComponentBase::CanInteract() const
{
	switch (InteractorState)
	{
		case EInteractorState::EIS_Disabled:
			return false;
		case EInteractorState::EIS_StandBy:
		case EInteractorState::EIS_Suppressed:
			return true;
		case EInteractorState::EIS_Active:
			return false;
		case EInteractorState::Default:
		default:
			return false;
	}
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

FKey UActorInteractorComponentBase::GetInteractionKey(FString& RequestedPlatform) const
{
	if(InteractionKeyPerPlatform.Find(RequestedPlatform))
	{
		return *InteractionKeyPerPlatform.Find(RequestedPlatform);
	}
	else
	{
		return FKey();
	}
}

void UActorInteractorComponentBase::SetInteractionKey(FString& Platform, const FKey NewInteractorKey)
{
	InteractionKeyPerPlatform.Add(Platform, NewInteractorKey);
}

TMap<FString, FKey> UActorInteractorComponentBase::GetInteractionKeys() const
{
	return InteractionKeyPerPlatform;
}

void UActorInteractorComponentBase::SetActiveInteractable(const TScriptInterface<IActorInteractableInterface>& NewInteractable)
{
	ActiveInteractable = NewInteractable;
}

TScriptInterface<IActorInteractableInterface> UActorInteractorComponentBase::GetActiveInteractable() const
{
	return ActiveInteractable;
}

