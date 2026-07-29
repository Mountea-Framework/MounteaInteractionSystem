// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Components/Interactor/MounteaInteractorComponentMouse.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Helpers/MounteaInteractionSystemLog.h"

#define LOCTEXT_NAMESPACE "MounteaInteractorComponentMouse"

UMounteaInteractorComponentMouse::UMounteaInteractorComponentMouse()
{
}

void UMounteaInteractorComponentMouse::BeginPlay()
{
	Super::BeginPlay();
}

void UMounteaInteractorComponentMouse::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UMounteaInteractorComponentMouse, MouseTraceRange,    COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractorComponentMouse, MouseTraceInterval, COND_OwnerOnly);
}

FString UMounteaInteractorComponentMouse::ToString_Implementation() const
{
	return FString::Printf(TEXT("%s | Mouse %.0fcm @%.2fs"),
		*Super::ToString_Implementation(), MouseTraceRange, MouseTraceInterval);
}

void UMounteaInteractorComponentMouse::ProcessStateChanged()
{
	Super::ProcessStateChanged();

	const bool bShouldTrace =
		InteractorState == EInteractorStateV3::EIS_Scanning ||
		InteractorState == EInteractorStateV3::EIS_Active;

	if (bShouldTrace)
	{
		EnableMouseTracing_Implementation();
	}
	else
	{
		DisableMouseTracing_Implementation();
	}
}

#pragma region MouseTracing

void UMounteaInteractorComponentMouse::EnableMouseTracing_Implementation()
{
	if (!GetOwner())
				return;

	if (!GetOwner()->HasAuthority())
	{
		EnableMouseTracing_Server();
		return;
	}

	if (!GetWorld())
				return;
	if (GetWorld()->GetTimerManager().IsTimerActive(Timer_MouseTicking))
				return;

	GetWorld()->GetTimerManager().SetTimer(
		Timer_MouseTicking,
		this,
		&UMounteaInteractorComponentMouse::ProcessMouseTrace,
		FMath::Max(0.01f, MouseTraceInterval),
		true,
		0.f
	);
}

void UMounteaInteractorComponentMouse::DisableMouseTracing_Implementation()
{
	if (!GetOwner())
				return;

	if (!GetOwner()->HasAuthority())
	{
		DisableMouseTracing_Server();
		return;
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(Timer_MouseTicking);
	}

	ClearDetected();
	LastMouseTraceHits.Reset();
}

bool UMounteaInteractorComponentMouse::CanMouseTrace_Implementation() const
{
	return GetOwner() != nullptr &&
		GetWorld() != nullptr &&
		GetOwnerPlayerController() != nullptr &&
		(InteractorState == EInteractorStateV3::EIS_Scanning || InteractorState == EInteractorStateV3::EIS_Active);
}

void UMounteaInteractorComponentMouse::ProcessMouseTrace()
{
	if (!CanMouseTrace())
				return;

	APlayerController* playerController = GetOwnerPlayerController();
	if (!playerController)
				return;

	FVector worldOrigin;
	FVector worldDirection;
	if (!playerController->DeprojectMousePositionToWorld(worldOrigin, worldDirection))
	{
		LastMouseTraceHits.Reset();
		PostMouseTraced_Implementation();
		return;
	}

	const FVector traceEnd = worldOrigin + worldDirection * MouseTraceRange;
	const ECollisionChannel traceChannel = (ECollisionChannel)CollisionChannel.GetValue();

	FCollisionQueryParams collisionParams;
	collisionParams.MobilityType = EQueryMobilityType::Any;
	collisionParams.bReturnPhysicalMaterial = false;
	collisionParams.AddIgnoredActor(GetOwner());

	for (AActor* ignored : ListOfIgnoredActors)
	{
		if (IsValid(ignored)) collisionParams.AddIgnoredActor(ignored);
	}

	FHitResult hitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, worldOrigin, traceEnd, traceChannel, collisionParams);

	LastMouseTraceHits.Reset();

	if (bHit && IsValid(hitResult.GetActor()))
	{
		TArray<UActorComponent*> interactableComps = hitResult.GetActor()->GetComponentsByInterface(UMounteaInteractableInterface::StaticClass());

		for (UActorComponent* comp : interactableComps)
		{
			if (!IsValid(comp))
				continue;

			TScriptInterface<IMounteaInteractableInterface> interactable;
			interactable.SetObject(comp);
			interactable.SetInterface(Cast<IMounteaInteractableInterface>(comp));

			if (!interactable.GetObject() || !interactable.GetInterface())
				continue;
			if (interactable->Execute_GetCollisionChannel(comp) != traceChannel)
				continue;

			LastMouseTraceHits.Add(interactable);
		}

		if (!GetOwner()->HasAuthority())
		{
			TArray<AActor*> hitActors;
			if (IsValid(hitResult.GetActor())) hitActors.Add(hitResult.GetActor());
			ReportHitActors_Server(hitActors);
		}
	}
	else if (!GetOwner()->HasAuthority())
	{
		ReportHitActors_Server(TArray<AActor*>());
	}

	PostMouseTraced_Implementation();
	OnMouseTraced.Broadcast();
}

