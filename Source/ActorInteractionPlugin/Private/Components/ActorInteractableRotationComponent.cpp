// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableRotationComponent.h"

#include "Kismet/KismetMathLibrary.h"

UActorInteractableRotationComponent::UActorInteractableRotationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	AngleLimit = 60.f;
	Interval = 0.2;
	InterpolationSpeed = 10.f;
	FacingType = ERotationFacingType::ERFT_ZOnlyLimited;

	EnableTick();
}

void UActorInteractableRotationComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentActivated.AddUniqueDynamic(this, &UActorInteractableRotationComponent::RotationActivated);
	OnComponentDeactivated.AddUniqueDynamic(this, &UActorInteractableRotationComponent::RotationDeactivated);
	OnFacingTypeChanged.AddUniqueDynamic(this, &UActorInteractableRotationComponent::OnFacingTypeChangedEvent);
	OnLookAtActorChanged.AddUniqueDynamic(this, &UActorInteractableRotationComponent::OnLookAtActorChangedEvent);
	
	EnableTick();

	CachedRotation = GetComponentRotation();
}

void UActorInteractableRotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CanRotate())
	{
		RotateComponent(DeltaTime);
	}
}

void UActorInteractableRotationComponent::OnFacingTypeChangedEvent()
{
	switch (FacingType)
	{
		case ERotationFacingType::ERFT_AllAxes:
		case ERotationFacingType::ERFT_ZOnly:
		case ERotationFacingType::ERFT_ZOnlyLimited:
			Activate();
			OnFacingTypeChangedEventBP();
			break;
		case ERotationFacingType::ERFT_None:
			Deactivate();
			OnFacingTypeChangedEventBP();
			break;
		case ERotationFacingType::ERFT_Default: 
		default:
			Deactivate();
			break;
	}
}

void UActorInteractableRotationComponent::OnLookAtActorChangedEvent(AActor* Actor)
{
	if (LookAtActor)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
	OnLookAtActorChangedEventBP(Actor);
}

void UActorInteractableRotationComponent::RotationActivated(UActorComponent* ActorComponent, bool bArg)
{
	EnableTick();
}

void UActorInteractableRotationComponent::RotationDeactivated(UActorComponent* ActorComponent)
{
	SetRelativeRotation(CachedRotation);
	
	EnableTick();
}

void UActorInteractableRotationComponent::EnableTick()
{
	switch (FacingType)
	{
		case ERotationFacingType::ERFT_AllAxes:
		case ERotationFacingType::ERFT_ZOnly:
		case ERotationFacingType::ERFT_ZOnlyLimited:
			PrimaryComponentTick.TickInterval = Interval;
			PrimaryComponentTick.SetTickFunctionEnable(true);
			break;
		case ERotationFacingType::ERFT_None:
		case ERotationFacingType::ERFT_Default: 
		default:
			PrimaryComponentTick.SetTickFunctionEnable(false);
			break;
	}
}

bool UActorInteractableRotationComponent::CanRotate() const
{
	switch (FacingType)
	{
		case ERotationFacingType::ERFT_AllAxes:
		case ERotationFacingType::ERFT_ZOnly:
		case ERotationFacingType::ERFT_ZOnlyLimited:
			return LookAtActor != nullptr;
		case ERotationFacingType::ERFT_None:
		case ERotationFacingType::ERFT_Default: 
		default:
			return false;
	}

	return false;
}

void UActorInteractableRotationComponent::RotateComponent(float DeltaTime)
{
	if (!LookAtActor)
	{
		OnLookAtActorChanged.Broadcast(nullptr);
		return;
	}

	LastRotation = GetComponentRotation();
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), LookAtActor->GetActorLocation());
		
	switch (FacingType)
	{
		case ERotationFacingType::ERFT_AllAxes:
			break;
		case ERotationFacingType::ERFT_ZOnly:
			NewRotation.Roll = 0.f;
			NewRotation.Pitch = 0.f;
			break;
		case ERotationFacingType::ERFT_ZOnlyLimited:
			NewRotation.Roll = 0.f;
			NewRotation.Pitch = 0.f;
			NewRotation.Yaw = FMath::ClampAngle
			(
				FMath::FindDeltaAngleDegrees(NewRotation.Yaw, CachedRotation.Yaw),
				-AngleLimit,
				AngleLimit
			);
			break;
	}
	
	SetWorldRotation( FMath::RInterpConstantTo(LastRotation, NewRotation, DeltaTime, InterpolationSpeed) );
}

void UActorInteractableRotationComponent::SetFacingType(const ERotationFacingType& NewType)
{
	FacingType = NewType;

	OnFacingTypeChanged.Broadcast();
}

void UActorInteractableRotationComponent::SetLookAtActor(AActor* Actor)
{
	if (LookAtActor != Actor)
	{
		LookAtActor = Actor;
		OnLookAtActorChanged.Broadcast(LookAtActor);
	}
}

AActor* UActorInteractableRotationComponent::GetLookAtActor() const
{ return LookAtActor; };

