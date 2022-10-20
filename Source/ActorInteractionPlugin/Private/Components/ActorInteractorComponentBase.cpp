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

	OnInteractableSelected.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractableSelectedEvent);
	OnInteractableFound.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractableFoundEvent);
	OnInteractableLost.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractableLostEvent);
	OnInteractionKeyPressed.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractionKeyPressedEvent);
	OnInteractionKeyReleased.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractionKeyReleasedEvent);
	OnStateChanged.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractorStateChanged);
}

void UActorInteractorComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UActorInteractorComponentBase::OnInteractableSelectedEvent_Implementation(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable)
{
	SetActiveInteractable(SelectedInteractable);
}

void UActorInteractorComponentBase::OnInteractableFoundEvent_Implementation(const TScriptInterface<IActorInteractableInterface>& FoundInteractable)
{
	if (!ListOfInteractables.Contains(FoundInteractable))
	{
		ListOfInteractables.Add(FoundInteractable);
	}

	for (const auto Itr : InteractionDependencies)
	{
		if (Itr.GetInterface())
		{
			Itr->SetState(EInteractorState::EIS_Suppressed);
		}
	}
	
	SelectInteractable();
}

void UActorInteractorComponentBase::OnInteractableLostEvent_Implementation(const TScriptInterface<IActorInteractableInterface>& LostInteractable)
{
	if (ListOfInteractables.Contains(LostInteractable))
	{
		ListOfInteractables.Remove(LostInteractable);
	}
	
	SetActiveInteractable(nullptr);
}

void UActorInteractorComponentBase::OnInteractionKeyPressedEvent_Implementation(const float TimeKeyPressed)
{
	StartInteraction();
}

void UActorInteractorComponentBase::OnInteractionKeyReleasedEvent_Implementation(const float TimeKeyReleased)
{
	StopInteraction();
}

void UActorInteractorComponentBase::SelectInteractable() const
{
	if (ListOfInteractables.Num())
	{		
		auto TempInteractable = ActiveInteractable;
		for (auto& Itr : ListOfInteractables)
		{
			/**
			 * TODO
			 * Much cleaner calculation what to choose based on weight etc.
			 */
		}

		OnInteractableSelected.Broadcast(ListOfInteractables[0]);
		return;
	}

	OnInteractableSelected.Broadcast(nullptr);
}

void UActorInteractorComponentBase::StartInteraction()
{
	if (CanInteract())
	{
		/**
		 * TODO
		 * Interaction can start here
		 */

		//ActiveInteractable->OnInteractionStarted();
	}
	else
	{
		return;
	}
}

void UActorInteractorComponentBase::StopInteraction()
{
	/**
	 * TODO
	 * Interaction must stop here
	 */

	//ActiveInteractable->OnInteractionStopped();
}

bool UActorInteractorComponentBase::ActivateInteractor(FString& ErrorMessage)
{
	const EInteractorState CachedState = GetState();
	
	SetState(EInteractorState::EIS_Active);
	
	switch (CachedState)
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

bool UActorInteractorComponentBase::WakeUpInteractor(FString& ErrorMessage)
{
	const EInteractorState CachedState = GetState();
	
	SetState(EInteractorState::EIS_StandBy);
	
	switch (CachedState)
	{
		case EInteractorState::EIS_Disabled:
		case EInteractorState::EIS_Suppressed:
			ErrorMessage.Append(TEXT("Interactor Component has been Awaken"));
			InteractorState = EInteractorState::EIS_StandBy;
			return true;
		case EInteractorState::EIS_StandBy:
		case EInteractorState::EIS_Active:
			ErrorMessage.Append(TEXT("Interactor Component is already Awake"));
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
	SetState(EInteractorState::EIS_Disabled);
}

void UActorInteractorComponentBase::AddInteractionDependency(const TScriptInterface<IActorInteractorInterface> InteractionDependency)
{
	if (InteractionDependencies.Contains(InteractionDependency))
	{
		return;
	}
	
	InteractionDependencies.Add(InteractionDependency);
}

void UActorInteractorComponentBase::RemoveInteractionDependency(const TScriptInterface<IActorInteractorInterface> InteractionDependency)
{
	if (InteractionDependencies.Contains(InteractionDependency))
	{
		InteractionDependencies.Remove(InteractionDependency);
	}
}

TArray<TScriptInterface<IActorInteractorInterface>> UActorInteractorComponentBase::GetInteractionDependencies() const
{
	return InteractionDependencies;
}

bool UActorInteractorComponentBase::CanInteract() const
{
	switch (InteractorState)
	{
		case EInteractorState::EIS_StandBy:
			return ActiveInteractable.GetInterface() != nullptr;
		case EInteractorState::EIS_Suppressed:
		case EInteractorState::EIS_Active:
		case EInteractorState::EIS_Disabled:
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
	switch (NewState)
	{
		case EInteractorState::EIS_Suppressed:
		case EInteractorState::EIS_StandBy:
		case EInteractorState::EIS_Disabled:
			InteractorState = NewState;
			OnStateChanged.Broadcast();
			break;
		case EInteractorState::EIS_Active:
			if (InteractorState == EInteractorState::EIS_StandBy)
			{
				InteractorState = NewState;
				OnStateChanged.Broadcast();
			}
			break;
		case EInteractorState::Default:
			break;
		default:
			break;
	}
}

bool UActorInteractorComponentBase::DoesAutoActivate() const
{
	return bDoesAutoActivate;
}

void UActorInteractorComponentBase::SetDoesAutoActivate(const bool bNewAutoActivate)
{
	bDoesAutoActivate = bNewAutoActivate;
}

FKey UActorInteractorComponentBase::GetInteractionKey(const FString& RequestedPlatform) const
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

void UActorInteractorComponentBase::SetInteractionKey(const FString& Platform, const FKey NewInteractorKey)
{
	InteractionKeyPerPlatform.Add(Platform, NewInteractorKey);
}

TMap<FString, FKey> UActorInteractorComponentBase::GetInteractionKeys() const
{
	return InteractionKeyPerPlatform;
}

void UActorInteractorComponentBase::SetActiveInteractable(const TScriptInterface<IActorInteractableInterface> NewInteractable)
{
	ActiveInteractable = NewInteractable;
}

TScriptInterface<IActorInteractableInterface> UActorInteractorComponentBase::GetActiveInteractable() const
{
	return ActiveInteractable;
}

