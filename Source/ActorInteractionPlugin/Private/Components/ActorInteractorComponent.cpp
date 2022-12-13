// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/ActorInteractorComponent.h"

#include "Components/ActorInteractableComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Helpers/ActorInteractionPluginLog.h"
#include "Net/UnrealNetwork.h"

#if (WITH_EDITOR || !UE_BUILD_SHIPPING)
#include "DrawDebugHelpers.h"
#endif

UActorInteractorComponent::UActorInteractorComponent()
{
	// Tick only if ticking is required
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	bAutoRegister = true;
	bAutoActivate = true;
	
	bInteractorAutoActivate = true;
	SetIsReplicatedByDefault(true);
	
	ComponentTags.Add(INTERACTOR_TAG_NAME);

	bUseCustomStartTransform = false;
	bOverlappingInteractable = false;

#if WITH_EDITOR
	bDebug = false;
#endif
}

void UActorInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	// Owner Check
	if (!GetOwner())
	{
		AIP_LOG(Error, TEXT("[BeginPlay] Cannot get Owner reference, returning."))
		return;
	}

	// Setup values to avoid issues
	UpdateTicking();
	UpdatePrecision();

	// Bind Events
	OnInteractableFound.AddDynamic(this, &UActorInteractorComponent::SetInteractingWith);
	OnInteractableLost.AddDynamic(this, &UActorInteractorComponent::SetInteractingWith);

	// Add required tag if missing
	if(!ComponentHasTag(INTERACTOR_TAG_NAME))
	{
		ComponentTags.Add(INTERACTOR_TAG_NAME);
	}

	// Always ignore Owner
	AddIgnoredActor(GetOwner());

	// Based on Auto Activation set the State
	if (GetInteractorAutoActivate())
	{
		FString ErrorMessage;
		if(!ActivateInteractor(ErrorMessage))
		{
			AIP_LOG(Error, TEXT("[BeginPlay] %s"), *ErrorMessage)
		}
	}
	else
	{
		DeactivateInteractor();
	}
}

void UActorInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (CanTick())
	{
		TickInteraction(DeltaTime);
	}
}

bool UActorInteractorComponent::CanTick() const
{
	const bool bHasWorld = GetWorld() != nullptr;
	const bool bHasOwner = GetOwner() != nullptr;
	const bool bHasState = InteractorState == EInteractorState::EIS_StandBy ||	InteractorState == EInteractorState::EIS_Active;
	const bool bHasActiveType = GetInteractorType() == EInteractorType::EIT_Active || GetInteractorType() == EInteractorType::EIT_Mixed;
	const bool bUnlimitedTickTime = GetInteractorTickInterval() <= KINDA_SMALL_NUMBER;
	const float TimeSince = GetWorld()->TimeSince(GetLastTickTime()); //GetInteractorTickInterval() > KINDA_SMALL_NUMBER 
	const bool bTimeSince = TimeSince > GetInteractorTickInterval();

	const bool bIsMixed = GetInteractorType() == EInteractorType::EIT_Mixed;
	
	if (bIsMixed)
	{
		return bHasWorld && bHasOwner && bHasState && bHasActiveType && (bUnlimitedTickTime || bTimeSince) && !bOverlappingInteractable;
	}
	
	return 
	bHasWorld && bHasOwner && bHasState && bHasActiveType && (bUnlimitedTickTime || bTimeSince);
}

bool UActorInteractorComponent::ActivateInteractor(FString& ErrorMessage)
{
	bool bResult = true;
	ErrorMessage = FString("");

	if(!GetOwner())
	{
		AIP_LOG(Error, TEXT("[ActivateInteractor] No Owner. Please report this bug."))
		ErrorMessage.Append(FString("Error: Invalid Owner;"));
		bResult = false;
	}

	if (!GetWorld())
	{
		AIP_LOG(Error, TEXT("[ActivateInteractor] No World. Please report this bug."))
		const FString NoWorldString = TEXT("Error: Invaid World;");
		if(ErrorMessage.Len() == 0) ErrorMessage = NoWorldString;
		else ErrorMessage.Append(FString("|")).Append(NoWorldString);
		bResult = false;
	}

	SetInteractorState(EInteractorState::EIS_StandBy);
	
	return bResult;
}

