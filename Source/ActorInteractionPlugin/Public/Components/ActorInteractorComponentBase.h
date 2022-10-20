// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/ActorInteractorInterface.h"
#include "ActorInteractorComponentBase.generated.h"


UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactor Component"))
class ACTORINTERACTIONPLUGIN_API UActorInteractorComponentBase : public UActorComponent, public IActorInteractorInterface
{
	GENERATED_BODY()

public:

	UActorInteractorComponentBase();

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	/**
	 * Event bound to OnInteractableFound event.
	 * Once OnInteractableFound is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param FoundInteractable Interactable Component which is found
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void OnInteractableFoundEvent(const TScriptInterface<IActorInteractableInterface>& FoundInteractable);
	
	/**
	 * Event bound to OnInteractableLost event.
	 * Once OnInteractableLost is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param LostInteractable Interactable Component which is lost
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void OnInteractableLostEvent(const TScriptInterface<IActorInteractableInterface>& LostInteractable);

	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractionKeyPressedEvent(const float TimeKeyPressed);

	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractionKeyReleasedEvent(const float TimeKeyReleased);

	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractorStateChanged();
	
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void StartInteraction() override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void StopInteraction() override;

	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool ActivateInteractor(FString& ErrorMessage) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void DeactivateInteractor() override;

	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractionDependency(const TScriptInterface<IActorInteractorInterface> NewInteractionDependency) override;

	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual TScriptInterface<IActorInteractorInterface> GetInteractionDependency() const override;

	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool CanInteract() const override;
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void TickInteraction(const float DeltaTime) override;

	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual ECollisionChannel GetResponseChannel() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetResponseChannel(const ECollisionChannel NewResponseChannel) override;


	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual EInteractorState GetState() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetState(const EInteractorState NewState) override;


	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool DoesAutoActivate() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction", meta=(DisplayName="Set Auto Activate"))
	virtual void SetDoesAutoActivate(const bool bNewAutoActivate) override;


	/**
	 * Returns Interaction Key for specified Platform.
	 * @param RequestedPlatform Name of platform you want to know the Interaction Key
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual FKey GetInteractionKey(FString& RequestedPlatform) const override;
	
	/**
	 * Sets or Updates Interaction Key for specified Platform.
	 * There is no validation for Keys validation! Nothing stops you from setting Keyboard keys for Consoles. Please, be careful with this variable!
	 * @param Platform Name of platform you want to set or update the Interaction Key
	 * @param NewInteractorKey The interaction key to setup.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractionKey(FString& Platform, const FKey NewInteractorKey) override;
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual TMap<FString, FKey> GetInteractionKeys() const override;

	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetActiveInteractable(const TScriptInterface<IActorInteractableInterface>& NewInteractable) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual TScriptInterface<IActorInteractableInterface> GetActiveInteractable() const override;

protected:

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractableFound OnInteractableFound;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractableLost OnInteractableLost;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionKeyPressed OnInteractionKeyPressed;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionKeyReleased OnInteractionKeyReleased;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FStateChanged OnStateChanged;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction|Required", meta=(DisplayName="Auto Activate", NoResetToDefault))
	uint8 bDoesAutoActivate : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction|Required", meta=(NoResetToDefault))
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	/**
	 * List of Interaction Keys for each platform.
	 * There is no validation for Keys validation! Nothing stops you from setting Keyboard keys for Consoles. Please, be careful with this variable!
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction|Required", meta=(NoResetToDefault))
	TMap<FString, FKey> InteractionKeyPerPlatform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction|Read Only", meta=(NoResetToDefault))
	EInteractorState InteractorState;

private:

	// Master which can suppress this interactor
	UPROPERTY()
	TScriptInterface<IActorInteractorInterface> InteractionDependency;
	
	// This is Interactable which is set as Active
	UPROPERTY()
	TScriptInterface<IActorInteractableInterface> ActiveInteractable;

	// List of Interactables, possibly all overlapping ones
	UPROPERTY()
	TArray<TScriptInterface<IActorInteractableInterface>> ListOfInteractables;
};
