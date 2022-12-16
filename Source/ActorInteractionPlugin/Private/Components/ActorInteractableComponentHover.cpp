// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentHover.h"

#define LOCTEXT_NAMESPACE "ActorInteractableComponentHover"

UActorInteractableComponentHover::UActorInteractableComponentHover()
{
	bInteractionHighlight = true;
	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = 3.f;
	InteractableName = LOCTEXT("ActorInteractableComponentHover", "Hover");

	InteractionKeysPerPlatform.Empty();
	const FInteractionKeySetup GamepadKeys = FKey("Gamepad_FaceButton_Bottom");
	FInteractionKeySetup KeyboardKeys = GamepadKeys;
	KeyboardKeys.Keys.Add("LeftMouseButton");
		
	InteractionKeysPerPlatform.Add((TEXT("Windows")), KeyboardKeys);
	InteractionKeysPerPlatform.Add((TEXT("Mac")), KeyboardKeys);
	InteractionKeysPerPlatform.Add((TEXT("PS4")), GamepadKeys);
	InteractionKeysPerPlatform.Add((TEXT("XboxOne")), GamepadKeys);
}

void UActorInteractableComponentHover::BeginPlay()
{
	Super::BeginPlay();

	OnInteractorOverlapped.RemoveDynamic(this, &UActorInteractableComponentHover::OnInteractableBeginOverlapEvent);
	OnInteractorStopOverlap.RemoveDynamic(this, &UActorInteractableComponentHover::OnInteractableStopOverlapEvent);
	OnInteractorTraced.RemoveDynamic(this, &UActorInteractableComponentHover::OnInteractableTracedEvent);
}

void UActorInteractableComponentHover::BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const
{
	Super::BindCollisionShape(PrimitiveComponent);

	if (PrimitiveComponent)
	{
		PrimitiveComponent->OnBeginCursorOver.AddUniqueDynamic(this, &UActorInteractableComponentHover::OnHoverBeginsEvent);
		PrimitiveComponent->OnEndCursorOver.AddUniqueDynamic(this, &UActorInteractableComponentHover::OnHoverStopsEvent);
	}
}

void UActorInteractableComponentHover::UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const
{
	Super::UnbindCollisionShape(PrimitiveComponent);

	if (PrimitiveComponent)
	{
		PrimitiveComponent->OnBeginCursorOver.RemoveDynamic(this, &UActorInteractableComponentHover::OnHoverBeginsEvent);
		PrimitiveComponent->OnEndCursorOver.RemoveDynamic(this, &UActorInteractableComponentHover::OnHoverStopsEvent);
	}
}

bool UActorInteractableComponentHover::CanInteract() const
{
	return Super::CanInteract() && OverlappingComponent != nullptr;
}

void UActorInteractableComponentHover::OnHoverBeginsEvent(UPrimitiveComponent* PrimitiveComponent)
{
	OverlappingComponent = PrimitiveComponent;
	OnCursorBeginsOverlap.Broadcast(PrimitiveComponent);
}

void UActorInteractableComponentHover::OnHoverStopsEvent(UPrimitiveComponent* PrimitiveComponent)
{
	OverlappingComponent = nullptr;
	OnCursorStopsOverlap.Broadcast(PrimitiveComponent);
}

#undef LOCTEXT_NAMESPACE
