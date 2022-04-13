// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/ActorInteractorComponent.h"

#include "Components/ActorInteractableComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Helpers/ActorInteractionPluginLog.h"
#include "Net/UnrealNetwork.h"

#if WITH_EDITOR || !UE_BUILD_SHIPPING
#include "DrawDebugHelpers.h"
#endif

UActorInteractorComponent::UActorInteractorComponent()
{
	// Tick only if ticking is required
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	bInteractorAutoActivate = true;
	SetIsReplicatedByDefault(true);
	
	ComponentTags.Add(INTERACTOR_TAG_NAME);

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

	CalculateTick();
	
	if (CanTick())
	{
		TickInteraction(DeltaTime);
	}
}

void UActorInteractorComponent::CalculateTick()
{
	if(HasAuthority())
	{
		const bool bHasWorld = GetWorld() != nullptr;
		const bool bHasOwner = GetOwner() != nullptr;
		const bool bHasState = InteractorState == EInteractorState::EIS_StandBy ||	InteractorState == EInteractorState::EIS_Active;
		const bool bHasActiveType = GetInteractorType() == EInteractorType::EIT_Active;
		const bool bHasValidTick = GetInteractorTickInterval() <= KINDA_SMALL_NUMBER;
		const float TimeSince = GetInteractorTickInterval() > KINDA_SMALL_NUMBER && GetWorld()->TimeSince(GetLastTickTime());
		const bool bHasEnoughTimePassed = TimeSince > GetInteractorTickInterval();
	
		bCanTick = 
		bHasWorld && bHasOwner && bHasState && bHasActiveType && (bHasValidTick || bHasEnoughTimePassed);

		OnRep_CanTick();
	}
	else
	{
		Server_CalculateTick();
	}
}

bool UActorInteractorComponent::CanTick() const
{
	return bCanTick;
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
}

void UActorInteractorComponent::StartInteraction()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (InteractingWith && GetWorld())
		{
			OnInteractionKeyPressed.Broadcast(GetWorld()->GetTimeSeconds());
		}
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_StartInteraction();
	}
}

void UActorInteractorComponent::StopInteraction()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (InteractingWith && GetWorld())
		{
			OnInteractionKeyReleased.Broadcast(GetWorld()->GetTimeSeconds());
		}
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_StopInteraction();
	}
}

// TODO: again the same bug with multiple interactables
void UActorInteractorComponent::TickInteraction(const float DeltaTime)
{
	const float WorldTime = GetWorld()->GetTimeSeconds();
	LastTickTime = WorldTime;

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

	FInteractionTraceData Trace =
		FInteractionTraceData
		(
			StartVector,
			EndVector,
			StartRotation,
			HitResult,
			CollisionParams,
			InteractorTracingChannel
		);
	
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
			if (GetInteractorPrecisionBoxHalfExtend() > KINDA_SMALL_NUMBER)
			{
				TickLoose(Trace);
			}
			else
			{
				TickPrecise(Trace);
			}
			break;
		default:
			break;
	}
	
	bool bValidFound = false;
	if (Trace.HitResult.Component.IsValid() && Trace.HitResult.Actor.IsValid())
	{
		if (UShapeComponent* HitComponent = Cast<UShapeComponent>(Trace.HitResult.Component.Get()))
		{
			AActor* HitActor = Trace.HitResult.Actor.Get();
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
							bValidFound = true;
							LastInteractionTickTime = WorldTime;
						
							OnInteractableLost.Broadcast(InteractingWith);
							OnInteractableFound.Broadcast(InteractableComponent);
							InteractableComponent->OnInteractableTraced.Broadcast(this);
							break;
						}
						
						bValidFound = true;
						LastInteractionTickTime = WorldTime;
						break;
					}
				}
			}
		}
	}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	if (bDebug)
	{
		if (GEngine)
		{
			FString MainText = InteractingWith ? FString
			(
				bValidFound ?
				"[Interactor] Found following component: " :
				"[Interactor] Loosing following component: "
			).Append(InteractingWith->GetName()).Append(" from Owner: ").Append(InteractingWith->GetOwner()->GetName())
			: "[Interactor] No Interactable found in this cycle";
			GEngine->AddOnScreenDebugMessage
			(
				-1,
				InteractorTickInterval,
				FColor::Red,
				MainText
			);
		}
		if (bValidFound)
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
	}
