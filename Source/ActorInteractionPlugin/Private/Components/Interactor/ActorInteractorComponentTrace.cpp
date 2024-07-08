// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "Components/Interactor/ActorInteractorComponentTrace.h"

#include "Interfaces/ActorInteractableInterface.h"

#include "Kismet/KismetMathLibrary.h"

#include "GameFramework/Actor.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"

#include "TimerManager.h"
#include "Helpers/ActorInteractionPluginLog.h"
#include "Helpers/InteractionHelpers.h"

#include "Net/UnrealNetwork.h"

#if WITH_EDITOR
#include "EditorHelper.h"
#include "DrawDebugHelpers.h"
#endif

UActorInteractorComponentTrace::UActorInteractorComponentTrace() :
		TraceType(ETraceType::ETT_Loose),
		TraceInterval(0.1f),
		TraceRange(250.f),
		TraceShapeHalfSize(5.f),
		bUseCustomStartTransform(false)
{
	ComponentTags.Add(FName("Trace"));
	
	SafetyTraceSetup.SafetyTracingMode = ESafetyTracingMode::ESTM_Location;
}

void UActorInteractorComponentTrace::BeginPlay()
{
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

void UActorInteractorComponentTrace::DisableTracing_Implementation()
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[EnableTracing] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(Timer_Ticking);
		}
	}
	else
	{
		DisableTracing_Server();
	}
}

void UActorInteractorComponentTrace::EnableTracing_Implementation()
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[EnableTracing] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		switch (Execute_GetState(this))
		{
		case EInteractorStateV2::EIS_Awake:
		case EInteractorStateV2::EIS_Active:
			break;
		case EInteractorStateV2::EIS_Asleep: 
		case EInteractorStateV2::EIS_Suppressed:
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::Default: 
		default:
			LOG_WARNING(TEXT("[EnableTracing] Tracing not allowed for this state!"));
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
	else
	{
		EnableTracing_Server();
	}
}

void UActorInteractorComponentTrace::PauseTracing_Implementation()
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[PauseTracing] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().PauseTimer(Timer_Ticking);
		}
	}
	else
	{
		PauseTracing_Server();
	}
}

void UActorInteractorComponentTrace::ResumeTracing_Implementation()
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[ResumeTracing] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		EnableTracing();
	}
	else
	{
		ResumeTracing_Server();
	}
}

void UActorInteractorComponentTrace::ProcessTrace_Implementation()
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[ProcessTrace] No Owner!"));
		return;
	}

	if (!GetOwner()->HasAuthority())
	{
		ProcessTrace_Server();
		return;
	}
	
	if (!CanTrace())
	{
		DisableTracing();
		return;
	}

	Execute_AddIgnoredActor(this, GetOwner());

	FInteractionTraceDataV2 TraceData;
	{
		TraceData.CollisionChannel = Execute_GetResponseChannel(this);
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
		default:
			break;
	}

	bool bAnyInteractable = false;
	bool bFoundActiveAgain = false;

	FHitResult BestHitResult;
	TScriptInterface<IActorInteractableInterface> bestFoundInteractable = nullptr;

	for (FHitResult& HitResult : TraceData.HitResults)
	{
		if (!HitResult.GetComponent() || !HitResult.GetActor())
			continue;

		const AActor* HitActor = HitResult.GetActor();
		auto interactableComponents = HitActor != nullptr ? HitActor->GetComponentsByInterface(UActorInteractableInterface::StaticClass()) : TArray<UActorComponent*>();
		if (interactableComponents.Num() == 0)
			continue;

		for (const auto& Itr : interactableComponents)
		{
			if (!Itr)
				continue;

			TScriptInterface<IActorInteractableInterface> localInteractable = Itr;
			localInteractable.SetObject(Itr);
			localInteractable.SetInterface(Cast<IActorInteractableInterface>(Itr));

			if (!localInteractable.GetObject() || !localInteractable.GetInterface())
				continue;

			if (!localInteractable->Execute_GetCollisionComponents(Itr).Contains(HitResult.GetComponent()))
				continue;

			if (localInteractable->Execute_GetCollisionChannel(Itr) != Execute_GetResponseChannel(this))
				continue;

			if (!localInteractable->Execute_CanBeTriggered(Itr))
			{
				if (localInteractable->Execute_GetInteractor(Itr) != this)
					continue;
			}

			if (InteractorTag.IsValid() && !localInteractable->Execute_GetInteractableCompatibleTags(Itr).HasTag(InteractorTag))
			{
				LOG_INFO(TEXT("[ProcessTrace] Interactor Tag %s is not compatible with %s Interactable on %s Actor"), *InteractorTag.ToString(), *localInteractable->Execute_GetInteractableName(Itr).ToString(), *HitActor->GetName())
				continue;
			}

			bAnyInteractable = true;

			if (localInteractable == Execute_GetActiveInteractable(this))
			{
				bFoundActiveAgain = true;
			}

			const float localInteractableWeight = localInteractable->Execute_GetInteractableWeight(Itr);
			const float bestFoundInteractableWeight = bestFoundInteractable != nullptr ? bestFoundInteractable->Execute_GetInteractableWeight(bestFoundInteractable.GetObject()) : -1.f;

			if (bestFoundInteractable == nullptr || localInteractableWeight > bestFoundInteractableWeight)
			{
				if (!Execute_PerformSafetyTrace(this, HitActor))
				{
					LOG_INFO(TEXT("[PerformTrace] Obstacle found in ray direction"))
					continue;
				}
				
				bestFoundInteractable = localInteractable;
				BestHitResult = HitResult;
			}
		}
	}

	if (bestFoundInteractable != Execute_GetActiveInteractable(this))
	{
		if (Execute_GetActiveInteractable(this) != nullptr)
		{
			if (!bAnyInteractable || Execute_GetActiveInteractable(this) != bestFoundInteractable)
			{
				OnInteractableLost.Broadcast(Execute_GetActiveInteractable(this));
			}
		}

		if (bAnyInteractable && Execute_GetActiveInteractable(this) != bestFoundInteractable)
		{
			OnInteractableFound.Broadcast(bestFoundInteractable);
			bestFoundInteractable->GetOnInteractorTracedHandle().Broadcast(BestHitResult.GetComponent(), GetOwner(), nullptr, BestHitResult.Location, BestHitResult);
			bestFoundInteractable->GetOnInteractorFoundHandle().Broadcast(this);
		}
	}

