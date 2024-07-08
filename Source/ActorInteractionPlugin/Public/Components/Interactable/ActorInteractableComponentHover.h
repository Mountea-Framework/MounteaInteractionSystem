// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractableComponentHold.h"
#include "Components/ActorComponent.h"
#include "ActorInteractableComponentHover.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCursorBeginsOverlap, UPrimitiveComponent*, PrimitiveComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCursorStopsOverlap, UPrimitiveComponent*, PrimitiveComponent);

UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Hover"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentHover : public UActorInteractableComponentHold
{
	GENERATED_BODY()

public:

	UActorInteractableComponentHover();

protected:

	virtual void BeginPlay() override;

	virtual void BindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const override;
	virtual void UnbindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const override;

	virtual bool CanInteract_Implementation() const override;

	
	UFUNCTION()
	void OnHoverBeginsEvent(UPrimitiveComponent* PrimitiveComponent);
	UFUNCTION()
	void OnHoverStopsEvent(UPrimitiveComponent* PrimitiveComponent);

protected:
	
	/**
	 * Event called once collision is overlapped by Cursor.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactable")
	FOnCursorBeginsOverlap OnCursorBeginsOverlap;
	/**
	 * Event called once collision is no longer overlapped by Cursor.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactable")
	FOnCursorStopsOverlap OnCursorStopsOverlap;

protected:

	UPROPERTY(SaveGame, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	UPrimitiveComponent* OverlappingComponent = nullptr;
};