void UActorInteractorComponent::DeactivateInteractor()
{
	SetInteractorState(EInteractorState::EIS_Disabled);

	// Force close all connections
	OnInteractableLost.Broadcast(InteractingWith);
}

void UActorInteractorComponent::StartInteraction()
{
	if (InteractingWith && GetWorld())
	{
		OnInteractionKeyPressed.Broadcast(GetWorld()->GetTimeSeconds());
	}
}

void UActorInteractorComponent::StopInteraction()
{
	if (InteractingWith && GetWorld())
	{
		OnInteractionKeyReleased.Broadcast(GetWorld()->GetTimeSeconds());
	}
}

void UActorInteractorComponent::OnInteractableOverlapped(UPrimitiveComponent* OverlappedComponent)
{
	UpdateOverlapping(true);
}

void UActorInteractorComponent::CalculateInteractionTrace(FInteractionTraceData& Trace) const
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	const TArray<AActor*> ListOfIgnoredActors = GetIgnoredActors();
	if (ListOfIgnoredActors.Num() > 0)
	{
		CollisionParams.AddIgnoredActors(ListOfIgnoredActors);
	}
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.bReturnPhysicalMaterial = true;

	FVector StartVector;
	FRotator StartRotation;
	FVector DirectionVector;
	FVector EndVector;

	if (bUseCustomStartTransform)
	{
		StartVector = CustomTraceTransform.GetLocation();
		StartRotation = CustomTraceTransform.GetRotation().Rotator();
		DirectionVector = UKismetMathLibrary::GetForwardVector(StartRotation);
		EndVector = (DirectionVector * InteractorRange) + CustomTraceTransform.GetLocation();
	}
	else
	{
		GetOwner()->GetActorEyesViewPoint(StartVector, StartRotation);
		
		DirectionVector = UKismetMathLibrary::GetForwardVector(StartRotation);
		EndVector = (DirectionVector * InteractorRange) + StartVector;
	}

	Trace.StartLocation = StartVector;
	Trace.EndLocation = EndVector;
	Trace.TraceRotation = StartRotation;
	Trace.HitResult = HitResult;
	Trace.CollisionParams = CollisionParams;
	Trace.CollisionChannel = InteractorTracingChannel;

#if WITH_EDITOR
	if(bDebug)
	{
		DrawDebugBox(GetWorld(), StartVector, FVector(10.f), FColor::Blue, false, 2.f, 0, 0.25);
		DrawDebugLine(GetWorld(), StartVector, EndVector, FColor::Green, false, 1, 0, 1);
	}
#endif
		
	switch (GetInteractorPrecision())
	{
		case EInteractorPrecision::EIP_High:
			TickPrecise(Trace);
			break;
		case EInteractorPrecision::EIP_Low:
			TickLoose(Trace);
			break;
		default:
			break;
	}
}

void UActorInteractorComponent::TickInteraction(const float DeltaTime)
{
	const float WorldTime = GetWorld()->GetTimeSeconds();
	LastTickTime = WorldTime;

	FInteractionTraceData Trace;
	CalculateInteractionTrace(Trace);

	if (Trace.HitResult.Component.IsValid() && Trace.HitResult.GetActor())
	{
		if (UShapeComponent* HitComponent = Cast<UShapeComponent>(Trace.HitResult.Component.Get()))
		{
			AActor* HitActor = Trace.HitResult.GetActor();
			TArray<UActorComponent*> InteractableComponents = HitActor->GetComponentsByTag(UActorInteractableComponent::StaticClass(), INTERACTABLE_TAG_NAME);
			if (InteractableComponents.Num())
			{
				for (UActorComponent* const Itr : InteractableComponents)
				{
					UActorInteractableComponent* InteractableComponent = Cast<UActorInteractableComponent>(Itr);
					if(InteractableComponent->FindCollisionShape(HitComponent))
					{
						if (InteractableComponent != InteractingWith)
						{
							LastInteractionTickTime = WorldTime;
						
							OnInteractableLost.Broadcast(InteractingWith);
							OnInteractableFound.Broadcast(InteractableComponent);
							
							InteractableComponent->OnInteractableTraced.Broadcast(this);
							break;
						}
						
						LastInteractionTickTime = WorldTime;
						
						OnInteractableFound.Broadcast(InteractableComponent);
						break;
					}
				}

				#if WITH_EDITOR || !UE_BUILD_SHIPPING
				if (bDebug)
				{
					DrawDebugBox
					(
						GetWorld(),
						Trace.HitResult.Location,
						FVector(InteractorPrecisionBoxHalfExtend),
						FColor::Black,
						false,
						InteractorTickInterval,
						0,
						0.25
					);
				}
				#endif
			}
		}
	}
	else
	{
		OnInteractableLost.Broadcast(InteractingWith);
	}
}