#if WITH_EDITOR
	if (DebugSettings.DebugMode)
	{
		DrawTracingDebugEnd(TraceData);
	}
#endif

	// Update Client
	PostTraced_Client();
	PostTraced();

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

bool UActorInteractorComponentTrace::CanTrace_Implementation() const
{
	return Execute_CanInteract(this);
}

ETraceType UActorInteractorComponentTrace::GetTraceType() const
{ return TraceType; }

void UActorInteractorComponentTrace::SetTraceType_Implementation(const ETraceType& NewTraceType)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetTraceType] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		const FTracingData OldData = GetLastTracingData();
		FTracingData NewData = GetLastTracingData();
		NewData.TracingType = NewTraceType;

		LastTracingData = NewData;
		
		TraceType = NewTraceType;
		OnTraceDataChanged.Broadcast(NewData, OldData);
	}
	else
	{
		SetTraceType_Server(NewTraceType);
	}
}

float UActorInteractorComponentTrace::GetTraceInterval() const
{ return TraceInterval; }

void UActorInteractorComponentTrace::SetTraceInterval_Implementation(const float NewInterval)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetTraceInterval] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		const FTracingData OldData = GetLastTracingData();
		FTracingData NewData = GetLastTracingData();
		NewData.TracingInterval = FMath::Max(0.01f, NewInterval);

		LastTracingData = NewData;
		
		TraceInterval = NewData.TracingInterval;
		OnTraceDataChanged.Broadcast(NewData, OldData);
	}
	else
	{
		SetTraceInterval_Server(NewInterval);
	}
}

float UActorInteractorComponentTrace::GetTraceRange() const
{ return TraceRange; }

void UActorInteractorComponentTrace::SetTraceRange_Implementation(const float NewRange)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetTraceRange] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		const FTracingData OldData = GetLastTracingData();
		FTracingData NewData = GetLastTracingData();
		NewData.TracingRange = FMath::Max(1.f, NewRange);

		LastTracingData = NewData;
		
		TraceRange = NewData.TracingRange;
		OnTraceDataChanged.Broadcast(NewData, OldData);
	}
	else
	{
		SetTraceRange_Server(NewRange);
	}
}

float UActorInteractorComponentTrace::GetTraceShapeHalfSize() const
{ return TraceShapeHalfSize; }

void UActorInteractorComponentTrace::SetTraceShapeHalfSize_Implementation(const float NewTraceShapeHalfSize)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetTraceShapeHalfSize] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		const FTracingData OldData = GetLastTracingData();
		FTracingData NewData = GetLastTracingData();
		NewData.TracingShapeHalfSize = FMath::Max(0.1f, NewTraceShapeHalfSize);

		LastTracingData = NewData;
		
		TraceShapeHalfSize = NewData.TracingShapeHalfSize;
		OnTraceDataChanged.Broadcast(NewData, OldData);
	}
	else
	{
		SetTraceShapeHalfSize_Server(NewTraceShapeHalfSize);
	}
}

bool UActorInteractorComponentTrace::GetUseCustomStartTransform() const
{ return bUseCustomStartTransform; }