void UMounteaInteractorComponentMouse::PostMouseTraced_Implementation()
{
	TArray<TScriptInterface<IMounteaInteractableInterface>> toAdd;
	TArray<TScriptInterface<IMounteaInteractableInterface>> toRemove;

	for (const TScriptInterface<IMounteaInteractableInterface>& hit : LastMouseTraceHits)
	{
		if (!DetectedSet.Contains(hit)) toAdd.Add(hit);
	}

	for (const TScriptInterface<IMounteaInteractableInterface>& existing : DetectedSet)
	{
		if (!LastMouseTraceHits.Contains(existing)) toRemove.Add(existing);
	}

	for (const auto& item : toAdd)    { AddDetected(item); }
	for (const auto& item : toRemove) { RemoveDetected(item); }
}

APlayerController* UMounteaInteractorComponentMouse::GetOwnerPlayerController() const
{
	if (!GetOwner())
				return nullptr;

	if (APlayerController* pc = Cast<APlayerController>(GetOwner()))
				return pc;

	if (APawn* pawn = Cast<APawn>(GetOwner()))
	{
		return Cast<APlayerController>(pawn->GetController());
	}

	return nullptr;
}

#pragma endregion

#pragma region Accessors

float UMounteaInteractorComponentMouse::GetMouseTraceRange() const { return MouseTraceRange; }

void UMounteaInteractorComponentMouse::SetMouseTraceRange_Implementation(float NewRange)
{
	if (!GetOwner())
				return;
	if (!GetOwner()->HasAuthority()) { SetMouseTraceRange_Server(NewRange); return; }

	MouseTraceRange = FMath::Max(1.f, NewRange);
}

float UMounteaInteractorComponentMouse::GetMouseTraceInterval() const { return MouseTraceInterval; }

void UMounteaInteractorComponentMouse::SetMouseTraceInterval_Implementation(float NewInterval)
{
	if (!GetOwner())
				return;
	if (!GetOwner()->HasAuthority()) { SetMouseTraceInterval_Server(NewInterval); return; }

	MouseTraceInterval = FMath::Max(0.01f, NewInterval);

	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(Timer_MouseTicking))
	{
		GetWorld()->GetTimerManager().SetTimer(
			Timer_MouseTicking,
			this,
			&UMounteaInteractorComponentMouse::ProcessMouseTrace,
			MouseTraceInterval,
			true,
			MouseTraceInterval
		);
	}
}

#pragma endregion

#pragma region Networking

void UMounteaInteractorComponentMouse::EnableMouseTracing_Server_Implementation()
{
	EnableMouseTracing_Implementation();
}

void UMounteaInteractorComponentMouse::DisableMouseTracing_Server_Implementation()
{
	DisableMouseTracing_Implementation();
}

void UMounteaInteractorComponentMouse::SetMouseTraceRange_Server_Implementation(float NewRange)
{
	SetMouseTraceRange_Implementation(NewRange);
}

void UMounteaInteractorComponentMouse::SetMouseTraceInterval_Server_Implementation(float NewInterval)
{
	SetMouseTraceInterval_Implementation(NewInterval);
}

void UMounteaInteractorComponentMouse::ReportHitActors_Server_Implementation(const TArray<AActor*>& HitActors)
{
	const ECollisionChannel traceChannel = (ECollisionChannel)CollisionChannel.GetValue();

	LastMouseTraceHits.Reset();
	for (AActor* actor : HitActors)
	{
		if (!IsValid(actor))
				continue;

		TArray<UActorComponent*> interactableComps = actor->GetComponentsByInterface(UMounteaInteractableInterface::StaticClass());
		for (UActorComponent* comp : interactableComps)
		{
			if (!IsValid(comp))
				continue;

			TScriptInterface<IMounteaInteractableInterface> interactable;
			interactable.SetObject(comp);
			interactable.SetInterface(Cast<IMounteaInteractableInterface>(comp));

			if (!interactable.GetObject() || !interactable.GetInterface())
				continue;
			if (interactable->Execute_GetCollisionChannel(comp) != traceChannel)
				continue;

			LastMouseTraceHits.Add(interactable);
		}
	}

	PostMouseTraced_Implementation();
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