void UActorInteractorComponent::TickPrecise(FInteractionTraceData& TraceData) const
{
	GetWorld()->LineTraceSingleByChannel
	(
		TraceData.HitResult,
		TraceData.StartLocation,
		TraceData.EndLocation,
		TraceData.CollisionChannel,
		TraceData.CollisionParams
	);
}

void UActorInteractorComponent::TickLoose(FInteractionTraceData& TraceData) const
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(GetInteractorPrecisionBoxHalfExtend()));

	GetWorld()->SweepSingleByChannel
		(
			TraceData.HitResult,
			TraceData.StartLocation,
			TraceData.EndLocation,
			TraceData.TraceRotation.Quaternion(),
			TraceData.CollisionChannel,
			CollisionShape,
			TraceData.CollisionParams
		);
}

void UActorInteractorComponent::UpdateTicking()
{
	switch (GetInteractorType())
	{
		case EInteractorType::EIT_Mixed:
		case EInteractorType::EIT_Active:
			if (!PrimaryComponentTick.IsTickFunctionEnabled())
			{
				PrimaryComponentTick.SetTickFunctionEnable(true);
			}
			break;
		case EInteractorType::EIT_Passive:
		case EInteractorType::Default:
		default:
			if (PrimaryComponentTick.IsTickFunctionEnabled())
			{
				PrimaryComponentTick.SetTickFunctionEnable(false);
			}
			break;
	}
}

void UActorInteractorComponent::UpdatePrecision()
{
	if (GetInteractorPrecisionBoxHalfExtend() < KINDA_SMALL_NUMBER)
	{
		SetInteractorPrecision(EInteractorPrecision::EIP_High);
	}
}

void UActorInteractorComponent::UpdateOverlapping(const bool bValue)
{
#if WITH_EDITOR
	if (bDebug)
	{
		AIP_LOG(Warning, TEXT("Overlapping Update from %s to %s"), bOverlappingInteractable ? TEXT("TRUE") : TEXT("FALSE"), bValue ? TEXT("TRUE") : TEXT("FALSE"))
	}
#endif
		
	bOverlappingInteractable = bValue;
}

#pragma region Getters_Setters

void UActorInteractorComponent::SetInteractingWith(UActorInteractableComponent* NewInteractingWith)
{
	if (NewInteractingWith == GetInteractingWith())
	{
		return;
	}
		
	InteractingWith = NewInteractingWith;
		
	if (InteractingWith)
	{
		SetInteractorState(EInteractorState::EIS_Active);

		InteractingWith->OnInteractorOverlapped.AddUniqueDynamic(this, &UActorInteractorComponent::OnInteractableOverlapped);
	}
	else
	{
		SetInteractorState(EInteractorState::EIS_StandBy);
		
		UpdateOverlapping(false);
	}
}

void UActorInteractorComponent::SetInteractorPrecision(const EInteractorPrecision NewPrecision)
{
	InteractorPrecision = NewPrecision;
}

void UActorInteractorComponent::SetInteractorType(const EInteractorType NewInteractorType)
{
	InteractorType = NewInteractorType;

	OnInteractorTypeChanged.Broadcast(GetWorld()->GetTimeSeconds());
	
	UpdateTicking();
}

void UActorInteractorComponent::SetInteractorAutoActivate(const bool bValue)
{
	bInteractorAutoActivate = bValue;
}

