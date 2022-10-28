// All rights reserved Dominik Pavlicek 2022.

#include "Components/ActorInteractableComponentBase.h"

#if WITH_EDITOR
#include "Helpers/ActorInteractionPluginLog.h"
#endif

#include "Helpers/InteractionHelpers.h"

#include "Interfaces/ActorInteractorInterface.h"

UActorInteractableComponentBase::UActorInteractableComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;

	bToggleDebug = false;
	bInteractableAutoSetup = false;

	InteractableState = EInteractableStateV2::EIS_Asleep;
	DefaultInteractableState = EInteractableStateV2::EIS_Asleep;
	InteractionWeight = 1;
	
	bInteractionHighlight = true;
	StencilID = 133;

	LifecycleMode = EInteractableLifecycle::EIL_Cycled;
	LifecycleCount = 2;
	CooldownPeriod = 3.f;

	InteractionOwner = GetOwner();

	const FInteractionKeySetup GamepadKeys = FKey("Gamepad Face Button Down");
	FInteractionKeySetup KeyboardKeys = GamepadKeys;
	KeyboardKeys.Keys.Add("E");
		
	InteractionKeysPerPlatform.Add((TEXT("Windows")), KeyboardKeys);
	InteractionKeysPerPlatform.Add((TEXT("Mac")), KeyboardKeys);
	InteractionKeysPerPlatform.Add((TEXT("PS4")), GamepadKeys);
	InteractionKeysPerPlatform.Add((TEXT("XboxOne")), GamepadKeys);
}

void UActorInteractableComponentBase::BeginPlay()
{
	Super::BeginPlay();

	InteractionOwner = GetOwner();

	// Interaction Events
	OnInteractorFound.AddUniqueDynamic(this, &UActorInteractableComponentBase::InteractorFound);
	OnInteractorLost.AddUniqueDynamic(this, &UActorInteractableComponentBase::InteractorLost);

	OnInteractorOverlapped.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableBeginOverlapEvent);
	OnInteractorStopOverlap.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableStopOverlapEvent);
	OnInteractorTraced.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableTracedEvent);

	OnInteractionCompleted.AddUniqueDynamic(this, &UActorInteractableComponentBase::InteractionCompleted);
	OnInteractionStarted.AddUniqueDynamic(this, &UActorInteractableComponentBase::InteractionStarted);
	OnInteractionStopped.AddUniqueDynamic(this, &UActorInteractableComponentBase::InteractionStopped);
	OnLifecycleCompleted.AddUniqueDynamic(this, &UActorInteractableComponentBase::InteractionLifecycleCompleted);
	OnCooldownCompleted.AddUniqueDynamic(this, &UActorInteractableComponentBase::InteractionCooldownCompleted);
	
	// Attributes Events
	OnInteractableAutoSetupChanged.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableAutoSetupChangedEvent);
	OnInteractableWeightChanged.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableWeightChangedEvent);
	OnInteractableStateChanged.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableStateChangedEvent);
	OnInteractableOwnerChanged.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableOwnerChangedEvent);
	OnInteractableCollisionChannelChanged.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableCollisionChannelChangedEvent);
	OnLifecycleModeChanged.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnLifecycleModeChangedEvent);
	OnLifecycleCountChanged.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnLifecycleCountChangedEvent);
	OnCooldownPeriodChanged.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnCooldownPeriodChangedEvent);
	OnInteractorChanged.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractorChangedEvent);

	// Ignored Classes Events
	OnIgnoredInteractorClassAdded.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnIgnoredClassAdded);
	OnIgnoredInteractorClassRemoved.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnIgnoredClassRemoved);

	// Highlight Events
	OnHighlightableComponentAdded.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnHighlightableComponentAddedEvent);
	OnHighlightableComponentRemoved.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnHighlightableComponentRemovedEvent);
	
	// Collision Events
	OnCollisionComponentAdded.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnCollisionComponentAddedEvent);
	OnCollisionComponentRemoved.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnCollisionComponentRemovedEvent);
	
	SetState(DefaultInteractableState);

	AutoSetup();
}

