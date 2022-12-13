// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractableComponentHold.h"
#include "Components/ActorComponent.h"
#include "ActorInteractableComponentHover.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCursorBeginsOverlap, UPrimitiveComponent*, PrimitiveComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCursorStopsOverlap, UPrimitiveComponent*, PrimitiveComponent);

UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Hover"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentHover : public UActorInteractableComponentHold
{
	GENERATED_BODY()

public:

	UActorInteractableComponentHover();

protected:

	virtual void BeginPlay() override;

	virtual void BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const override;
	virtual void UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const override;

	virtual bool CanInteract() const override;

	
	UFUNCTION()
	void OnHoverBeginsEvent(UPrimitiveComponent* PrimitiveComponent);
	UFUNCTION()
	void OnHoverStopsEvent(UPrimitiveComponent* PrimitiveComponent);

protected:
	
	/**
	 * Event called once collision is overlapped by Cursor.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnCursorBeginsOverlap OnCursorBeginsOverlap;
	/**
	 * Event called once collision is no longer overlapped by Cursor.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnCursorStopsOverlap OnCursorStopsOverlap;

protected:

	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only")
	UPrimitiveComponent* OverlappingComponent = nullptr;
};
