// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Helpers/InteractionHelpers.h"

#include "ActorInteractableWidget.generated.h"

class UTextBlock;
class UImage;
class UBorder;

class UActorInteractableComponent;

/**
 * Implement an Interactable Actor Widget.
 *
 * Allows to show interaction progress and basic interaction info to be displayed.
 * Does work with Interactable Actor Component out of the box.
 * @note	Consists of:
 * - Interaction Progress Bar Border
 * - Interaction Action Name
 * - Interaction Actor Name
 * - Interaction Key
 *
 * @see [InteractableWidget](https://sites.google.com/view/dominikpavlicek/home/documentation)
 */
UCLASS()
class ACTORINTERACTIONPLUGIN_API UActorInteractableWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Custom Initialize to pass down some required values for Widget to work correctly.
	 * @note Widget won't work unless Initialized.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void InitializeInteractionWidget(const FText& NewInteractableKey, const FText& NewInteractableName, const FText& NewInteractionAction,
	                                 UActorInteractableComponent* NewOwningComponent, UTexture2D* NewInteractionTexture);

	/**
	 * Use this Event to Toggle Visibility;
	 * Call Parent function to ensure caching previous Visibility!
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void ToggleVisibility();

	virtual bool Initialize() override;

	/**
	 * Calculates Remaining percentage to update the Widget progress bar.
	 * @param InDeltaTime	Delta time to make interaction framerate independent.
	 */
	void CalculateRemainingTime(float InDeltaTime);

	/**
	 * Returns owning Interactable Component or any of its child classes.
	 * @return	Owning Component of UActorInteractableComponent
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	FORCEINLINE UActorInteractableComponent* GetOwningComponent() const {return OwningComponent; };

	/**
	 * Returns values between 0 and 1, where 1 means 100% and 0 means 0%
	 * @return InteractionProgress float
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	FORCEINLINE float GetInteractionProgress() const {return InteractionProgress;	};

	/**
	 * This function is responsible for keeping Interactable Widget up to date with Interaction timing.
	 * 
	 * @note	Do not use this function unless you really want to set values by hand.
	 * @param NewInteractionProgress	A value of Interaction Progress to be set manually. Will be clamped to fit between 0 and 1.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void SetInteractionProgress(float NewInteractionProgress);

	/**
	 * Whenever Interaction Progress is updated, this event will be called.
	 * This event should be responsible for visual updates of any progress bars/animations.
	 *
	 * @param DeltaInteractionProgress Percentage of the Interaction Progress, should be plugged into progress bar or similar functionality.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnInteractionProgressChanged(float DeltaInteractionProgress);

	/**
	 * Helper function that returns Interactable Key Text Block.
	 * @return Interactable Key
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	FORCEINLINE UTextBlock* GetInteractableKey() const {return InteractableKey; };

	/**
	 * Helper function that returns Interactable Name Text Block.
	 * @return	Interactable Name
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	FORCEINLINE UTextBlock* GetInteractableName() const {return InteractableName; };

	/**
	 * Helper function that returns Interactable Action Text Block.
	 * @return	Interactable Action
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	FORCEINLINE UTextBlock* GetInteractionAction() const {return InteractionAction; };

	/**
	 * Helper function that returns Interactable Action Texture.
	 * @return Interactable Texture
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	FORCEINLINE UImage* GetInteractionTexture() const {return InteractionTexture; };
	/**
	 * Event called when Owning Component requests re-draw of the Widget.
	 * Should be responsible for getting data from Owning Component and setting those properly.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnUpdateInteractionWidget();
	
protected:
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
protected:
	
	/**
	 * Value of the Key to be pressed for Interaction to start.
	 */
	UPROPERTY(Category="Properties", BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* InteractableKey = nullptr;
	
	/**
	 * Name of the Object that is interactable. If left empty, nothing will be displayed.
	 */
	UPROPERTY(Category="Properties", BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* InteractableName = nullptr;

	/**
	 * Name of the Action that is required for interaction to start. If left empty, nothing will be displayed.
	 */
	UPROPERTY(Category="Properties", BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* InteractionAction = nullptr;

	/*
	 * Texture of the Action that displays for instance a Console button image.
	 * If left empty, nothing will be displayed.
	 */
	UPROPERTY(Category="Properties", BlueprintReadWrite, meta=(BindWidget))
	UImage* InteractionTexture = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Interaction|Debug")
	ESlateVisibility CachedVisibility;
	
private:

	/**
	 * Owning Interactable Actor Component.
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Debug")
	UActorInteractableComponent* OwningComponent = nullptr;

	/**
	 * Value between 0 and 1, that represents how far in the progress the interaction is.
	 * 0 means 0%
	 * 1 means 100%
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Debug", meta=(ClampMax=1, UIMax=1, ClampMin=0, UIMin=0))
	float InteractionProgress;

	float TimeRemainder;


};