#endif

	if (!bValidFound)
	{
		if (GetInteractingWith() != nullptr)
		{
			OnInteractableLost.Broadcast(InteractingWith);
		}
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		OnRep_InteractorComponent();
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

// TODO: serveeeeer
void UActorInteractorComponent::UpdateTicking()
{
	switch (GetInteractorType())
	{
	case EInteractorType::EIT_Active:
		if (!PrimaryComponentTick.IsTickFunctionEnabled())
		{
			PrimaryComponentTick.SetTickFunctionEnable(true);
			//const float MinAllowedTick = GetInteractorTickInterval();
			//PrimaryComponentTick.TickInterval = FMath::Max(MinAllowedTick, KINDA_SMALL_NUMBER);
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

#pragma region Getters_Setters

void UActorInteractorComponent::SetInteractingWith(UActorInteractableComponent* NewInteractingWith)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (NewInteractingWith == GetInteractingWith())
		{
			return;
		}
		if (NewInteractingWith)
		{
			SetInteractorState(EInteractorState::EIS_Active);
		}
		else
		{
			SetInteractorState(EInteractorState::EIS_StandBy);
		}

		InteractingWith = NewInteractingWith;

		OnRep_InteractorComponent();
	}
}

void UActorInteractorComponent::SetInteractorPrecision(const EInteractorPrecision NewPrecision)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InteractorPrecision = NewPrecision;

		OnRep_InteractorPrecision();
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_SetInteractorPrecision(NewPrecision);
	}
}

void UActorInteractorComponent::SetInteractorType(const EInteractorType NewInteractorType)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InteractorType = NewInteractorType;

		OnInteractorTypeChanged.Broadcast(GetWorld()->GetTimeSeconds());
	
		UpdateTicking();

		OnRep_InteractorType();
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_SetInteractorType(NewInteractorType);
	}
}

void UActorInteractorComponent::SetInteractorAutoActivate(const bool bValue)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		bInteractorAutoActivate = bValue;

		OnRep_InteractorAutoActivate();
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_SetInteractorAutoActivate(bValue);
	}
}

void UActorInteractorComponent::SetIgnoredActors(const TArray<AActor*>& NewIgnoredActors)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		IgnoredActors = NewIgnoredActors;

		OnRep_IgnoredActors();
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_SetIgnoredActors(NewIgnoredActors);
	}
}

void UActorInteractorComponent::AddIgnoredActor(AActor* IgnoredActor)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (!IgnoredActors.Contains(IgnoredActor))
		{
			IgnoredActors.Add(IgnoredActor);

			OnRep_IgnoredActors();
		}
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_AddIgnoredActor(IgnoredActor);
	}
}

void UActorInteractorComponent::AddIgnoredActors(const TArray<AActor*>& ActorsToAdd)
{
	if (ActorsToAdd.Num() > 0)
	{
		// TODO: This is not network friendly, because OnRep_IgnoredActors will be called for each iterator
		for (AActor* Itr : ActorsToAdd)
		{
			AddIgnoredActor(Itr);
		}
	}
}

void UActorInteractorComponent::RemoveIgnoredActor(AActor* UnignoredActor)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (IgnoredActors.Contains(UnignoredActor))
		{
			IgnoredActors.Remove(UnignoredActor);

			OnRep_IgnoredActors();
		}
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_RemoveIgnoredActor(UnignoredActor);
	}
}

void UActorInteractorComponent::RemoveIgnoredActors(const TArray<AActor*>& UnignoredActors)
{
	if (UnignoredActors.Num() > 0)
	{
		// TODO: This is not network friendly, because OnRep_IgnoredActors will be called for each iterator
		for (AActor* const Itr : UnignoredActors)
		{
			RemoveIgnoredActor(Itr);
		}
	}
}

void UActorInteractorComponent::SetInteractorBoxHalfExtend(const float NewBoxHalfExtend)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InteractorPrecisionBoxHalfExtend = NewBoxHalfExtend;

		OnRep_InteractorBoxHalfExtend();
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_SetInteractorPrecisionBoxHalfExtend(NewBoxHalfExtend);
	}
}

void UActorInteractorComponent::SetUseCustomTraceStart(const bool NewValue)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		bUseCustomStartTransform = NewValue;

		OnRep_UseCustomStartTransform();
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_SetUseCustomTraceStart(NewValue);
	}
}

void UActorInteractorComponent::SetCustomTraceStart(const FTransform NewTraceStart)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (bUseCustomStartTransform)
		{
			CustomTraceTransform = NewTraceStart;
		}
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_SetCustomTraceStart(NewTraceStart);
	}
}

