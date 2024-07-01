// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractorComponentOverlap.h"

#include "Helpers/ActorInteractionPluginLog.h"
#include "Helpers/MounteaInteractionSystemBFL.h"
#include "Interfaces/ActorInteractableInterface.h"
#include "Net/UnrealNetwork.h"

UActorInteractorComponentOverlap::UActorInteractorComponentOverlap()
		: OverrideCollisionComponents(TArray<FName>()),
		CollisionShapes(TArray<UPrimitiveComponent*>()),
		CachedCollisionShapesSettings(TMap<UPrimitiveComponent*, FCollisionShapeCache>())
{
	ComponentTags.Add(FName("Overlap"));
}

void UActorInteractorComponentOverlap::BeginPlay()
{
	SetupInteractorOverlap();
	Super::BeginPlay();
}

FString UActorInteractorComponentOverlap::ToString_Implementation() const
{
	FText baseDebugData = FText::FromString(Super::ToString_Implementation());
	FText safetyTraceText = FText::FromString(bUseSafetyTrace ? TEXT("True") : TEXT("False"));
	FText validationCollisionChannelText = FText::FromString(UEnum::GetValueAsString(ValidationCollisionChannel));

	FText overlapDebugData = FText::Format(
		NSLOCTEXT("InteractorOverlapDebugData", "Format", "\nUse Safety Trace: {0}\nValidation Collision Channel: {1}"),
		safetyTraceText, validationCollisionChannelText
	);

	return FText::Format(
		NSLOCTEXT("InteractorOverlapDebugData", "CombinedFormat", "{0}{1}"),
		baseDebugData, overlapDebugData
	).ToString();
}

void UActorInteractorComponentOverlap::UseSafetyTrace_Implementation(bool bEnable)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[UseSafetyTrace] No owner!"))
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (bUseSafetyTrace != bEnable)
			bUseSafetyTrace = bEnable;
	}
	else
	{
		UseSafetyTrace_Server(bEnable);
	}
}

bool UActorInteractorComponentOverlap::DoesUseSafetyTrace() const
{
	return bUseSafetyTrace;
}

void UActorInteractorComponentOverlap::SetValidationCollisionChannel_Implementation(const ECollisionChannel NewSafetyChannel)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetSafetyCollisionChannel] No owner!"))
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (NewSafetyChannel != ValidationCollisionChannel)
			ValidationCollisionChannel = NewSafetyChannel;
	}
	else
	{
		SetValidationCollisionChannel_Server(NewSafetyChannel);
	}
}

ECollisionChannel UActorInteractorComponentOverlap::GetValidationCollisionChannel() const
{
	return ValidationCollisionChannel;
}

