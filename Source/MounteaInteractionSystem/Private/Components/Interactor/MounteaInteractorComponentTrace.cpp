// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Components/Interactor/MounteaInteractorComponentTrace.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Helpers/MounteaInteractionSystemLog.h"
#include "Helpers/MounteaInteractionHelpers.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "MounteaInteractorComponentTrace"

UMounteaInteractorComponentTrace::UMounteaInteractorComponentTrace()
{
	bUseCustomStartTransform = 0;

	SafetyTraceSetup.SafetyTracingMode = ESafetyTracingMode::ESTM_Location;
}

void UMounteaInteractorComponentTrace::BeginPlay()
{
	// Initialize LastTracingData from properties before Super (which calls SetDefaults)
	LastTracingData.TracingType = TraceType;
	LastTracingData.TracingInterval = TraceInterval;
	LastTracingData.TracingRange = TraceRange;
	LastTracingData.TracingShapeHalfSize = TraceShapeHalfSize;
	LastTracingData.bUsingCustomStartTransform = bUseCustomStartTransform;
	LastTracingData.CustomTracingTransform = CustomTraceTransform;

	Super::BeginPlay();
}

void UMounteaInteractorComponentTrace::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UMounteaInteractorComponentTrace, TraceType,              COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractorComponentTrace, TraceInterval,           COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractorComponentTrace, TraceRange,              COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractorComponentTrace, TraceShapeHalfSize,      COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractorComponentTrace, bUseCustomStartTransform,COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractorComponentTrace, CustomTraceTransform,    COND_OwnerOnly);
}

FString UMounteaInteractorComponentTrace::ToString_Implementation() const
{
	const FString traceTypeName = (TraceType == EMounteaTraceType::ETT_Precise) ? TEXT("Precise") : TEXT("Loose");
	return FString::Printf(TEXT("%s | Trace: %s %.0fcm @%.2fs"),
		*Super::ToString_Implementation(), *traceTypeName, TraceRange, TraceInterval);
}

void UMounteaInteractorComponentTrace::ProcessStateChanged()
{
	Super::ProcessStateChanged();

	const bool bShouldTrace =
		InteractorState == EInteractorStateV3::EIS_Scanning ||
		InteractorState == EInteractorStateV3::EIS_Active;

	if (bShouldTrace)
	{
		EnableTracing_Implementation();
	}
	else
	{
		DisableTracing_Implementation();
	}
}

#pragma region TracingControl

void UMounteaInteractorComponentTrace::EnableTracing_Implementation()
{
	if (!GetOwner())
				return;

	if (!GetOwner()->HasAuthority())
	{
		EnableTracing_Server();
		return;
	}

	if (!GetWorld())
				return;

	if (GetWorld()->GetTimerManager().IsTimerActive(Timer_Ticking))
				return;

	GetWorld()->GetTimerManager().SetTimer(
		Timer_Ticking,
		this,
		&UMounteaInteractorComponentTrace::ProcessTrace_Implementation,
		FMath::Max(0.01f, TraceInterval),
		true,
		0.f
	);
}

void UMounteaInteractorComponentTrace::DisableTracing_Implementation()
{
	if (!GetOwner())
				return;

	if (!GetOwner()->HasAuthority())
	{
		DisableTracing_Server();
		return;
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(Timer_Ticking);
	}

	ClearDetected();
	LastTraceHits.Reset();
}

void UMounteaInteractorComponentTrace::PauseTracing_Implementation()
{
	if (!GetOwner())
				return;

	if (!GetOwner()->HasAuthority())
	{
		PauseTracing_Server();
		return;
	}

	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(Timer_Ticking))
	{
		GetWorld()->GetTimerManager().PauseTimer(Timer_Ticking);
	}
}

void UMounteaInteractorComponentTrace::ResumeTracing_Implementation()
{
	if (!GetOwner())
				return;

	if (!GetOwner()->HasAuthority())
	{
		ResumeTracing_Server();
		return;
	}

	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerPaused(Timer_Ticking))
	{
		GetWorld()->GetTimerManager().UnPauseTimer(Timer_Ticking);
	}
}

bool UMounteaInteractorComponentTrace::CanTrace_Implementation() const
{
	return GetOwner() != nullptr &&
		GetWorld() != nullptr &&
		(InteractorState == EInteractorStateV3::EIS_Scanning || InteractorState == EInteractorStateV3::EIS_Active);
}