void UActorInteractorComponentTrace::SetUseCustomStartTransform_Implementation(const bool bUse)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetUseCustomStartTransform] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		const FTracingData OldData = GetLastTracingData();
		FTracingData NewData = GetLastTracingData();
		NewData.bUsingCustomStartTransform = bUse;
		
		bUseCustomStartTransform = bUse;

		LastTracingData = NewData;
		OnTraceDataChanged.Broadcast(NewData, OldData);
	}
	else
	{
		SetUseCustomStartTransform_Server(bUse);
	}
}

FTracingData UActorInteractorComponentTrace::GetLastTracingData() const
{ return LastTracingData; }

void UActorInteractorComponentTrace::SetCustomTraceStart_Implementation(const FTransform& TraceStart)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetCustomTraceStart] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (bUseCustomStartTransform)
		{
			const FTracingData OldData = GetLastTracingData();
			FTracingData NewData = GetLastTracingData();
			NewData.CustomTracingTransform = TraceStart;
			
			CustomTraceTransform = TraceStart;

			LastTracingData = NewData;
			OnTraceDataChanged.Broadcast(NewData, OldData);

			switch (SafetyTraceSetup.SafetyTracingMode)
			{
				case ESafetyTracingMode::ESTM_Location:
					SafetyTraceSetup.StartLocation = NewData.CustomTracingTransform.GetLocation();
				case ESafetyTracingMode::ESTM_Socket:
				case ESafetyTracingMode::ESTM_None:
				default:
					break;
			}
		}
	}
	else
	{
		SetCustomTraceStart_Server(TraceStart);
	}
}

FTransform UActorInteractorComponentTrace::GetCustomTraceStart() const
{ return CustomTraceTransform; }

void UActorInteractorComponentTrace::PostTraced_Implementation()
{
	OnTraced.Broadcast();
}

void UActorInteractorComponentTrace::SetTraceType_Server_Implementation(const ETraceType& NewTraceType)
{
	SetTraceType(NewTraceType);
}

void UActorInteractorComponentTrace::SetTraceInterval_Server_Implementation(float NewInterval)
{
	SetTraceInterval(NewInterval);
}

void UActorInteractorComponentTrace::SetTraceRange_Server_Implementation(float NewRange)
{
	SetTraceRange(NewRange);
}

void UActorInteractorComponentTrace::SetTraceShapeHalfSize_Server_Implementation(float NewTraceShapeHalfSize)
{
	SetTraceShapeHalfSize(NewTraceShapeHalfSize);
}

void UActorInteractorComponentTrace::SetUseCustomStartTransform_Server_Implementation(bool bUse)
{
	SetUseCustomStartTransform(bUse);
}

void UActorInteractorComponentTrace::SetCustomTraceStart_Server_Implementation(const FTransform& TraceStart)
{
	SetCustomTraceStart(TraceStart);
}

void UActorInteractorComponentTrace::PostTraced_Client_Implementation()
{
	PostTraced();
}

void UActorInteractorComponentTrace::ProcessStateChanged()
{
	Super::ProcessStateChanged();

	switch (Execute_GetState(this))
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

FString UActorInteractorComponentTrace::ToString_Implementation() const
{
	FText baseDebugData = FText::FromString(Super::ToString_Implementation());
	FText traceTypeText = FText::FromString(UEnum::GetValueAsString(TraceType));
	FText traceIntervalText = FText::AsNumber(TraceInterval);
	FText traceRangeText = FText::AsNumber(TraceRange);

	FText traceDebugData = FText::Format(
		NSLOCTEXT("InteractorTraceDebugData", "Format", "\nTrace Type: {0}\nTrace Interval: {1}\nTrace Range: {2}"),
		traceTypeText, traceIntervalText, traceRangeText
	);

	return FText::Format(
		NSLOCTEXT("InteractorTraceDebugData", "CombinedFormat", "{0}{1}"),
		baseDebugData, traceDebugData
	).ToString();
}

void UActorInteractorComponentTrace::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UActorInteractorComponentTrace, TraceType,									COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentTrace, TraceInterval,								COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentTrace, TraceRange,									COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentTrace, TraceShapeHalfSize,					COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentTrace, bUseCustomStartTransform,		COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentTrace, CustomTraceTransform,				COND_OwnerOnly);
}

void UActorInteractorComponentTrace::DisableTracing_Server_Implementation()
{
	DisableTracing();
}

void UActorInteractorComponentTrace::EnableTracing_Server_Implementation()
{
	EnableTracing();
}

void UActorInteractorComponentTrace::PauseTracing_Server_Implementation()
{
	PauseTracing();
}

void UActorInteractorComponentTrace::ResumeTracing_Server_Implementation()
{
	ResumeTracing();
}

void UActorInteractorComponentTrace::ProcessTrace_Server_Implementation()
{
	ProcessTrace();
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