void UActorInteractorComponent::SetInteractorTracingChannel(const ECollisionChannel NewChannel)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InteractorTracingChannel = NewChannel;

		OnRep_InteractorTracingChannel();
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_SetInteractorTracingChannel(NewChannel);
	}
}

void UActorInteractorComponent::SetInteractorTickInterval(const float NewRefreshRate)
{
	if(GetOwner()->HasAuthority())
	{
		InteractorTickInterval = (FMath::Max(0.f, NewRefreshRate));
		
		OnRep_InteractorTickInterval();
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_SetInteractorTickInterval(NewRefreshRate);
	}
}

void UActorInteractorComponent::SetInteractorRange(const float NewRange)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InteractorRange = (FMath::Max(0.f, NewRange));

		OnRep_InteractorRange();
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_SetInteractorRange(NewRange);
	}
}

void UActorInteractorComponent::SetInteractorState(const EInteractorState NewState)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InteractorState = NewState;

		OnRep_InteractorState();
	}
	else if(GetOwner() && !GetOwner()->HasAuthority())
	{
		Server_SetInteractorState(NewState);
	}
}

#pragma endregion Getters_Setters

#pragma region Replication

#pragma region OnRep

void UActorInteractorComponent::OnRep_InteractorComponent()
{
	InteractingWith = GetInteractingWith();
	LastTickTime = GetLastTickTime();
	LastInteractionTickTime = GetLastInteractionTickTime();
}

void UActorInteractorComponent::OnRep_InteractorAutoActivate()
{
	bInteractorAutoActivate = GetInteractorAutoActivate();
}

void UActorInteractorComponent::OnRep_InteractorState()
{
	InteractorState = GetInteractorState();
}

void UActorInteractorComponent::OnRep_InteractorTickInterval()
{
	InteractorTickInterval = GetInteractorTickInterval();
}

void UActorInteractorComponent::OnRep_InteractorRange()
{
	InteractorRange = GetInteractorRange();
}

void UActorInteractorComponent::OnRep_InteractorBoxHalfExtend()
{
	InteractorPrecisionBoxHalfExtend = GetInteractorPrecisionBoxHalfExtend();
}

void UActorInteractorComponent::OnRep_InteractorPrecision()
{
	InteractorPrecision = GetInteractorPrecision();
}

void UActorInteractorComponent::OnRep_InteractorTracingChannel()
{
	InteractorTracingChannel = GetInteractorTracingChannel();
}

void UActorInteractorComponent::OnRep_InteractorType()
{
	InteractorType = GetInteractorType();
}

void UActorInteractorComponent::OnRep_UseCustomStartTransform()
{
	bUseCustomStartTransform = GetUseCustomTraceStart();
}

void UActorInteractorComponent::OnRep_CustomTraceTransform()
{
	CustomTraceTransform = GetCustomTraceStart();
}

void UActorInteractorComponent::OnRep_IgnoredActors()
{
	IgnoredActors = GetIgnoredActors();
}

void UActorInteractorComponent::OnRep_CanTick()
{
	bCanTick = CanTick();
}

#pragma endregion OnRep

#pragma region Getters_Setters

void UActorInteractorComponent::Server_CalculateTick_Implementation()
{
	CalculateTick();
}

// TODO: some proper validation here
bool UActorInteractorComponent::Server_CalculateTick_Validate()
{	return true;}

void UActorInteractorComponent::Server_StartInteraction_Implementation()
{
	StartInteraction();
}

bool UActorInteractorComponent::Server_StartInteraction_Validate()
{return true;}

void UActorInteractorComponent::Server_StopInteraction_Implementation()
{
	StopInteraction();
}

bool UActorInteractorComponent::Server_StopInteraction_Validate()
{return true;}


void UActorInteractorComponent::Server_SetInteractorPrecision_Implementation(const EInteractorPrecision& Value)
{
	SetInteractorPrecision(Value);
}

bool UActorInteractorComponent::Server_SetInteractorPrecision_Validate(const EInteractorPrecision& Value)
{return true; }

void UActorInteractorComponent::Server_SetInteractorType_Implementation(const EInteractorType& Value)
{
	SetInteractorType(Value);
}

bool UActorInteractorComponent::Server_SetInteractorType_Validate(const EInteractorType& Value)
{ return true; }

