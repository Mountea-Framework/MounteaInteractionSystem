// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "Components/Interactable/MounteaInteractableComponentHover.h"
#include "Helpers/MounteaInteractionHelpers.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableComponentHover"

UMounteaInteractableComponentHover::UMounteaInteractableComponentHover()
{
	bInteractionHighlight = true;
	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = 3.f;
	InteractableName = NSLOCTEXT("MounteaInteractableComponentHover", "Hover", "Hover");
}

void UMounteaInteractableComponentHover::BeginPlay()
{
	Super::BeginPlay();

	OnInteractorOverlapped.										RemoveDynamic(this, &UMounteaInteractableComponentHover::OnInteractableBeginOverlapEvent);
	OnInteractorStopOverlap.										RemoveDynamic(this, &UMounteaInteractableComponentHover::OnInteractableStopOverlapEvent);
	OnInteractorTraced.												RemoveDynamic(this, &UMounteaInteractableComponentHover::OnInteractableTracedEvent);
}

void UMounteaInteractableComponentHover::BindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const
{
	Super::BindCollisionShape_Implementation(PrimitiveComponent);

	if (PrimitiveComponent)
	{
		PrimitiveComponent->OnBeginCursorOver.		AddUniqueDynamic(this, &UMounteaInteractableComponentHover::OnHoverBeginsEvent);
		PrimitiveComponent->OnEndCursorOver.			AddUniqueDynamic(this, &UMounteaInteractableComponentHover::OnHoverStopsEvent);
	}
}

void UMounteaInteractableComponentHover::UnbindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const
{
	Super::UnbindCollisionShape_Implementation(PrimitiveComponent);

	if (PrimitiveComponent)
	{
		PrimitiveComponent->OnBeginCursorOver.		RemoveDynamic(this, &UMounteaInteractableComponentHover::OnHoverBeginsEvent);
		PrimitiveComponent->OnEndCursorOver.			RemoveDynamic(this, &UMounteaInteractableComponentHover::OnHoverStopsEvent);
	}
}

bool UMounteaInteractableComponentHover::CanInteract_Implementation() const
{
	return Super::CanInteract_Implementation() && OverlappingComponent != nullptr;
}

void UMounteaInteractableComponentHover::OnHoverBeginsEvent(UPrimitiveComponent* PrimitiveComponent)
{
	OverlappingComponent = PrimitiveComponent;
	OnCursorBeginsOverlap.Broadcast(PrimitiveComponent);
}

void UMounteaInteractableComponentHover::OnHoverStopsEvent(UPrimitiveComponent* PrimitiveComponent)
{
	OverlappingComponent = nullptr;
	OnCursorStopsOverlap.Broadcast(PrimitiveComponent);
}

#undef LOCTEXT_NAMESPACE
