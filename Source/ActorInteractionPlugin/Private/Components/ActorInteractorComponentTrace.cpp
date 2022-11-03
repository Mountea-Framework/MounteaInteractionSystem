// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractorComponentTrace.h"
#include "Kismet/KismetMathLibrary.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif

UActorInteractorComponentTrace::UActorInteractorComponentTrace()
{
	TraceType = ETraceType::ETT_Loose;
	TraceInterval = 0.01f;
	TraceRange = 250.f;
	TraceShapeHalfSize = 5.f;
	bUseCustomStartTransform = false;
}

void UActorInteractorComponentTrace::BeginPlay()
{
	OnTraceTypeChanged.AddUniqueDynamic(this, &UActorInteractorComponentTrace::OnTraceTypeChangedEvent);

	Super::BeginPlay();
}

void UActorInteractorComponentTrace::DrawTracingDebugStart(FInteractionTraceDataV2& InteractionTraceData) const
{
#if WITH_EDITOR
	if(bToggleDebug)
	{
		DrawDebugString(GetWorld(), InteractionTraceData.StartLocation, TEXT("Tracing Start Location"), nullptr, FColor::Blue, TraceInterval);
		
		switch (TraceType)
		{
			case ETraceType::ETT_Precise:
				DrawDebugLine(GetWorld(), InteractionTraceData.StartLocation, InteractionTraceData.EndLocation, FColor::Blue, false, TraceInterval, 0, 0.25f);
				break;
			case ETraceType::ETT_Loose:
				DrawDebugSphere(GetWorld(), InteractionTraceData.StartLocation, 10.f, 6, FColor::Blue, false, TraceInterval, 0, 0.25f);
				break;
		}
		
		DrawDebugString(GetWorld(), InteractionTraceData.EndLocation, TEXT("Tracing Expected End"), nullptr, FColor::Red, TraceInterval);
		DrawDebugSphere(GetWorld(), InteractionTraceData.EndLocation, 10.f, 6, FColor::Red, false, TraceInterval, 0, 0.25f);
	}
#endif
}

void UActorInteractorComponentTrace::DrawTracingDebugEnd(FInteractionTraceDataV2& InteractionTraceData) const
{
#if WITH_EDITOR
	if(bToggleDebug && InteractionTraceData.HitResult.bBlockingHit)
	{
		DrawDebugString(GetWorld(), InteractionTraceData.HitResult.ImpactPoint, TEXT("Tracing Hit"),nullptr, FColor::Green, TraceInterval);
		DrawDebugSphere(GetWorld(), InteractionTraceData.HitResult.ImpactPoint, 10.f, 6, FColor::Green, false, TraceInterval, 0, 0.25f);
	}
#endif
}

void UActorInteractorComponentTrace::DisableTracing()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(Timer_Ticking);
	}
}

void UActorInteractorComponentTrace::EnableTracing()
{
	if (GetWorld())
	{
		if(GetWorld()->GetTimerManager().IsTimerPaused(Timer_Ticking))
		{
			GetWorld()->GetTimerManager().UnPauseTimer(Timer_Ticking);
		}
		else
		{
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &UActorInteractorComponentTrace::ProcessTrace);
			
			GetWorld()->GetTimerManager().SetTimer(Timer_Ticking, Delegate, FMath::Max(0.01f, TraceInterval), false);
		}
	}
}

void UActorInteractorComponentTrace::PauseTracing()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().PauseTimer(Timer_Ticking);
	}
}

void UActorInteractorComponentTrace::ResumeTracing()
{
	EnableTracing();
}