#pragma endregion

#pragma region TraceExecution

void UMounteaInteractorComponentTrace::ProcessTrace_Implementation()
{
	if (!CanTrace())
				return;
	if (!GetOwner() || !GetWorld())
				return;

	FInteractionTraceDataV2 traceData;
	traceData.CollisionChannel = (ECollisionChannel)CollisionChannel.GetValue();
	traceData.CollisionParams.MobilityType = EQueryMobilityType::Any;
	traceData.CollisionParams.bReturnPhysicalMaterial = true;
	traceData.CollisionParams.AddIgnoredActor(GetOwner());

	for (AActor* ignored : ListOfIgnoredActors)
	{
		if (IsValid(ignored)) traceData.CollisionParams.AddIgnoredActor(ignored);
	}

	if (bUseCustomStartTransform)
	{
		traceData.StartLocation = CustomTraceTransform.GetLocation();
		traceData.TraceRotation = CustomTraceTransform.GetRotation().Rotator();
	}
	else
	{
		GetOwner()->GetActorEyesViewPoint(traceData.StartLocation, traceData.TraceRotation);
	}

	traceData.EndLocation = traceData.StartLocation + traceData.TraceRotation.Vector() * TraceRange;

#if WITH_EDITOR
	DrawTracingDebugStart(traceData);
#endif

	switch (TraceType)
	{
		case EMounteaTraceType::ETT_Precise: ProcessTrace_Precise(traceData); break;
		case EMounteaTraceType::ETT_Loose:
		default:                              ProcessTrace_Loose(traceData);   break;
	}

	// Build LastTraceHits from this tick's results
	LastTraceHits.Reset();
	for (const FHitResult& hit : traceData.HitResults)
	{
		if (!hit.GetActor())
				continue;

		TArray<UActorComponent*> interactableComps = hit.GetActor()->GetComponentsByInterface(UMounteaInteractableInterface::StaticClass());
		for (UActorComponent* comp : interactableComps)
		{
			if (!IsValid(comp))
				continue;

			TScriptInterface<IMounteaInteractableInterface> interactable;
			interactable.SetObject(comp);
			interactable.SetInterface(Cast<IMounteaInteractableInterface>(comp));

			if (!interactable.GetObject() || !interactable.GetInterface())
				continue;

			if (interactable->Execute_GetCollisionChannel(comp) != traceData.CollisionChannel)
				continue;

			if (!Execute_PerformSafetyTrace(this, hit.GetActor()))
				continue;

			LastTraceHits.Add(interactable);
		}
	}

#if WITH_EDITOR
	DrawTracingDebugEnd(traceData);
#endif

	PostTraced_Implementation();
	OnTraced.Broadcast();
}

void UMounteaInteractorComponentTrace::ProcessTrace_Precise(FInteractionTraceDataV2& TraceData)
{
	FHitResult singleHit;
	if (GetWorld()->LineTraceSingleByChannel(
		singleHit,
		TraceData.StartLocation,
		TraceData.EndLocation,
		TraceData.CollisionChannel,
		TraceData.CollisionParams))
	{
		TraceData.HitResults.Add(singleHit);
	}
}

void UMounteaInteractorComponentTrace::ProcessTrace_Loose(FInteractionTraceDataV2& TraceData)
{
	const FCollisionShape boxShape = FCollisionShape::MakeBox(FVector(TraceShapeHalfSize));

	GetWorld()->SweepMultiByChannel(
		TraceData.HitResults,
		TraceData.StartLocation,
		TraceData.EndLocation,
		TraceData.TraceRotation.Quaternion(),
		TraceData.CollisionChannel,
		boxShape,
		TraceData.CollisionParams
	);
}

void UMounteaInteractorComponentTrace::PostTraced_Implementation()
{
	// Diff LastTraceHits against DetectedSet: add new, remove stale
	TArray<TScriptInterface<IMounteaInteractableInterface>> toAdd;
	TArray<TScriptInterface<IMounteaInteractableInterface>> toRemove;

	for (const TScriptInterface<IMounteaInteractableInterface>& hit : LastTraceHits)
	{
		if (!DetectedSet.Contains(hit)) toAdd.Add(hit);
	}

	for (const TScriptInterface<IMounteaInteractableInterface>& existing : DetectedSet)
	{
		if (!LastTraceHits.Contains(existing)) toRemove.Add(existing);
	}

	for (const auto& item : toAdd)    { AddDetected(item); }
	for (const auto& item : toRemove) { RemoveDetected(item); }
}