#pragma region InteractionImplementations

bool UActorInteractableComponentBase::DoesAutoSetup() const
{ return bInteractableAutoSetup; }

void UActorInteractableComponentBase::ToggleAutoSetup(const bool NewValue)
{
	bInteractableAutoSetup = NewValue;
}

bool UActorInteractableComponentBase::ActivateInteractable(FString& ErrorMessage)
{
	const EInteractableStateV2 CachedState = GetState();

	SetState(EInteractableStateV2::EIS_Active);

	switch (CachedState)
	{
		case EInteractableStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactable Component is already Active"));
			break;
		case EInteractableStateV2::EIS_Awake:
			ErrorMessage.Append(TEXT("Interactable Component has been Activated"));
			return true;
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::EIS_Cooldown:
		case EInteractableStateV2::EIS_Completed:
		case EInteractableStateV2::EIS_Disabled:
			ErrorMessage.Append(TEXT("Interactable Component cannot be Activated"));
			break;
		case EInteractableStateV2::Default: 
		default:
			ErrorMessage.Append(TEXT("Interactable Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UActorInteractableComponentBase::WakeUpInteractable(FString& ErrorMessage)
{
	const EInteractableStateV2 CachedState = GetState();

	SetState(EInteractableStateV2::EIS_Awake);

	switch (CachedState)
	{
		case EInteractableStateV2::EIS_Awake:
			ErrorMessage.Append(TEXT("Interactable Component is already Awake"));
			break;
		case EInteractableStateV2::EIS_Active:
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::EIS_Cooldown:
		case EInteractableStateV2::EIS_Disabled:
			ErrorMessage.Append(TEXT("Interactable Component has been Awaken"));
			return true;
		case EInteractableStateV2::EIS_Completed:
			ErrorMessage.Append(TEXT("Interactable Component cannot be Awaken"));
			break;
		case EInteractableStateV2::Default: 
		default:
			ErrorMessage.Append(TEXT("Interactable Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UActorInteractableComponentBase::SnoozeInteractable(FString& ErrorMessage)
{
	const EInteractableStateV2 CachedState = GetState();

	SetState(EInteractableStateV2::EIS_Asleep);

	switch (CachedState)
	{
		case EInteractableStateV2::EIS_Asleep:
			ErrorMessage.Append(TEXT("Interactable Component is already Asleep"));
			break;
		case EInteractableStateV2::EIS_Awake:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::EIS_Active:
		case EInteractableStateV2::EIS_Disabled:
			ErrorMessage.Append(TEXT("Interactable Component has been Asleep"));
			return true;
		case EInteractableStateV2::EIS_Cooldown:
			// TODO: reset cooldown
			ErrorMessage.Append(TEXT("Interactable Component has been Asleep"));
			return true;
		case EInteractableStateV2::EIS_Completed:
			ErrorMessage.Append(TEXT("Interactable Component cannot be Asleep"));
			break;
		case EInteractableStateV2::Default: 
		default:
			ErrorMessage.Append(TEXT("Interactable Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UActorInteractableComponentBase::CompleteInteractable(FString& ErrorMessage)
{
	const EInteractableStateV2 CachedState = GetState();

	SetState(EInteractableStateV2::EIS_Asleep);

	switch (CachedState)
	{
		case EInteractableStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactable Component is Completed"));
			return true;
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::EIS_Awake:
		case EInteractableStateV2::EIS_Disabled:
		case EInteractableStateV2::EIS_Cooldown:
			ErrorMessage.Append(TEXT("Interactable Component cannot be Completed"));
			break;
		case EInteractableStateV2::EIS_Completed:
			ErrorMessage.Append(TEXT("Interactable Component is already Completed"));
			break;
		case EInteractableStateV2::Default: 
		default:
			ErrorMessage.Append(TEXT("Interactable Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

void UActorInteractableComponentBase::DeactivateInteractable()
{
	SetState(EInteractableStateV2::EIS_Disabled);
}

bool UActorInteractableComponentBase::CanInteractEvent_Implementation() const
{
	return CanInteract();
}

bool UActorInteractableComponentBase::CanInteract() const
{
	switch (InteractableState)
	{
		case EInteractableStateV2::EIS_Awake:
			return GetInteractor().GetInterface() != nullptr;
		case EInteractableStateV2::EIS_Active: // already interacting
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Disabled:
		case EInteractableStateV2::EIS_Cooldown:
		case EInteractableStateV2::EIS_Completed:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::Default: 
		default: break;
	}

	return false;
}

EInteractableStateV2 UActorInteractableComponentBase::GetState() const
{ return InteractableState; }

void UActorInteractableComponentBase::SetState(const EInteractableStateV2 NewState)
{
	switch (NewState)
	{
		case EInteractableStateV2::EIS_Active:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Awake:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Active:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::EIS_Suppressed:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Disabled:
				case EInteractableStateV2::Default:
				default: break;
			}
			break;
		case EInteractableStateV2::EIS_Awake:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Active:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::EIS_Suppressed:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Disabled:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Awake:
				case EInteractableStateV2::Default: 
				default: break;
			}
			break;
		case EInteractableStateV2::EIS_Asleep:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Active:
				case EInteractableStateV2::EIS_Awake:
				case EInteractableStateV2::EIS_Suppressed:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Disabled:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::Default: 
				default: break;
			}
			break;
		case EInteractableStateV2::EIS_Cooldown:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Active:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Awake:
				case EInteractableStateV2::EIS_Suppressed:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Disabled:
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::Default: 
				default: break;
			}
			break;
		case EInteractableStateV2::EIS_Completed:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Active:
					{
						InteractableState = NewState;
						OnInteractableStateChanged.Broadcast(InteractableState);
						if (GetWorld()) GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
						SetInteractor(nullptr);
					}
					break;
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Suppressed:
				case EInteractableStateV2::EIS_Awake:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Disabled:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::Default: 
				default: break;
			}
			break;
		case EInteractableStateV2::EIS_Disabled:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Active:
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Awake:
				case EInteractableStateV2::EIS_Suppressed:
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Asleep:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Disabled:
				case EInteractableStateV2::Default: 
				default: break;
			}
			break;
		case EInteractableStateV2::EIS_Suppressed:
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Active:
				case EInteractableStateV2::EIS_Awake:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::EIS_Disabled:
					InteractableState = NewState;
					OnInteractableStateChanged.Broadcast(InteractableState);
					break;
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Suppressed:
				case EInteractableStateV2::Default:
				default: break;
			}
			break;
		case EInteractableStateV2::Default: 
		default: break;
	}

	ProcessDependencies();
}

TArray<TSoftClassPtr<UObject>> UActorInteractableComponentBase::GetIgnoredClasses() const
{ return IgnoredClasses; }

void UActorInteractableComponentBase::SetIgnoredClasses(const TArray<TSoftClassPtr<UObject>> NewIgnoredClasses)
{
	IgnoredClasses.Empty();

	IgnoredClasses = NewIgnoredClasses;
}

void UActorInteractableComponentBase::AddIgnoredClass(TSoftClassPtr<UObject> AddIgnoredClass)
{
	if (AddIgnoredClass == nullptr) return;

	if (IgnoredClasses.Contains(AddIgnoredClass)) return;

	IgnoredClasses.Add(AddIgnoredClass);

	OnIgnoredInteractorClassAdded.Broadcast(AddIgnoredClass);
}

void UActorInteractableComponentBase::AddIgnoredClasses(TArray<TSoftClassPtr<UObject>> AddIgnoredClasses)
{
	for (const auto Itr : AddIgnoredClasses)
	{
		AddIgnoredClass(Itr);
	}
}

void UActorInteractableComponentBase::RemoveIgnoredClass(TSoftClassPtr<UObject> RemoveIgnoredClass)
{
	if (RemoveIgnoredClass == nullptr) return;

	if (!IgnoredClasses.Contains(RemoveIgnoredClass)) return;

	IgnoredClasses.Remove(RemoveIgnoredClass);

	OnIgnoredInteractorClassRemoved.Broadcast(RemoveIgnoredClass);
}

void UActorInteractableComponentBase::RemoveIgnoredClasses(TArray<TSoftClassPtr<UObject>> RemoveIgnoredClasses)
{
	for (const auto Itr : RemoveIgnoredClasses)
	{
		RemoveIgnoredClass(Itr);
	}
}

void UActorInteractableComponentBase::AddInteractionDependency(const TScriptInterface<IActorInteractableInterface> InteractionDependency)
{
	if (InteractionDependencies.Contains(InteractionDependency)) return;

	InteractionDependencies.Add(InteractionDependency);
}

void UActorInteractableComponentBase::RemoveInteractionDependency(const TScriptInterface<IActorInteractableInterface> InteractionDependency)
{
	if (!InteractionDependencies.Contains(InteractionDependency)) return;

	InteractionDependencies.Remove(InteractionDependency);
}

TArray<TScriptInterface<IActorInteractableInterface>> UActorInteractableComponentBase::GetInteractionDependencies() const
{ return InteractionDependencies; }

void UActorInteractableComponentBase::ProcessDependencies()
{
	if (InteractionDependencies.Num() == 0) return;
	
	for (const auto Itr : InteractionDependencies)
	{
		switch (InteractableState)
		{
			case EInteractableStateV2::EIS_Active:
			case EInteractableStateV2::EIS_Suppressed:
				Itr->SetState(EInteractableStateV2::EIS_Suppressed);
				break;
			case EInteractableStateV2::EIS_Disabled:
			case EInteractableStateV2::EIS_Awake:
			case EInteractableStateV2::EIS_Asleep:
				Itr->SetState(InteractableState);
				break;
			case EInteractableStateV2::EIS_Cooldown:
			case EInteractableStateV2::EIS_Completed:
				Itr->SetState(EInteractableStateV2::EIS_Awake);
				RemoveInteractionDependency(Itr);
				break;
			case EInteractableStateV2::Default:
			default:
				break;
		}
	}
}

TScriptInterface<IActorInteractorInterface> UActorInteractableComponentBase::GetInteractor() const
{ return Interactor; }

void UActorInteractableComponentBase::SetInteractor(const TScriptInterface<IActorInteractorInterface> NewInteractor)
{
	Interactor = NewInteractor;

	OnInteractorChanged.Broadcast(Interactor);
}

int32 UActorInteractableComponentBase::GetInteractableWeight() const
{ return InteractionWeight; }

void UActorInteractableComponentBase::SetInteractableWeight(const int32 NewWeight)
{
	InteractionWeight = FMath::Max(0, NewWeight);

	OnInteractableWeightChanged.Broadcast(InteractionWeight);
}

AActor* UActorInteractableComponentBase::GetInteractableOwner() const
{ return InteractionOwner; }

void UActorInteractableComponentBase::SetInteractableOwner(AActor* NewOwner)
{
	InteractionOwner = NewOwner;
	
	OnInteractableOwnerChanged.Broadcast(InteractionOwner);
}

ECollisionChannel UActorInteractableComponentBase::GetCollisionChannel() const
{ return CollisionChannel; }

void UActorInteractableComponentBase::SetCollisionChannel(const ECollisionChannel& NewChannel)
{
	CollisionChannel = NewChannel;

	OnInteractableCollisionChannelChanged.Broadcast(CollisionChannel);
}

TArray<UPrimitiveComponent*> UActorInteractableComponentBase::GetCollisionComponents() const
{	return CollisionComponents;}

EInteractableLifecycle UActorInteractableComponentBase::GetLifecycleMode() const
{	return LifecycleMode;}

void UActorInteractableComponentBase::SetLifecycleMode(const EInteractableLifecycle& NewMode)
{
	LifecycleMode = NewMode;

	OnLifecycleModeChanged.Broadcast(LifecycleMode);
}

int32 UActorInteractableComponentBase::GetLifecycleCount() const
{	return LifecycleCount;}

void UActorInteractableComponentBase::SetLifecycleCount(const int32 NewLifecycleCount)
{
	switch (LifecycleMode)
	{
		case EInteractableLifecycle::EIL_Cycled:
			if (NewLifecycleCount < -1)
			{
				LifecycleCount = -1;
				OnLifecycleCountChanged.Broadcast(LifecycleCount);
			}
			else if (NewLifecycleCount < 2)
			{
				LifecycleCount = 2;
				OnLifecycleCountChanged.Broadcast(LifecycleCount);
			}
			else if (NewLifecycleCount > 2)
			{
				LifecycleCount = NewLifecycleCount;
				OnLifecycleCountChanged.Broadcast(LifecycleCount);
			}
			break;
		case EInteractableLifecycle::EIL_OnlyOnce:
		case EInteractableLifecycle::Default:
		default: break;
	}
}

int32 UActorInteractableComponentBase::GetRemainingLifecycleCount() const
{ return RemainingLifecycleCount; }

float UActorInteractableComponentBase::GetCooldownPeriod() const
{ return CooldownPeriod; }

void UActorInteractableComponentBase::SetCooldownPeriod(const float NewCooldownPeriod)
{
	switch (LifecycleMode)
	{
		case EInteractableLifecycle::EIL_Cycled:
			LifecycleCount = FMath::Max(0.1f, NewCooldownPeriod);
			OnLifecycleCountChanged.Broadcast(LifecycleCount);
			break;
		case EInteractableLifecycle::EIL_OnlyOnce:
		case EInteractableLifecycle::Default:
		default: break;
	}
}

FKey UActorInteractableComponentBase::GetInteractionKey(const FString& RequestedPlatform) const
{
	if(const FInteractionKeySetup* KeySet = InteractionKeysPerPlatform.Find(RequestedPlatform))
	{
		if (KeySet->Keys.Num() == 0) return FKey();

		return KeySet->Keys[0];
	}
	
	return FKey();
}

void UActorInteractableComponentBase::SetInteractionKey(const FString& Platform, const FKey NewInteractorKey)
{
	if (const auto KeySet = InteractionKeysPerPlatform.Find(Platform))
	{
		if (KeySet->Keys.Contains(NewInteractorKey)) return;

		KeySet->Keys.Add(NewInteractorKey);
	}
}

TMap<FString, FInteractionKeySetup> UActorInteractableComponentBase::GetInteractionKeys() const
{	return InteractionKeysPerPlatform;}

bool UActorInteractableComponentBase::FindKey(const FKey& RequestedKey, const FString& Platform) const
{
	if (const auto KeySet = InteractionKeysPerPlatform.Find(Platform))
	{
		return KeySet->Keys.Contains(RequestedKey);
	}
	
	return false;
}

void UActorInteractableComponentBase::AddCollisionComponent(UPrimitiveComponent* CollisionComp)
{
	if (CollisionComp == nullptr) return;
	if (CollisionComponents.Contains(CollisionComp)) return;
	
	CollisionComponents.Add(CollisionComp);
	
	BindCollisionShape(CollisionComp);
	
	OnCollisionComponentAdded.Broadcast(CollisionComp);
}

void UActorInteractableComponentBase::AddCollisionComponents(const TArray<UPrimitiveComponent*> NewCollisionComponents)
{
	for (UPrimitiveComponent* const Itr : NewCollisionComponents)
	{
		AddCollisionComponent(Itr);
	}
}

void UActorInteractableComponentBase::RemoveCollisionComponent(UPrimitiveComponent* CollisionComp)
{
	if (CollisionComp == nullptr) return;
	if (!CollisionComponents.Contains(CollisionComp)) return;
	
	CollisionComponents.Remove(CollisionComp);

	UnbindCollisionShape(CollisionComp);
	
	OnCollisionComponentRemoved.Broadcast(CollisionComp);
}

void UActorInteractableComponentBase::RemoveCollisionComponents(const TArray<UPrimitiveComponent*> RemoveCollisionComponents)
{
	for (UPrimitiveComponent* const Itr : RemoveCollisionComponents)
	{
		RemoveCollisionComponent(Itr);
	}
}

TArray<UMeshComponent*> UActorInteractableComponentBase::GetHighlightableComponents() const
{	return HighlightableComponents;}

void UActorInteractableComponentBase::AddHighlightableComponent(UMeshComponent* MeshComponent)
{
	if (MeshComponent == nullptr) return;
	if (HighlightableComponents.Contains(MeshComponent)) return;

	HighlightableComponents.Add(MeshComponent);

	BindHighlightableMesh(MeshComponent);

	OnHighlightableComponentAdded.Broadcast(MeshComponent);
}

void UActorInteractableComponentBase::AddHighlightableComponents(const TArray<UMeshComponent*> AddMeshComponents)
{
	for (UMeshComponent* const Itr : HighlightableComponents)
	{
		AddHighlightableComponent(Itr);
	}
}

void UActorInteractableComponentBase::RemoveHighlightableComponent(UMeshComponent* MeshComponent)
{
	if (MeshComponent == nullptr) return;
	if (!HighlightableComponents.Contains(MeshComponent)) return;

	HighlightableComponents.Remove(MeshComponent);

	UnbindHighlightableMesh(MeshComponent);

	OnHighlightableComponentRemoved.Broadcast(MeshComponent);
}

void UActorInteractableComponentBase::RemoveHighlightableComponents(const TArray<UMeshComponent*> RemoveMeshComponents)
{
	for (UMeshComponent* const Itr : RemoveMeshComponents)
	{
		RemoveHighlightableComponent(Itr);
	}
}

UMeshComponent* UActorInteractableComponentBase::FindMeshByTag(const FName Tag) const
{
	if (!GetOwner()) return nullptr;

	TArray<UMeshComponent*> PrimitiveComponents;
	GetOwner()->GetComponents(PrimitiveComponents);

	for (const auto Itr : PrimitiveComponents)
	{
		if (Itr && Itr->ComponentHasTag(Tag))
		{
			return Itr;
		}
	}
	
	return nullptr;
}

UPrimitiveComponent* UActorInteractableComponentBase::FindPrimitiveByTag(const FName Tag) const
{
	if (!GetOwner()) return nullptr;

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	GetOwner()->GetComponents(PrimitiveComponents);

	for (const auto Itr : PrimitiveComponents)
	{
		if (Itr && Itr->ComponentHasTag(Tag))
		{
			return Itr;
		}
	}
	
	return nullptr;
}

TArray<FName> UActorInteractableComponentBase::GetCollisionOverrides() const
{	return CollisionOverrides;}

TArray<FName> UActorInteractableComponentBase::GetHighlightableOverrides() const
{	return HighlightableOverrides;}

void UActorInteractableComponentBase::InteractorFound(const TScriptInterface<IActorInteractorInterface>& FoundInteractor)
{
	/**
	 * TODO
	 * Validation
	 * If Valid, then Broadcast
	 */

	SetInteractor(FoundInteractor);
	OnInteractorFoundEvent(FoundInteractor);
}

void UActorInteractableComponentBase::InteractorLost(const TScriptInterface<IActorInteractorInterface>& LostInteractor)
{
	/**
	 * TODO
	 * Validation
	 * If Valid, then Broadcast
	 */

	SetInteractor(nullptr);
	OnInteractorLostEvent(LostInteractor);
}

void UActorInteractableComponentBase::InteractionCompleted(const float& TimeCompleted)
{
	/**
	 * TODO
	 * Validation
	 * If Valid, then Broadcast
	 */

	if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
	{
		TriggerCooldown();
	}

	OnInteractionCompletedEvent(TimeCompleted);
}

void UActorInteractableComponentBase::InteractionStarted(const float& TimeStarted)
{
	/**
	 * TODO
	 * Validation
	 * If Valid, then Broadcast
	 */

	OnInteractionStartedEvent(TimeStarted);
}

void UActorInteractableComponentBase::InteractionStopped()
{
	/**
	 * TODO
	 * Validation
	 * If Valid, then Broadcast
	 */
	

	OnInteractionStoppedEvent();
}

void UActorInteractableComponentBase::InteractionLifecycleCompleted()
{
	SetState(EInteractableStateV2::EIS_Completed);

	OnLifecycleCompletedEvent();
}

void UActorInteractableComponentBase::InteractionCooldownCompleted()
{
	/**
	 * TODO
	 * Validation
	 * If Valid, then Broadcast
	 */

	OnCooldownCompletedEvent();
}

void UActorInteractableComponentBase::OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
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
				break;
			case EInteractorStateV2::EIS_Asleep:
			case EInteractorStateV2::EIS_Suppressed:
			case EInteractorStateV2::EIS_Disabled:
			case EInteractorStateV2::Default:
				continue;
		}

		if (FoundInteractor->GetResponseChannel() != GetCollisionChannel()) continue;

		SetInteractor(FoundInteractor);
		
		OnInteractorOverlapped.Broadcast(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

		OnInteractorFound.Broadcast(FoundInteractor);
		break;
	}
}

void UActorInteractableComponentBase::OnInteractableStopOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;

	TArray<UActorComponent*> InteractorComponents = OtherActor->GetComponentsByInterface(UActorInteractorInterface::StaticClass());

	if (InteractorComponents.Num() == 0) return;

	for (const auto Itr : InteractorComponents)
	{
		if (Itr == GetInteractor().GetObject())
		{
			OnInteractorLost.Broadcast(GetInteractor());
			
			SetInteractor(nullptr);
			
			OnInteractorStopOverlap.Broadcast(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
			
			return;
		}
	}
}

void UActorInteractableComponentBase::OnInteractableTraced(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	/**
	 * TODO
	 * Validation
	 * If Valid, then Broadcast
	 */

	OnInteractorTraced.Broadcast(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

void UActorInteractableComponentBase::FindAndAddCollisionShapes()
{
	for (const auto Itr : CollisionOverrides)
	{
		if (const auto NewCollision = FindPrimitiveByTag(Itr))
		{
			AddCollisionComponent(NewCollision);
			BindCollisionShape(NewCollision);
		}
		else
		{
			AIntP_LOG(Error, TEXT("[Actor Interactable Component] Primitive Component with %s tag not found!"), *Itr.ToString())
		}
	}
}

void UActorInteractableComponentBase::FindAndAddHighlightableMeshes()
{
	for (const auto Itr : HighlightableOverrides)
	{
		if (const auto NewMesh = FindMeshByTag(Itr))
		{
			AddHighlightableComponent(NewMesh);
			BindHighlightableMesh(NewMesh);
		}
		else
		{
			AIntP_LOG(Error, TEXT("[Actor Interactable Component] Mesh Component with %s tag not found!"), *Itr.ToString())
		}
	}
}

void UActorInteractableComponentBase::TriggerCooldown()
{
	const int32 TempRemainingLifecycleCount = RemainingLifecycleCount - 1;
	RemainingLifecycleCount = FMath::Max(0, TempRemainingLifecycleCount);

	if (GetWorld())
	{
		if (RemainingLifecycleCount <= 0)
		{
			SetState(EInteractableStateV2::EIS_Completed);
			return;
		}

		SetState(EInteractableStateV2::EIS_Cooldown);
		GetWorld()->GetTimerManager().SetTimer
		(
			CooldownHandle,
			this,
			&UActorInteractableComponentBase::OnCooldownCompletedEvent,
			CooldownPeriod
		);
	}
}

void UActorInteractableComponentBase::BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const
{
	if (!PrimitiveComponent) return;

	PrimitiveComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableBeginOverlap);
	PrimitiveComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableStopOverlap);
	PrimitiveComponent->OnComponentHit.AddUniqueDynamic(this, &UActorInteractableComponentBase::OnInteractableTraced);

	FCollisionShapeCache CachedValues;
	CachedValues.bGenerateOverlapEvents = PrimitiveComponent->GetGenerateOverlapEvents();
	CachedValues.CollisionEnabled = PrimitiveComponent->GetCollisionEnabled();
	CachedValues.CollisionResponse = GetCollisionResponseToChannel(CollisionChannel);
	
	CachedCollisionShapesSettings.Add(PrimitiveComponent, CachedValues);

	PrimitiveComponent->SetGenerateOverlapEvents(true);
	PrimitiveComponent->SetCollisionResponseToChannel(CollisionChannel, ECollisionResponse::ECR_Overlap);

	switch (PrimitiveComponent->GetCollisionEnabled())
	{
		case ECollisionEnabled::NoCollision:
			PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			break;
		case ECollisionEnabled::QueryOnly:
		case ECollisionEnabled::PhysicsOnly:
		case ECollisionEnabled::QueryAndPhysics:
		default: break;
	}
}

void UActorInteractableComponentBase::UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const
{
	if(!PrimitiveComponent) return;

	PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UActorInteractableComponentBase::OnInteractableBeginOverlap);
	PrimitiveComponent->OnComponentEndOverlap.RemoveDynamic(this, &UActorInteractableComponentBase::OnInteractableStopOverlap);
	PrimitiveComponent->OnComponentHit.RemoveDynamic(this, &UActorInteractableComponentBase::OnInteractableTraced);

	if (CachedCollisionShapesSettings.Find(PrimitiveComponent))
	{
		PrimitiveComponent->SetGenerateOverlapEvents(CachedCollisionShapesSettings[PrimitiveComponent].bGenerateOverlapEvents);
		PrimitiveComponent->SetCollisionEnabled(CachedCollisionShapesSettings[PrimitiveComponent].CollisionEnabled);
		PrimitiveComponent->SetCollisionResponseToChannel(CollisionChannel, CachedCollisionShapesSettings[PrimitiveComponent].CollisionResponse);
	}
	else
	{
		PrimitiveComponent->SetGenerateOverlapEvents(true);
		PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		PrimitiveComponent->SetCollisionResponseToChannel(CollisionChannel, ECollisionResponse::ECR_Overlap);
	}
}

void UActorInteractableComponentBase::BindHighlightableMesh(UMeshComponent* MeshComponent) const
{
	if (!MeshComponent) return;
	
	MeshComponent->SetRenderCustomDepth(true);
}

void UActorInteractableComponentBase::UnbindHighlightableMesh(UMeshComponent* MeshComponent) const
{
	if (!MeshComponent) return;
	
	MeshComponent->SetRenderCustomDepth(false);
}

void UActorInteractableComponentBase::AutoSetup()
{
	if (DoesAutoSetup())
	{
		// Get all Parent Components
		TArray<USceneComponent*> ParentComponents;
		GetParentComponents(ParentComponents);

		// Iterate over them and assign them properly
		if (ParentComponents.Num() > 0)
		{
			for (const auto Itr : ParentComponents)
			{
				if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Itr))
				{
					AddCollisionComponent(PrimitiveComp);

					if (UMeshComponent* MeshComp = Cast<UMeshComponent>(PrimitiveComp))
					{
						AddHighlightableComponent(MeshComp);
					}
				}
			}
		}
	}
	
	FindAndAddCollisionShapes();
	FindAndAddHighlightableMeshes();
}

#pragma endregion