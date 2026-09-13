// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Components/Interactable/MounteaInteractableComponentHover.h"
#include "Interfaces/MounteaInteractorInterface.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "Helpers/MounteaInteractionStatics.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableComponentHover"

UMounteaInteractableComponentHover::UMounteaInteractableComponentHover()
{
	OverlappingComponent = nullptr;
}

void UMounteaInteractableComponentHover::BindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const
{
	if (!IsValid(PrimitiveComponent))
				return;

	// Hover uses cursor events - do NOT bind overlap/hit callbacks from base
	PrimitiveComponent->OnBeginCursorOver.AddUniqueDynamic(
		const_cast<UMounteaInteractableComponentHover*>(this),
		&UMounteaInteractableComponentHover::OnCursorBeginOverlapEvent);

	PrimitiveComponent->OnEndCursorOver.AddUniqueDynamic(
		const_cast<UMounteaInteractableComponentHover*>(this),
		&UMounteaInteractableComponentHover::OnCursorEndOverlapEvent);
}

void UMounteaInteractableComponentHover::UnbindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const
{
	if (!IsValid(PrimitiveComponent))
				return;

	PrimitiveComponent->OnBeginCursorOver.RemoveDynamic(
		const_cast<UMounteaInteractableComponentHover*>(this),
		&UMounteaInteractableComponentHover::OnCursorBeginOverlapEvent);

	PrimitiveComponent->OnEndCursorOver.RemoveDynamic(
		const_cast<UMounteaInteractableComponentHover*>(this),
		&UMounteaInteractableComponentHover::OnCursorEndOverlapEvent);
}

bool UMounteaInteractableComponentHover::CanInteract_Implementation() const
{
	return IsValid(OverlappingComponent) && Super::CanInteract_Implementation();
}

void UMounteaInteractableComponentHover::OnCursorBeginOverlapEvent(UPrimitiveComponent* PrimitiveComponent)
{
	if (!IsValid(PrimitiveComponent))
				return;

	OverlappingComponent = PrimitiveComponent;
	OnCursorBeginsOverlap.Broadcast(PrimitiveComponent);

	// Cursor entering = interactor "found" - no actual interactor object for hover, drive interaction directly
	if (Execute_CanInteract(this))
	{
		FString err;
		Execute_ActivateInteractable(this, err);

		// Simulate interaction start with server time
		const float now = UMounteaInteractionStatics::GetServerTime(this);
		TScriptInterface<IMounteaInteractorInterface> emptyInteractor;
		Execute_InteractionStarted(this, now, emptyInteractor);
	}
}

void UMounteaInteractableComponentHover::OnCursorEndOverlapEvent(UPrimitiveComponent* PrimitiveComponent)
{
	OnCursorStopsOverlap.Broadcast(PrimitiveComponent);
	OverlappingComponent = nullptr;

	const float now = UMounteaInteractionStatics::GetServerTime(this);
	TScriptInterface<IMounteaInteractorInterface> emptyInteractor;
	Execute_InteractionStopped(this, now, emptyInteractor);
}

#undef LOCTEXT_NAMESPACE
