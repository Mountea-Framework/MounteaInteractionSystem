// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractorComponentTrace.h"

#include "Interfaces/ActorInteractableInterface.h"
#include "Kismet/KismetMathLibrary.h"

#include "TimerManager.h"
#include "Helpers/ActorInteractionPluginLog.h"
#include "Helpers/InteractionHelpers.h"

#if WITH_EDITOR
#include "EditorHelper.h"
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
	OnTraceDataChanged.AddUniqueDynamic(this, &UActorInteractorComponentTrace::OnTraceDataChangedEvent);

	{
		FTracingData NewData;
		NewData.TracingType = TraceType;
		NewData.TracingInterval = TraceInterval;
		NewData.TracingRange = TraceRange;
		NewData.TracingShapeHalfSize = TraceShapeHalfSize;
		NewData.bUsingCustomStartTransform = bUseCustomStartTransform;
		NewData.CustomTracingTransform = CustomTraceTransform;

		LastTracingData = NewData;
	}
	
	Super::BeginPlay();
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
	switch (GetState())
	{
		case EInteractorStateV2::EIS_Awake:
		case EInteractorStateV2::EIS_Active:
			break;
		case EInteractorStateV2::EIS_Asleep: 
		case EInteractorStateV2::EIS_Suppressed:
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::Default: 
		default:
			AIntP_LOG(Warning, TEXT("[EnableTracing] Tracing not allowed for this state!"))
			return;
	}
	
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
	if(DebugSettings.DebugMode)
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

	bool bAnyInteractable = false;
	bool bFoundActiveAgain = false;
	
	FHitResult BestHitResult;
	TScriptInterface<IActorInteractableInterface> BestInteractable = nullptr;
	
	for (FHitResult& HitResult : TraceData.HitResults)
	{
		if (HitResult.GetComponent() == nullptr) continue;
		if (const AActor* HitActor = HitResult.GetActor())
		{
			auto InteractableComponents = HitActor->GetComponentsByInterface(UActorInteractableInterface::StaticClass());
			for (const auto& Itr : InteractableComponents)
			{
				if (Itr && Itr->Implements<UActorInteractableInterface>())
				{
					TScriptInterface<IActorInteractableInterface> Interactable = Itr;
					Interactable.SetObject(Itr);
					Interactable.SetInterface(Cast<IActorInteractableInterface>(Itr));

					const bool bCanTraceWith =
					(
						Interactable->GetCollisionComponents().Contains(HitResult.GetComponent()) &&
						Interactable->GetCollisionChannel() == GetResponseChannel() &&
						Interactable->CanBeTriggered()
					);
					
					if (bCanTraceWith)
					{
						bAnyInteractable = true;

						if (Interactable == GetActiveInteractable())
						{
							bFoundActiveAgain = true;
						}

						if (BestInteractable.GetObject() == nullptr)
						{
							BestInteractable = Interactable;
							BestHitResult = HitResult;
						}
						else
						{
							if (Interactable->GetInteractableWeight() > BestInteractable->GetInteractableWeight())
							{
								BestInteractable = Interactable;
								BestHitResult = HitResult;
							}
						}
					}
				}
			}
		}
	}
		
	if (bAnyInteractable == false)
	{
		OnInteractableLost.Broadcast(GetActiveInteractable());
	}
	else if (bFoundActiveAgain == false)
	{
		OnInteractableLost.Broadcast(GetActiveInteractable());
	}
	
	if (bAnyInteractable)
	{
		BestInteractable->GetOnInteractorTracedHandle().Broadcast(BestHitResult.GetComponent(), GetOwner(), nullptr, BestHitResult.Location, BestHitResult);
	}

#if WITH_EDITOR
	if (DebugSettings.DebugMode)
	{
		DrawTracingDebugEnd(TraceData);
	}
#endif
	
	ResumeTracing();

	
}

