// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/ActorInteractorComponent.h"

#include "Components/ActorInteractableComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Helpers/ActorInteractionPluginLog.h"

#if WITH_EDITOR || !UE_BUILD_SHIPPING
#include "DrawDebugHelpers.h"
#endif

UActorInteractorComponent::UActorInteractorComponent()
{
	// Tick only if ticking is required
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	ComponentTags.Add(INTERACTOR_TAG_NAME);

	bInteractorAutoActivate = true;
	SetIsReplicatedByDefault(true);

#if WITH_EDITOR
	bDebug = false;
#endif
}

void UActorInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner()) AIP_LOG(Error, TEXT("[BeginPlay] Cannot get Owner reference, returning."))

	UpdateTicking();
	UpdatePrecision();

	OnInteractableFound.AddDynamic(this, &UActorInteractorComponent::SetInteractingWith);
	OnInteractableLost.AddDynamic(this, &UActorInteractorComponent::SetInteractingWith);

	// Add required tag if missing
	if(!ComponentHasTag(INTERACTOR_TAG_NAME))
	{
		ComponentTags.Add(INTERACTOR_TAG_NAME);
	}
	
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

	SetInteractionState(EInteractorState::EIS_StandBy);
	
	return bResult;
}

void UActorInteractorComponent::DeactivateInteractor()
{
	SetInteractionState(EInteractorState::EIS_Disabled);
}

void UActorInteractorComponent::SetInteractingWith(UActorInteractableComponent* NewInteractingWith)
{
	if (NewInteractingWith)
	{
		SetInteractionState(EInteractorState::EIS_Active);
	}
	else
	{
		SetInteractionState(EInteractorState::EIS_StandBy);
	}

	InteractingWith = NewInteractingWith;
}

void UActorInteractorComponent::SetInteractionPrecision(const EInteractorPrecision NewPrecision)
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

void UActorInteractorComponent::SetInteractionBoxHalfExtend(const float NewBoxHalfExtend)
{
	InteractorPrecisionBoxHalfExtend = NewBoxHalfExtend;
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
	InteractionTickInterval = (FMath::Max(0.f, NewRefreshRate));
}

void UActorInteractorComponent::SetInteractionRange(const float NewRange)
{
	InteractionRange = (FMath::Max(0.f, NewRange));
}

void UActorInteractorComponent::SetInteractionState(const EInteractorState NewState)
{
	InteractorState = NewState;
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
		EndVector = (DirectionVector * InteractionRange) + CustomTraceTransform.GetLocation();
	}
	else
	{
		GetOwner()->GetActorEyesViewPoint(StartVector, StartRotation);
		
		DirectionVector = UKismetMathLibrary::GetForwardVector(StartRotation);
		EndVector = (DirectionVector * InteractionRange) + StartVector;
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
		
	switch (GetInteractionPrecision())
	{
		case EInteractorPrecision::EIP_High:
			TickPrecise(Trace);
			break;
		case EInteractorPrecision::EIP_Low:
			if (GetPrecisionBoxHalfExtend() > KINDA_SMALL_NUMBER)
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
				InteractionTickInterval,
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
				InteractionTickInterval,
				0,
				0.25
				);
		}
	}
#endif

	if (!bValidFound)
	{
		OnInteractableLost.Broadcast(InteractingWith);
	}
}

void UActorInteractorComponent::TickPrecise(FInteractionTraceData& TraceData) const
{
	GetWorld()->LineTraceSingleByChannel(TraceData.HitResult, TraceData.StartLocation, TraceData.EndLocation, TraceData.CollisionChannel, TraceData.CollisionParams);
}

void UActorInteractorComponent::TickLoose(FInteractionTraceData& TraceData) const
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(GetPrecisionBoxHalfExtend()));

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
	if (GetPrecisionBoxHalfExtend() < KINDA_SMALL_NUMBER)
	{
		SetInteractionPrecision(EInteractorPrecision::EIP_High);
	}
}

void UActorInteractorComponent::OnRep_InteractorState()
{
	//...
}

void UActorInteractorComponent::OnRep_IgnoredActors()
{
	//...
}

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

void UActorInteractorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
#endif

