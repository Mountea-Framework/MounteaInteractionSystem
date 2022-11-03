// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractorComponentBase.h"

#if WITH_EDITOR
#include "Helpers/ActorInteractionPluginLog.h"
#endif

#include "Helpers/InteractionHelpers.h"
#include "Interfaces/ActorInteractableInterface.h"

UActorInteractorComponentBase::UActorInteractorComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;

	bDoesAutoActivate = false;
	bToggleDebug = false;

	InteractorState = EInteractorStateV2::EIS_Asleep;
	DefaultInteractorState = EInteractorStateV2::EIS_Asleep;
}

void UActorInteractorComponentBase::BeginPlay()
{
	Super::BeginPlay();
	
	OnInteractableSelected.AddUniqueDynamic(this, &UActorInteractorComponentBase::InteractableSelected);
	OnInteractableFound.AddUniqueDynamic(this, &UActorInteractorComponentBase::InteractableFound);
	OnInteractableLost.AddUniqueDynamic(this, &UActorInteractorComponentBase::InteractableLost);
	
	//OnInteractionKeyPressed.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractionKeyPressedEvent);
	//OnInteractionKeyReleased.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractionKeyReleasedEvent);
	
	OnStateChanged.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractorStateChanged);
	OnCollisionChanged.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractorCollisionChanged);
	OnAutoActivateChanged.AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractorAutoActivateChanged);

	if (bDoesAutoActivate)
	{
		SetState(EInteractorStateV2::EIS_Awake);
	}
	else
	{
		SetState(DefaultInteractorState);
	}
}

void UActorInteractorComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickInteraction(DeltaTime);
}

void UActorInteractorComponentBase::InteractableSelected(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable)
{
	OnInteractableSelectedEvent(SelectedInteractable);
}

void UActorInteractorComponentBase::InteractableFound(const TScriptInterface<IActorInteractableInterface>& FoundInteractable)
{
	for (const auto Itr : InteractionDependencies)
	{
		if (Itr.GetInterface())
		{
			Itr->SetState(EInteractorStateV2::EIS_Suppressed);
		}
	}
	
	EvaluateInteractable(FoundInteractable);

	OnInteractableFoundEvent(FoundInteractable);
}

void UActorInteractorComponentBase::InteractableLost(const TScriptInterface<IActorInteractableInterface>& LostInteractable)
{
	if (LostInteractable == ActiveInteractable)
	{
		SetState(EInteractorStateV2::EIS_Awake);
		
		SetActiveInteractable(nullptr);

		for (const auto Itr : InteractionDependencies)
		{
			if (Itr.GetInterface())
			{
				Itr->SetState(InteractorState);
			}
		}
	}

	OnInteractableLostEvent(LostInteractable);
}

void UActorInteractorComponentBase::OnInteractionKeyPressedEvent_Implementation(const float& TimeKeyPressed, const FKey& PressedKey)
{
	StartInteraction(TimeKeyPressed, PressedKey);
}

void UActorInteractorComponentBase::OnInteractionKeyReleasedEvent_Implementation(const float& TimeKeyReleased, const FKey& ReleasedKey)
{
	StopInteraction();
}

bool UActorInteractorComponentBase::IsValidInteractor() const
{
	switch (InteractorState)
	{
		case EInteractorStateV2::EIS_Awake:
		case EInteractorStateV2::EIS_Asleep:
		case EInteractorStateV2::EIS_Active:
			return true;
		case EInteractorStateV2::EIS_Suppressed:
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::Default:
		default:
			return false;
	}
}

void UActorInteractorComponentBase::EvaluateInteractable(const TScriptInterface<IActorInteractableInterface>& FoundInteractable)
{
	if (!IsValidInteractor()) return;
	
	if (FoundInteractable.GetInterface() == nullptr)
	{
		InteractableLost(ActiveInteractable);
		return;
	}
	
	if (ActiveInteractable.GetInterface() == nullptr)
	{
		SetActiveInteractable(FoundInteractable);
		OnInteractableSelected.Broadcast(FoundInteractable);

		return;
	}
	
	if (ActiveInteractable != FoundInteractable)
	{
		if (FoundInteractable->GetInteractableWeight() > ActiveInteractable->GetInteractableWeight())
		{
			SetActiveInteractable(FoundInteractable);
			OnInteractableSelected.Broadcast(FoundInteractable);

			return;
		}
	}
	else
	{
		//SetActiveInteractable(FoundInteractable);
		//OnInteractableSelected.Broadcast(FoundInteractable);
	}
}