void UActorInteractorComponentTrace::ProcessTrace_Precise(FInteractionTraceDataV2& InteractionTraceData)
{
	GetWorld()->LineTraceMultiByChannel
	(
		InteractionTraceData.HitResults,
		InteractionTraceData.StartLocation,
		InteractionTraceData.EndLocation,
		InteractionTraceData.CollisionChannel,
		InteractionTraceData.CollisionParams
	);
}

void UActorInteractorComponentTrace::ProcessTrace_Loose(FInteractionTraceDataV2& InteractionTraceData)
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(TraceShapeHalfSize));

	GetWorld()->SweepMultiByChannel
	(
		InteractionTraceData.HitResults,
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
	return CanInteract();
}

ETraceType UActorInteractorComponentTrace::GetTraceType() const
{ return TraceType; }

void UActorInteractorComponentTrace::SetTraceType(const ETraceType& NewTraceType)
{
	const FTracingData OldData = GetLastTracingData();
	FTracingData NewData = GetLastTracingData();
	NewData.TracingType = NewTraceType;

	LastTracingData = NewData;
	
	TraceType = NewTraceType;
	OnTraceDataChanged.Broadcast(NewData, OldData);
}

float UActorInteractorComponentTrace::GetTraceInterval() const
{ return TraceInterval; }

void UActorInteractorComponentTrace::SetTraceInterval(const float NewInterval)
{
	const FTracingData OldData = GetLastTracingData();
	FTracingData NewData = GetLastTracingData();
	NewData.TracingInterval = FMath::Max(0.01f, NewInterval);

	LastTracingData = NewData;
	
	TraceInterval = NewData.TracingInterval;
	OnTraceDataChanged.Broadcast(NewData, OldData);
}

float UActorInteractorComponentTrace::GetTraceRange() const
{ return TraceRange; }

void UActorInteractorComponentTrace::SetTraceRange(const float NewRange)
{
	const FTracingData OldData = GetLastTracingData();
	FTracingData NewData = GetLastTracingData();
	NewData.TracingRange = FMath::Max(1.f, NewRange);

	LastTracingData = NewData;
	
	TraceRange = NewData.TracingRange;
	OnTraceDataChanged.Broadcast(NewData, OldData);
}

float UActorInteractorComponentTrace::GetTraceShapeHalfSize() const
{ return TraceShapeHalfSize; }

void UActorInteractorComponentTrace::SetTraceShapeHalfSize(const float NewTraceShapeHalfSize)
{
	const FTracingData OldData = GetLastTracingData();
	FTracingData NewData = GetLastTracingData();
	NewData.TracingShapeHalfSize = FMath::Max(0.1f, NewTraceShapeHalfSize);

	LastTracingData = NewData;
	
	TraceShapeHalfSize = NewData.TracingShapeHalfSize;
	OnTraceDataChanged.Broadcast(NewData, OldData);
}

bool UActorInteractorComponentTrace::GetUseCustomStartTransform() const
{ return bUseCustomStartTransform; }

void UActorInteractorComponentTrace::SetUseCustomStartTransform(const bool bUse)
{
	const FTracingData OldData = GetLastTracingData();
	FTracingData NewData = GetLastTracingData();
	NewData.bUsingCustomStartTransform = bUse;
	
	bUseCustomStartTransform = bUse;

	LastTracingData = NewData;
	OnTraceDataChanged.Broadcast(NewData, OldData);
}

FTracingData UActorInteractorComponentTrace::GetLastTracingData() const
{ return LastTracingData; }

void UActorInteractorComponentTrace::SetCustomTraceStart(const FTransform TraceStart)
{
	if (bUseCustomStartTransform)
	{
		const FTracingData OldData = GetLastTracingData();
		FTracingData NewData = GetLastTracingData();
		NewData.CustomTracingTransform = TraceStart;
		
		CustomTraceTransform = TraceStart;

		LastTracingData = NewData;
		OnTraceDataChanged.Broadcast(NewData, OldData);
	}
}

