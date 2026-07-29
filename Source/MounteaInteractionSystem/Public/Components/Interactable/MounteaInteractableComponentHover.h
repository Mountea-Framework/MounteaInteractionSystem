// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInteractableComponentHold.h"
#include "MounteaInteractableComponentHover.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCursorBeginsOverlap, UPrimitiveComponent*, PrimitiveComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCursorStopsOverlap, UPrimitiveComponent*, PrimitiveComponent);

/**
 * v5.0 Actor Interactable Hover Component
 *
 * Triggered by cursor hover instead of key presses.
 * Holds for InteractionPeriod seconds while cursor remains over collision shape.
 * Moving cursor away cancels the interaction.
 * Binds cursor events (OnBeginCursorOver / OnEndCursorOver) instead of overlap/hit events.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactable-Component-Hover
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName="Interactable Component Hover"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableComponentHover : public UMounteaInteractableComponentHold
{
	GENERATED_BODY()

public:

	UMounteaInteractableComponentHover();

protected:

	virtual void BindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const override;
	virtual void UnbindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const override;

	virtual bool CanInteract_Implementation() const override;

	UFUNCTION()
	void OnCursorBeginOverlapEvent(UPrimitiveComponent* PrimitiveComponent);

	UFUNCTION()
	void OnCursorEndOverlapEvent(UPrimitiveComponent* PrimitiveComponent);

protected:

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactable")
	FOnCursorBeginsOverlap OnCursorBeginsOverlap;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactable")
	FOnCursorStopsOverlap OnCursorStopsOverlap;

	UPROPERTY(SaveGame, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	TObjectPtr<UPrimitiveComponent> OverlappingComponent = nullptr;
};