void UActorInteractorComponentBase::StartInteraction(const float StartTime, FKey InputKey)
{
	if (CanInteract())
	{
		ActiveInteractable->GetOnInteractionStartedHandle().Broadcast(StartTime, InputKey);
	}
}

void UActorInteractorComponentBase::StopInteraction()
{
	/**
	 * TODO
	 * Interaction must stop here
	 */

	if (CanInteract())
	{
		//ActiveInteractable->OnInteractionStopped();
	}
}

bool UActorInteractorComponentBase::ActivateInteractor(FString& ErrorMessage)
{
	const EInteractorStateV2 CachedState = GetState();
	
	SetState(EInteractorStateV2::EIS_Active);
	
	switch (CachedState)
	{
		case EInteractorStateV2::EIS_Awake:
			ErrorMessage.Append(TEXT("Interactor Component has been Activated"));
			return true;
		case EInteractorStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactor Component is already Active"));
			break;
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::EIS_Suppressed:
		case EInteractorStateV2::EIS_Asleep:
			ErrorMessage.Append(TEXT("Interactor Component cannot be Activated"));
			break;
		case EInteractorStateV2::Default:
		default:
			ErrorMessage.Append(TEXT("Interactor Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UActorInteractorComponentBase::WakeUpInteractor(FString& ErrorMessage)
{
	const EInteractorStateV2 CachedState = GetState();
	
	SetState(EInteractorStateV2::EIS_Awake);
	
	switch (CachedState)
	{
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::EIS_Asleep:
		case EInteractorStateV2::EIS_Suppressed:
		case EInteractorStateV2::EIS_Awake:
			ErrorMessage.Append(TEXT("Interactor Component has been Awaken"));
			return true;
		case EInteractorStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactor Component is Awaken and no longer Active"));
			return true;
		case EInteractorStateV2::Default:
		default:
			ErrorMessage.Append(TEXT("Interactor Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UActorInteractorComponentBase::SuppressInteractor(FString& ErrorMessage)
{
	const EInteractorStateV2 CachedState = GetState();
	
	SetState(EInteractorStateV2::EIS_Suppressed);

	switch (CachedState)
	{
		case EInteractorStateV2::EIS_Asleep:
			ErrorMessage.Append(TEXT("Interactor Component is Asleep, cannot be Suppressed"));
			break;
		case EInteractorStateV2::EIS_Disabled:
			ErrorMessage.Append(TEXT("Interactor Component is Inactive, cannot be Suppressed"));
			break;
		case EInteractorStateV2::EIS_Suppressed:
			ErrorMessage.Append(TEXT("Interactor Component is already Suppressed"));
			break;
		case EInteractorStateV2::EIS_Awake:
		case EInteractorStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactor Component has been Suppressed"));
			return true;
		case EInteractorStateV2::Default:
		default:
			ErrorMessage.Append(TEXT("Interactor Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

void UActorInteractorComponentBase::DeactivateInteractor()
{
	SetState(EInteractorStateV2::EIS_Disabled);
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

void UActorInteractorComponentBase::ProcessDependencies()
{
	if (InteractionDependencies.Num() == 0) return;
	
	for (const auto Itr : InteractionDependencies)
	{
		switch (InteractorState)
		{
			case EInteractorStateV2::EIS_Active:
			case EInteractorStateV2::EIS_Suppressed:
				Itr->SetState(EInteractorStateV2::EIS_Suppressed);
				break;
			case EInteractorStateV2::EIS_Disabled:
			case EInteractorStateV2::EIS_Awake:
			case EInteractorStateV2::EIS_Asleep:
				Itr->SetState(EInteractorStateV2::EIS_Awake);
				RemoveInteractionDependency(Itr);
				break;
			case EInteractorStateV2::Default:
			default:
				break;
		}
	}
}

bool UActorInteractorComponentBase::CanInteractEvent_Implementation() const
{
	return CanInteract();
}

bool UActorInteractorComponentBase::CanInteract() const
{
	switch (InteractorState)
	{
		case EInteractorStateV2::EIS_Awake: return ActiveInteractable.GetInterface() != nullptr;;
		case EInteractorStateV2::EIS_Asleep:
		case EInteractorStateV2::EIS_Suppressed:
		case EInteractorStateV2::EIS_Active:
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::Default:
		default: break;
	}

	return false;
}

void UActorInteractorComponentBase::TickInteraction(const float DeltaTime)
{
	TickInteractionEvent(DeltaTime);
}

ECollisionChannel UActorInteractorComponentBase::GetResponseChannel() const
{
	return CollisionChannel;
}

void UActorInteractorComponentBase::SetResponseChannel(const ECollisionChannel NewResponseChannel)
{
	CollisionChannel = NewResponseChannel;

	OnCollisionChanged.Broadcast(NewResponseChannel);
}

EInteractorStateV2 UActorInteractorComponentBase::GetState() const
{
	return InteractorState;
}

void UActorInteractorComponentBase::SetState(const EInteractorStateV2 NewState)
{
	switch (NewState)
	{
		case EInteractorStateV2::EIS_Awake:
			switch (InteractorState)
			{
				case EInteractorStateV2::EIS_Asleep:
				case EInteractorStateV2::EIS_Disabled:
				case EInteractorStateV2::EIS_Suppressed:
				case EInteractorStateV2::EIS_Active:
					InteractorState = NewState;
					OnStateChanged.Broadcast(InteractorState);
					break;
				case EInteractorStateV2::EIS_Awake:
				case EInteractorStateV2::Default:
				default: break;
			}
			break;
		case EInteractorStateV2::EIS_Asleep:
			switch (InteractorState)
			{
				case EInteractorStateV2::EIS_Awake:
				case EInteractorStateV2::EIS_Suppressed:
				case EInteractorStateV2::EIS_Active:
				case EInteractorStateV2::EIS_Disabled:
					InteractorState = NewState;
					OnStateChanged.Broadcast(InteractorState);
					break;
				case EInteractorStateV2::EIS_Asleep:
				case EInteractorStateV2::Default:
				default: break;
			}
			break;
		case EInteractorStateV2::EIS_Suppressed:
			switch (InteractorState)
			{
				case EInteractorStateV2::EIS_Awake:
				case EInteractorStateV2::EIS_Asleep:
				case EInteractorStateV2::EIS_Active:
					InteractorState = NewState;
					OnStateChanged.Broadcast(InteractorState);
					break;
				case EInteractorStateV2::EIS_Suppressed:
				case EInteractorStateV2::EIS_Disabled:
				case EInteractorStateV2::Default:
				default: break;
			}
			break;
		case EInteractorStateV2::EIS_Active:
			switch (InteractorState)
			{
				case EInteractorStateV2::EIS_Awake:
					InteractorState = NewState;
					OnStateChanged.Broadcast(InteractorState);
					break;
				case EInteractorStateV2::EIS_Asleep:
				case EInteractorStateV2::EIS_Active:
				case EInteractorStateV2::EIS_Suppressed:
				case EInteractorStateV2::EIS_Disabled:
				case EInteractorStateV2::Default:
				default: break;
			}
			break;
		case EInteractorStateV2::EIS_Disabled:
			switch (InteractorState)
			{
				case EInteractorStateV2::EIS_Asleep:
				case EInteractorStateV2::EIS_Awake:
				case EInteractorStateV2::EIS_Suppressed:
				case EInteractorStateV2::EIS_Active:
					InteractorState = NewState;
					OnStateChanged.Broadcast(InteractorState);
					break;
				case EInteractorStateV2::EIS_Disabled:
				case EInteractorStateV2::Default:
				default: break;
			}
			break;
		case EInteractorStateV2::Default:
		default: break;
	}
}

bool UActorInteractorComponentBase::DoesAutoActivate() const
{
	return bDoesAutoActivate;
}

void UActorInteractorComponentBase::ToggleAutoActivate(const bool bNewAutoActivate)
{
	bDoesAutoActivate = bNewAutoActivate;

	OnAutoActivateChanged.Broadcast(bDoesAutoActivate);
}

void UActorInteractorComponentBase::SetActiveInteractable(const TScriptInterface<IActorInteractableInterface> NewInteractable)
{
	ActiveInteractable = NewInteractable;

	OnInteractableSelected.Broadcast(ActiveInteractable);
}

TScriptInterface<IActorInteractableInterface> UActorInteractorComponentBase::GetActiveInteractable() const
{
	return ActiveInteractable;
}