FTransform UActorInteractorComponentTrace::GetCustomTraceStart() const
{ return CustomTraceTransform; }

bool UActorInteractorComponentTrace::CanInteract() const
{
	return Super::CanInteract();
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
				EnableTracing();
				break;
			case EInteractorStateV2::Default:
			default:
				break;
		}
	}
}

#if WITH_EDITOR

void UActorInteractorComponentTrace::DrawTracingDebugStart(FInteractionTraceDataV2& InteractionTraceData) const
{
	if(DebugSettings.DebugMode)
	{
		switch (TraceType)
		{
			case ETraceType::ETT_Precise:
				DrawDebugLine(GetWorld(), InteractionTraceData.StartLocation, InteractionTraceData.EndLocation, FColor::Blue, false, TraceInterval, 0, 0.25f);
				break;
			case ETraceType::ETT_Loose:
				DrawDebugSphere(GetWorld(), InteractionTraceData.StartLocation, 10.f, 6, FColor::Blue, false, TraceInterval, 0, 0.25f);
				break;
		}
		
		DrawDebugSphere(GetWorld(), InteractionTraceData.EndLocation, 10.f, 6, FColor::Red, false, TraceInterval, 0, 0.25f);
	}
}

void UActorInteractorComponentTrace::DrawTracingDebugEnd(FInteractionTraceDataV2& InteractionTraceData) const
{
	for (FHitResult& HitResult : InteractionTraceData.HitResults)
	{
		if(DebugSettings.DebugMode && HitResult.GetComponent())
		{
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 6, FColor::Green, false, TraceInterval, 0, 0.25f);
		}
	}
}

void UActorInteractorComponentTrace::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;
	
	FString InteractorName = GetName();
	// Format Name
	{
		if (InteractorName.Contains(TEXT("_GEN_VARIABLE")))
		{
			InteractorName.ReplaceInline(TEXT("_GEN_VARIABLE"), TEXT(""));
		}
		if(InteractorName.EndsWith(TEXT("_C")) && InteractorName.StartsWith(TEXT("Default__")))
		{
		
			InteractorName.RightChopInline(9);
			InteractorName.LeftChopInline(2);
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UActorInteractorComponentTrace, TraceInterval))
	{
		if (TraceInterval < 0.01f)
		{
			const FText ErrorMessage = FText::FromString
			(
				InteractorName.Append(TEXT(": TraceInterval cannot be less than 0.01s!"))
			);
			FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));

			TraceInterval = 0.01f;
		}

		if (TraceInterval > 3.f && DebugSettings.EditorDebugMode)
		{
			const FText ErrorMessage = FText::FromString
			(
				InteractorName.Append(TEXT(": TraceInterval is more than 3s! This might be unintentional and cause gameplay issues."))
			);
			FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("NotificationList.DefaultMessage"));
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UActorInteractorComponentTrace, TraceRange))
	{
		if (TraceRange < 1.f)
		{
			const FText ErrorMessage = FText::FromString
			(
				InteractorName.Append(TEXT(": TraceRange cannot be less than 1cm!"))
			);
			FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));

			TraceRange = 0.01f;
		}

		if (TraceRange > 5000.f && DebugSettings.EditorDebugMode)
		{
			const FText ErrorMessage = FText::FromString
			(
				InteractorName.Append(TEXT(": TraceRange is more than 5000cm! This might be unintentional and cause gameplay issues."))
			);
			FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("NotificationList.DefaultMessage"));
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UActorInteractorComponentTrace, TraceShapeHalfSize))
	{
		if (TraceShapeHalfSize > 25.f && DebugSettings.EditorDebugMode)
		{
			const FText ErrorMessage = FText::FromString
			(
				InteractorName.Append(TEXT(": TraceShapeHalfSize is more than 25cm! This might be unintentional and cause gameplay issues."))
			);
			FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("NotificationList.DefaultMessage"));
		}
	}
}

#endif