void UActorInteractorComponent::SetIgnoredActors(const TArray<AActor*>& NewIgnoredActors)
{
	IgnoredActors = NewIgnoredActors;
}

void UActorInteractorComponent::AddIgnoredActor(AActor* IgnoredActor)
{
	if (!IgnoredActors.Contains(IgnoredActor))
	{
		IgnoredActors.Add(IgnoredActor);
	}
}

void UActorInteractorComponent::AddIgnoredActors(const TArray<AActor*>& ActorsToAdd)
{
	if (ActorsToAdd.Num() > 0)
	{
		for (AActor* Itr : ActorsToAdd)
		{
			AddIgnoredActor(Itr);
		}
	}
}

void UActorInteractorComponent::RemoveIgnoredActor(AActor* UnignoredActor)
{
	if (IgnoredActors.Contains(UnignoredActor))
	{
		IgnoredActors.Remove(UnignoredActor);
	}
}

void UActorInteractorComponent::RemoveIgnoredActors(const TArray<AActor*>& UnignoredActors)
{
	if (UnignoredActors.Num() > 0)
	{
		for (AActor* const Itr : UnignoredActors)
		{
			RemoveIgnoredActor(Itr);
		}
	}
}

void UActorInteractorComponent::SetInteractorBoxHalfExtend(const float NewBoxHalfExtend)
{
	InteractorPrecisionBoxHalfExtend = NewBoxHalfExtend;

	UpdatePrecision();
}

void UActorInteractorComponent::SetUseCustomTraceStart(const bool NewValue)
{
	bUseCustomStartTransform = NewValue;
}

void UActorInteractorComponent::SetCustomTraceStart(const FTransform NewTraceStart)
{
	if (bUseCustomStartTransform)
	{
		CustomTraceTransform = NewTraceStart;
	}
}

void UActorInteractorComponent::SetInteractorTracingChannel(const ECollisionChannel NewChannel)
{
	InteractorTracingChannel = NewChannel;
}

void UActorInteractorComponent::SetInteractorTickInterval(const float NewRefreshRate)
{
	InteractorTickInterval = (FMath::Max(0.f, NewRefreshRate));
}

void UActorInteractorComponent::SetInteractorRange(const float NewRange)
{
	InteractorRange = (FMath::Max(0.f, NewRange));
}

void UActorInteractorComponent::SetInteractorState(const EInteractorState NewState)
{
	InteractorState = NewState;
}

#pragma endregion Getters_Setters

#pragma region Replication

#pragma region OnRep

///////////////////////////////////////
////// HERE GO ON_REP FUNCTIONS ///////
///////////////////////////////////////

#pragma endregion OnRep

#pragma region Server_Functions

///////////////////////////////////////
////// HERE GO SERVER FUNCTIONS ///////
///////////////////////////////////////

#pragma endregion Server_Functions

#pragma region Server_Getters_Setters

///////////////////////////////////////
/// HERE GO SERVER GetSet FUNCTIONS ///
///////////////////////////////////////

#pragma endregion SERVER_Getters_Setters

void UActorInteractorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, bInteractorAutoActivate, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, InteractorState, COND_OwnerOnly);
	
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, InteractingWith, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, LastTickTime, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, LastInteractionTickTime, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, InteractorType, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, InteractorTickInterval, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, InteractorTracingChannel, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, InteractorPrecision, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, InteractorPrecisionBoxHalfExtend, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, InteractorRange, COND_OwnerOnly);
	
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, bUseCustomStartTransform, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponent, CustomTraceTransform, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(UActorInteractorComponent, IgnoredActors, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(UActorInteractorComponent, bOverlappingInteractable, COND_OwnerOnly);
	*/
}

#pragma endregion Replication

#pragma region Editor

#if WITH_EDITOR
void UActorInteractorComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;
	
	if(PropertyName == TEXT("InteractorType"))
	{
		UpdateTicking();
	}

	if (PropertyName == TEXT("InteractorPrecisionBoxHalfExtend"))
	{
		UpdatePrecision();
	}
}
#endif

#pragma endregion Editor