void UActorInteractorComponent::Server_SetInteractorAutoActivate_Implementation(const bool Value)
{
	SetInteractorAutoActivate(Value);
}

bool UActorInteractorComponent::Server_SetInteractorAutoActivate_Validate(const bool Value)
{ return true; }

void UActorInteractorComponent::Server_SetInteractorPrecisionBoxHalfExtend_Implementation(const float Value)
{
	SetInteractorBoxHalfExtend(Value);
}

bool UActorInteractorComponent::Server_SetInteractorPrecisionBoxHalfExtend_Validate(const float Value)
{return true;}

void UActorInteractorComponent::Server_SetUseCustomTraceStart_Implementation(const bool Value)
{
	SetUseCustomTraceStart(Value);
}

bool UActorInteractorComponent::Server_SetUseCustomTraceStart_Validate(const bool Value)
{return true; }

void UActorInteractorComponent::Server_SetCustomTraceStart_Implementation(const FTransform& Value)
{
	SetCustomTraceStart(Value);
}

bool UActorInteractorComponent::Server_SetCustomTraceStart_Validate(const FTransform& Value)
{return true; }

void UActorInteractorComponent::Server_SetInteractorTracingChannel_Implementation(const ECollisionChannel& Value)
{
	SetInteractorTracingChannel(Value);
}

bool UActorInteractorComponent::Server_SetInteractorTracingChannel_Validate(const ECollisionChannel& Value)
{return true; }

void UActorInteractorComponent::Server_SetInteractorTickInterval_Implementation(const float Value)
{
	SetInteractorTickInterval(Value);
}

bool UActorInteractorComponent::Server_SetInteractorTickInterval_Validate(const float Value)
{ return true; }

void UActorInteractorComponent::Server_SetInteractorRange_Implementation(const float Value)
{
	SetInteractorRange(Value);
}

bool UActorInteractorComponent::Server_SetInteractorRange_Validate(const float Value)
{ return true; }

void UActorInteractorComponent::Server_SetInteractorState_Implementation(const EInteractorState Value)
{
	SetInteractorState(Value);
}

bool UActorInteractorComponent::Server_SetInteractorState_Validate(const EInteractorState Value)
{ return true; }

void UActorInteractorComponent::Server_SetIgnoredActors_Implementation(const TArray<AActor*>& Values)
{
	SetIgnoredActors(Values);
}

bool UActorInteractorComponent::Server_SetIgnoredActors_Validate(const TArray<AActor*>& Values)
{return true; }

void UActorInteractorComponent::Server_RemoveIgnoredActor_Implementation(AActor* Value)
{
	RemoveIgnoredActor(Value);
}

bool UActorInteractorComponent::Server_RemoveIgnoredActor_Validate(AActor* Value)
{ return true;}

void UActorInteractorComponent::Server_AddIgnoredActor_Implementation(AActor* Value)
{
	AddIgnoredActor(Value);
}

bool UActorInteractorComponent::Server_AddIgnoredActor_Validate(AActor* Value)
{return true;}

#pragma endregion Server

void UActorInteractorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UActorInteractorComponent, bInteractorAutoActivate); // requested only with its OnRep
	DOREPLIFETIME(UActorInteractorComponent, InteractorState); // done
	
	DOREPLIFETIME(UActorInteractorComponent, InteractingWith); // requested only with OnRep_Interactor
	DOREPLIFETIME(UActorInteractorComponent, LastTickTime); // requested only with OnRep_Interactor
	DOREPLIFETIME(UActorInteractorComponent, LastInteractionTickTime); // requested only with OnRep_Interactor
	DOREPLIFETIME(UActorInteractorComponent, InteractorType); // done
	DOREPLIFETIME(UActorInteractorComponent, InteractorTickInterval); // done
	DOREPLIFETIME(UActorInteractorComponent, InteractorTracingChannel); // done
	DOREPLIFETIME(UActorInteractorComponent, InteractorPrecision); // done
	DOREPLIFETIME(UActorInteractorComponent, InteractorPrecisionBoxHalfExtend); // done
	DOREPLIFETIME(UActorInteractorComponent, InteractorRange); // done
	
	DOREPLIFETIME(UActorInteractorComponent, bUseCustomStartTransform); // done
	DOREPLIFETIME(UActorInteractorComponent, CustomTraceTransform); // done

	DOREPLIFETIME(UActorInteractorComponent, IgnoredActors); // done
	DOREPLIFETIME(UActorInteractorComponent, bCanTick);
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