#pragma endregion

#pragma region Accessors

EMounteaTraceType UMounteaInteractorComponentTrace::GetTraceType() const { return TraceType; }

void UMounteaInteractorComponentTrace::SetTraceType_Implementation(const EMounteaTraceType& NewTraceType)
{
	if (!GetOwner())
				return;
	if (!GetOwner()->HasAuthority()) { SetTraceType_Server(NewTraceType); return; }

	const FTracingData oldData = LastTracingData;
	TraceType = NewTraceType;
	LastTracingData.TracingType = NewTraceType;
	OnTraceDataChanged.Broadcast(LastTracingData, oldData);
}

float UMounteaInteractorComponentTrace::GetTraceInterval() const { return TraceInterval; }

void UMounteaInteractorComponentTrace::SetTraceInterval_Implementation(const float NewInterval)
{
	if (!GetOwner())
				return;
	if (!GetOwner()->HasAuthority()) { SetTraceInterval_Server(NewInterval); return; }

	const FTracingData oldData = LastTracingData;
	TraceInterval = FMath::Max(0.01f, NewInterval);
	LastTracingData.TracingInterval = TraceInterval;
	OnTraceDataChanged.Broadcast(LastTracingData, oldData);

	// Restart timer at new rate if active
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(Timer_Ticking))
	{
		GetWorld()->GetTimerManager().SetTimer(
			Timer_Ticking,
			this,
			&UMounteaInteractorComponentTrace::ProcessTrace_Implementation,
			TraceInterval,
			true,
			TraceInterval
		);
	}
}

float UMounteaInteractorComponentTrace::GetTraceRange() const { return TraceRange; }

void UMounteaInteractorComponentTrace::SetTraceRange_Implementation(const float NewRange)
{
	if (!GetOwner())
				return;
	if (!GetOwner()->HasAuthority()) { SetTraceRange_Server(NewRange); return; }

	const FTracingData oldData = LastTracingData;
	TraceRange = FMath::Max(1.f, NewRange);
	LastTracingData.TracingRange = TraceRange;
	OnTraceDataChanged.Broadcast(LastTracingData, oldData);
}

float UMounteaInteractorComponentTrace::GetTraceShapeHalfSize() const { return TraceShapeHalfSize; }

void UMounteaInteractorComponentTrace::SetTraceShapeHalfSize_Implementation(const float NewTraceShapeHalfSize)
{
	if (!GetOwner())
				return;
	if (!GetOwner()->HasAuthority()) { SetTraceShapeHalfSize_Server(NewTraceShapeHalfSize); return; }

	const FTracingData oldData = LastTracingData;
	TraceShapeHalfSize = FMath::Max(0.1f, NewTraceShapeHalfSize);
	LastTracingData.TracingShapeHalfSize = TraceShapeHalfSize;
	OnTraceDataChanged.Broadcast(LastTracingData, oldData);
}

bool UMounteaInteractorComponentTrace::GetUseCustomStartTransform() const { return bUseCustomStartTransform != 0; }

void UMounteaInteractorComponentTrace::SetUseCustomStartTransform_Implementation(const bool bUse)
{
	if (!GetOwner())
				return;
	if (!GetOwner()->HasAuthority()) { SetUseCustomStartTransform_Server(bUse); return; }

	const FTracingData oldData = LastTracingData;
	bUseCustomStartTransform = bUse ? 1 : 0;
	LastTracingData.bUsingCustomStartTransform = bUseCustomStartTransform;
	OnTraceDataChanged.Broadcast(LastTracingData, oldData);
}

FTracingData UMounteaInteractorComponentTrace::GetLastTracingData() const { return LastTracingData; }

void UMounteaInteractorComponentTrace::SetCustomTraceStart_Implementation(const FTransform& TraceStart)
{
	if (!GetOwner())
				return;
	if (!GetOwner()->HasAuthority()) { SetCustomTraceStart_Server(TraceStart); return; }

	const FTracingData oldData = LastTracingData;
	CustomTraceTransform = TraceStart;
	LastTracingData.CustomTracingTransform = TraceStart;

	if (SafetyTraceSetup.SafetyTracingMode == ESafetyTracingMode::ESTM_Location)
	{
		SafetyTraceSetup.StartLocation = TraceStart.GetLocation();
	}

	OnTraceDataChanged.Broadcast(LastTracingData, oldData);
}