void UActorInteractorComponentOverlap::ProcessStateChanged()
{
	Super::ProcessStateChanged();

	switch (Execute_GetState(this))
	{
		case EInteractorStateV2::EIS_Active:
			break;
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
		if (auto Comp = UMounteaInteractionSystemBFL::FindPrimitiveByName(Itr, GetOwner()))
			AddCollisionComponent(Comp);
		else
		{
			Comp = UMounteaInteractionSystemBFL::FindPrimitiveByTag(Itr, GetOwner());
			 if (Comp)
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

#if WITH_EDITOR || UE_BUILD_DEBUG

	if (DebugSettings.DebugMode)
	{
		Component->SetHiddenInGame(false);
		Component->SetVisibility(true);
	}
	
#endif
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
			auto interactableComponents = OtherActor->GetComponentsByInterface(UActorInteractableInterface::StaticClass());
			if (interactableComponents.Num() == 0)
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
	
	TScriptInterface<IActorInteractableInterface> currentlyActiveInteractable	= Execute_GetActiveInteractable(this);
	TScriptInterface<IActorInteractableInterface> tempInteractable					= nullptr;
	
	TArray<UActorComponent*> interactableComponents = OtherActor->GetComponentsByInterface(UActorInteractableInterface::StaticClass());
	int32 highestWeight = -1;

	for (const auto& Component : interactableComponents)
	{
		TScriptInterface<IActorInteractableInterface> InteractableComponent = TScriptInterface<IActorInteractableInterface>(Component);

		if (!InteractableComponent->Execute_CanBeTriggered(Component))
			continue;

		ECollisionChannel componentCollisionChannel = InteractableComponent->Execute_GetCollisionChannel(Component);
		if (componentCollisionChannel != Execute_GetResponseChannel(this))
			continue;

		if (PrimitiveComponent->GetCollisionResponseToChannel(componentCollisionChannel) == ECR_Ignore)
			continue;

		if (!InteractableComponent->Execute_CanBeTriggered(InteractableComponent.GetObject()))
			continue;

		int32 ComponentWeight = InteractableComponent->Execute_GetInteractableWeight(Component);
		if (ComponentWeight > highestWeight)
		{
			highestWeight = ComponentWeight;
			tempInteractable = InteractableComponent;
		}
	}
	
	if (!tempInteractable.GetObject())
	{
		return;
	}

	if (currentlyActiveInteractable.GetObject())
	{
		if (currentlyActiveInteractable == tempInteractable)
		{
			return;
		}
		
		int32 localWeight		= tempInteractable->					Execute_GetInteractableWeight(tempInteractable.GetObject());
		int32 activeWeight	= currentlyActiveInteractable->	Execute_GetInteractableWeight(currentlyActiveInteractable.GetObject());

		if (localWeight < activeWeight)
		{
			return;
		}
	}

	if (bUseSafetyTrace && !Execute_PerformSafetyTrace(this, OtherActor))
		return;
	
	/*
	{
		// Safety check: Perform line trace to ensure no wall obstruction and that we indeed hit the OtherActor
		FHitResult safetyTrace;
		FCollisionQueryParams queryParams;
		{
			queryParams.AddIgnoredActor(GetOwner());
		}

		bool bHit = GetWorld()->LineTraceSingleByChannel(safetyTrace, GetOwner()->GetActorLocation(), OtherActor->GetActorLocation(), ValidationCollisionChannel, queryParams);

#if WITH_EDITOR || UE_BUILD_DEBUG
		if(DebugSettings.DebugMode)
		{
			DrawDebugBox
			(
				GetWorld(),
				GetOwner()->GetActorLocation(),
				FVector(5.f),
				FColor::Blue,
				false,
				2.f,
				0,
				1.f
			);

			DrawDebugBox
			(
				GetWorld(),
				OtherActor->GetActorLocation(),
				FVector(5.f),
				FColor::Red,
				false,
				2.f,
				0,
				1.f
			);
		
			DrawDebugDirectionalArrow
			(
				GetWorld(),
				GetOwner()->GetActorLocation(),
				OtherActor->GetActorLocation(),
				2.f,
				FColor::Purple,
				false,
				2.f,
				0,
				1.f
			);
		}
#endif
	
		if (bHit && safetyTrace.GetActor() != OtherActor)
		{
			return;
		}
	}
	*/
	
	OnInteractableLost.Broadcast(currentlyActiveInteractable);
	OnInteractableFound.Broadcast(tempInteractable);

	tempInteractable->GetOnInteractorOverlappedHandle().Broadcast(PrimitiveComponent, OtherActor, OtherComp, 0, false, HitResult);
	tempInteractable->GetOnInteractorFoundHandle().Broadcast(this);
}

void UActorInteractorComponentOverlap::HandleEndOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	if (!OtherActor)
	{
		LOG_ERROR(TEXT("[HandleEndOverlap] OtherActor is null!"));
		return;
	}

	if (!PrimitiveComponent)
	{
		LOG_ERROR(TEXT("[HandleEndOverlap] PrimitiveComponent is null!"));
		return;
	}

	if (!OtherComp)
	{
		LOG_ERROR(TEXT("[HandleEndOverlap] OtherComp is null!"));
		return;
	}

	TScriptInterface<IActorInteractableInterface> currentlyActiveInteractable = Execute_GetActiveInteractable(this);
	if (!currentlyActiveInteractable.GetObject())
	{
		return;
	}
	
	// Check if OtherActor has the active interactable component
	TArray<UActorComponent*> interactableComponents = OtherActor->GetComponentsByInterface(UActorInteractableInterface::StaticClass());
	bool bHasActiveInteractable = false;
	for (const auto& Component : interactableComponents)
	{
		if (currentlyActiveInteractable.GetObject() == Component)
		{
			bHasActiveInteractable = true;
			break;
		}
	}

	if (!bHasActiveInteractable)
	{
		return;
	}

	// Check if we still overlap at least one of the collision components from the active interactable
	bool bStillOverlapping = false;
	const TArray<UPrimitiveComponent*> interactableCollisionComponents = currentlyActiveInteractable->Execute_GetCollisionComponents(currentlyActiveInteractable.GetObject());
	for (UPrimitiveComponent* InteractableComp : interactableCollisionComponents)
	{
		for (UPrimitiveComponent* InteractorComp : CollisionShapes)
		{
			if (InteractableComp && InteractorComp && InteractableComp->IsOverlappingComponent(InteractorComp))
			{
				bStillOverlapping = true;
				break;
			}
		}
		if (bStillOverlapping)
		{
			break;
		}
	}

	if (bStillOverlapping)
	{
		return;
	}

	/*
	switch (currentlyActiveInteractable->Execute_GetState(currentlyActiveInteractable.GetObject()))
	{
		case EInteractableStateV2::EIS_Cooldown:
			{
				LOG_INFO(TEXT("[OnInteractableEndOverlap] Overlap ended, but it's OK, because it is in Cooldown!"))
				if (PerformSafetyTrace(OtherActor))
				{
					// TODO: Setup looping Timer that will check 
					return;
				}
			}
			break;
		case EInteractableStateV2::EIS_Paused:
		case EInteractableStateV2::EIS_Completed:
		case EInteractableStateV2::EIS_Disabled:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Active:
		case EInteractableStateV2::EIS_Awake:
		case EInteractableStateV2::Default:
			return;
	}
	*/
	
	OnInteractableLost.Broadcast(currentlyActiveInteractable);
	
	currentlyActiveInteractable->GetOnInteractorStopOverlapHandle().Broadcast(PrimitiveComponent, OtherActor, OtherComp, 0);
	currentlyActiveInteractable->GetOnInteractorLostHandle().Broadcast(this);
}

void UActorInteractorComponentOverlap::StartInteractorOverlap_Server_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	StartInteractorOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void UActorInteractorComponentOverlap::StopInteractorOverlap_Server_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	StopInteractorOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void UActorInteractorComponentOverlap::UseSafetyTrace_Server_Implementation(bool bEnable)
{
	UseSafetyTrace(bEnable);
}

void UActorInteractorComponentOverlap::SetValidationCollisionChannel_Server_Implementation(const ECollisionChannel NewSafetyChannel)
{
	SetValidationCollisionChannel(NewSafetyChannel);
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

		MARK_PROPERTY_DIRTY_FROM_NAME(UActorInteractorComponentOverlap, CollisionShapes, this);
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
			if (!Itr) return;

			if (CollisionShapes.Contains(Itr)) return;

			CollisionShapes.Add(Itr);

			OnCollisionShapeAdded.Broadcast(Itr);
		}

		MARK_PROPERTY_DIRTY_FROM_NAME(UActorInteractorComponentOverlap, CollisionShapes, this);
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

		MARK_PROPERTY_DIRTY_FROM_NAME(UActorInteractorComponentOverlap, CollisionShapes, this);
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
			if (!Itr) return;

			if (!CollisionShapes.Contains(Itr)) return;

			CollisionShapes.Remove(Itr);

			OnCollisionShapeRemoved.Broadcast(Itr);
		}

		MARK_PROPERTY_DIRTY_FROM_NAME(UActorInteractorComponentOverlap, CollisionShapes, this);
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

TArray<UPrimitiveComponent*> UActorInteractorComponentOverlap::GetCollisionComponents() const
{
	return CollisionShapes;
}

void UActorInteractorComponentOverlap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UActorInteractorComponentOverlap, bUseSafetyTrace,					COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentOverlap, ValidationCollisionChannel,	COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentOverlap, CollisionShapes,					COND_Custom);
}