void UActorInteractorComponentTrace::ProcessTrace()
{
	if (!CanTrace())
	{
		DisableTracing();
	}
	
	AddIgnoredActor(GetOwner());
	
	FInteractionTraceDataV2 TraceData;
	{
		TraceData.CollisionChannel = GetResponseChannel();
	
		TraceData.CollisionParams.AddIgnoredActors(ListOfIgnoredActors);
		TraceData.CollisionParams.MobilityType = EQueryMobilityType::Any;
		TraceData.CollisionParams.bReturnPhysicalMaterial = true;
	
		FVector DirectionVector;
		if (bUseCustomStartTransform)
		{
			TraceData.StartLocation = CustomTraceTransform.GetLocation();
			TraceData.TraceRotation = CustomTraceTransform.GetRotation().Rotator();
			DirectionVector = UKismetMathLibrary::GetForwardVector(TraceData.TraceRotation);
			TraceData.EndLocation = (DirectionVector * TraceRange) + CustomTraceTransform.GetLocation();
		}
		else
		{
			GetOwner()->GetActorEyesViewPoint(TraceData.StartLocation, TraceData.TraceRotation);
		
			DirectionVector = UKismetMathLibrary::GetForwardVector(TraceData.TraceRotation);
			TraceData.EndLocation = (DirectionVector * TraceRange) + TraceData.StartLocation;
		}
	}

#if WITH_EDITOR
	if(bToggleDebug)
	{
		DrawTracingDebugStart(TraceData);
	}
#endif
	
	switch (TraceType)
	{
		case ETraceType::ETT_Precise:
			ProcessTrace_Precise(TraceData);
			break;
		case ETraceType::ETT_Loose:
			ProcessTrace_Loose(TraceData);
			break;
		case ETraceType::Default:
		default: break;
	}

#if WITH_EDITOR
	if (bToggleDebug)
	{
		DrawTracingDebugEnd(TraceData);
	}
#endif

	// TODO
	// Validate results
	ResumeTracing();
}

void UActorInteractorComponentTrace::ProcessTrace_Precise(FInteractionTraceDataV2& InteractionTraceData)
{
	GetWorld()->LineTraceSingleByChannel
	(
		InteractionTraceData.HitResult,
		InteractionTraceData.StartLocation,
		InteractionTraceData.EndLocation,
		InteractionTraceData.CollisionChannel,
		InteractionTraceData.CollisionParams
	);
}

void UActorInteractorComponentTrace::ProcessTrace_Loose(FInteractionTraceDataV2& InteractionTraceData)
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(TraceShapeHalfSize));

	GetWorld()->SweepSingleByChannel
	(
		InteractionTraceData.HitResult,
		InteractionTraceData.StartLocation,
		InteractionTraceData.EndLocation,
		InteractionTraceData.TraceRotation.Quaternion(),
		InteractionTraceData.CollisionChannel,
		CollisionShape,
		InteractionTraceData.CollisionParams
	);
}

bool UActorInteractorComponentTrace::CanTrace() const
{
	switch (GetState())
	{
		case EInteractorStateV2::EIS_Asleep:
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::EIS_Suppressed:
		case EInteractorStateV2::Default:
		default:
			return false;
		case EInteractorStateV2::EIS_Awake:
		case EInteractorStateV2::EIS_Active:
			return true;
	}
}

bool UActorInteractorComponentTrace::CanInteract() const
{
	if (GetWorld() && Super::CanInteract())
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(Timer_Ticking))
		{
			return GetWorld()->GetTimerManager().GetTimerElapsed(Timer_Ticking) >= TraceInterval;
		}
	}

	return false;
}

void UActorInteractorComponentTrace::SetState(const EInteractorStateV2 NewState)
{
	Super::SetState(NewState);

	if (GetWorld())
	{
		switch (GetState())
		{
			case EInteractorStateV2::EIS_Asleep:
			case EInteractorStateV2::EIS_Disabled:
			case EInteractorStateV2::EIS_Suppressed:
				DisableTracing();
				break;
			case EInteractorStateV2::EIS_Awake:
				EnableTracing();
				break;
			case EInteractorStateV2::EIS_Active:
				PauseTracing();
				break;
			case EInteractorStateV2::Default:
			default:
				break;
		}
	}
}