FTransform UMounteaInteractorComponentTrace::GetCustomTraceStart() const { return CustomTraceTransform; }

#pragma endregion

#pragma region NetworkingImpl

void UMounteaInteractorComponentTrace::DisableTracing_Server_Implementation()  { DisableTracing_Implementation(); }
void UMounteaInteractorComponentTrace::EnableTracing_Server_Implementation()   { EnableTracing_Implementation(); }
void UMounteaInteractorComponentTrace::PauseTracing_Server_Implementation()    { PauseTracing_Implementation(); }
void UMounteaInteractorComponentTrace::ResumeTracing_Server_Implementation()   { ResumeTracing_Implementation(); }
void UMounteaInteractorComponentTrace::ProcessTrace_Server_Implementation()    { ProcessTrace_Implementation(); }

void UMounteaInteractorComponentTrace::SetTraceType_Server_Implementation(const EMounteaTraceType& NewTraceType) { SetTraceType_Implementation(NewTraceType); }
void UMounteaInteractorComponentTrace::SetTraceInterval_Server_Implementation(float NewInterval)                 { SetTraceInterval_Implementation(NewInterval); }
void UMounteaInteractorComponentTrace::SetTraceRange_Server_Implementation(float NewRange)                       { SetTraceRange_Implementation(NewRange); }
void UMounteaInteractorComponentTrace::SetTraceShapeHalfSize_Server_Implementation(float NewSize)               { SetTraceShapeHalfSize_Implementation(NewSize); }
void UMounteaInteractorComponentTrace::SetUseCustomStartTransform_Server_Implementation(bool bUse)              { SetUseCustomStartTransform_Implementation(bUse); }
void UMounteaInteractorComponentTrace::SetCustomTraceStart_Server_Implementation(const FTransform& T)           { SetCustomTraceStart_Implementation(T); }

#pragma endregion

#if WITH_EDITOR

void UMounteaInteractorComponentTrace::DrawTracingDebugStart(FInteractionTraceDataV2& TraceData) const
{
	if (!DebugSettings.DebugMode || !GetWorld())
				return;

	switch (TraceType)
	{
		case EMounteaTraceType::ETT_Precise:
			DrawDebugLine(GetWorld(), TraceData.StartLocation, TraceData.EndLocation, FColor::Blue, false, TraceInterval, 0, 0.25f);
			break;
		case EMounteaTraceType::ETT_Loose:
			DrawDebugSphere(GetWorld(), TraceData.StartLocation, TraceShapeHalfSize, 6, FColor::Blue, false, TraceInterval, 0, 0.25f);
			break;
	}

	DrawDebugSphere(GetWorld(), TraceData.EndLocation, 10.f, 6, FColor::Red, false, TraceInterval, 0, 0.25f);
}

void UMounteaInteractorComponentTrace::DrawTracingDebugEnd(FInteractionTraceDataV2& TraceData) const
{
	if (!DebugSettings.DebugMode || !GetWorld())
				return;

	for (const FHitResult& hit : TraceData.HitResults)
	{
		DrawDebugSphere(GetWorld(), hit.ImpactPoint, 10.f, 6, FColor::Green, false, TraceInterval, 0, 0.25f);
	}
}

void UMounteaInteractorComponentTrace::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName propertyName = PropertyChangedEvent.GetPropertyName();

	if (propertyName == GET_MEMBER_NAME_CHECKED(UMounteaInteractorComponentTrace, TraceInterval))
	{
		TraceInterval = FMath::Max(0.01f, TraceInterval);
	}
	else if (propertyName == GET_MEMBER_NAME_CHECKED(UMounteaInteractorComponentTrace, TraceRange))
	{
		TraceRange = FMath::Max(1.f, TraceRange);
	}
	else if (propertyName == GET_MEMBER_NAME_CHECKED(UMounteaInteractorComponentTrace, TraceShapeHalfSize))
	{
		TraceShapeHalfSize = FMath::Max(0.1f, TraceShapeHalfSize);
	}
}

#endif

#undef LOCTEXT_NAMESPACE
