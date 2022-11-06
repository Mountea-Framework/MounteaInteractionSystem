// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/ActorInteractorInterface.h"
#include "ActorInteractorComponentBase.generated.h"

/**
 * Actor Interactor Base Component
 *
 * Implements ActorInteractorInterface.
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactor-Component-Base
 */
UCLASS(Abstract, ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation, Rendering), meta=(BlueprintSpawnableComponent, DisplayName = "Interactor Component"))
class ACTORINTERACTIONPLUGIN_API UActorInteractorComponentBase : public UActorComponent, public IActorInteractorInterface
{
	GENERATED_BODY()

public:

	UActorInteractorComponentBase();

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Handles

public:

	virtual FInteractableSelected& GetOnInteractableSelectedHandle() override
	{ return OnInteractableSelected; };
	virtual FInteractableFound& GetOnInteractableFoundHandle() override
	{ return OnInteractableFound; };
	virtual FInteractableLost& GetOnInteractableLostHandle() override
	{ return OnInteractableLost; };
	virtual FInteractionKeyPressed& OnInteractionKeyPressedHandle() override
	{ return OnInteractionKeyPressed; };
	virtual FInteractionKeyReleased& OnInteractionKeyReleasedHandle() override
	{ return OnInteractionKeyReleased; };

#pragma endregion
	
protected:

#pragma region NativeFunctions

	UFUNCTION()
	virtual void InteractableSelected(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable) override;
	UFUNCTION()
	virtual void InteractableFound(const TScriptInterface<IActorInteractableInterface>& FoundInteractable) override;
	UFUNCTION()
	virtual void InteractableLost(const TScriptInterface<IActorInteractableInterface>& LostInteractable) override;

#pragma endregion 

#pragma region Events
	
	/**
	 * Event bound to OnInteractableSelected event.
	 * Once OnInteractableSelected is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param SelectedInteractable Interactable Component which is being interacted with
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableSelectedEvent(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable);

	/**
	 * Event bound to OnInteractableFound event.
	 * Once OnInteractableFound is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param FoundInteractable Interactable Component which is found
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableFoundEvent(const TScriptInterface<IActorInteractableInterface>& FoundInteractable);
	
	/**
	 * Event bound to OnInteractableLost event.
	 * Once OnInteractableLost is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param LostInteractable Interactable Component which is lost
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableLostEvent(const TScriptInterface<IActorInteractableInterface>& LostInteractable);

	/**
	 * Event bound to OnInteractionKeyPressed event.
	 * Once OnInteractionKeyPressed is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param TimeKeyPressed Time Key was pressed
	 * @param PressedKey Key which was pressed
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void OnInteractionKeyPressedEvent(const float& TimeKeyPressed, const FKey& PressedKey);

	/**
	 * Event bound to OnInteractionKeyReleased event.
	 * Once OnInteractionKeyReleased is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param TimeKeyReleased Time Key was released
	 * @param ReleasedKey Key which was released
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void OnInteractionKeyReleasedEvent(const float& TimeKeyReleased, const FKey& ReleasedKey);

	/**
	 * Event bound to OnStateChanged event.
	 * Once OnStateChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewState New State if this Interactor
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractorStateChanged(const EInteractorStateV2& NewState);

	/**
	 * Event bound to OnCollisionChanged event.
	 * Once OnCollisionChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewCollisionChannel New Collision Channel set as Response Channel
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractorCollisionChanged(const TEnumAsByte<ECollisionChannel>& NewCollisionChannel);

	/**
	 * Event bound to OnAutoActivateChanged event.
	 * Once OnAutoActivateChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewAutoActivate Whether this Interactor is Auto Activated or not
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractorAutoActivateChanged(const bool NewAutoActivate);

#pragma endregion

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool IsValidInteractor() const override;
	
	/**
	 * Compares Interactables.
	 * If with the same Owner, then by Weight.
	 */
	UFUNCTION()
	virtual void EvaluateInteractable(const TScriptInterface<IActorInteractableInterface>& FoundInteractable) override;


	/**
	 * Function to start interaction.
	 * Interaction will start only if CanInteract function evaluates true.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void StartInteraction(const float StartTime, FKey InputKey) override;

	/**
	 * Function to stop interaction.
	 * No conditions required.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void StopInteraction() override;


	/**
	 * Tries to Active Interactor by setting Interactor state to Active.
	 * Returns whether it was successful or not.
	 * Either way, Error Message contains useful information what possibly went wrong.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool ActivateInteractor(FString& ErrorMessage) override;

	/**
	 * Tries to Wake Up Interactor by setting Interactor state to Stand By.
	 * Returns whether it was successful or not.
	 * Either way, Error Message contains useful information what possibly went wrong.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool WakeUpInteractor(FString& ErrorMessage) override;

	/**
	 * Tries to Suppress Interactor by setting Interactor state to Suppressed.
	 * Returns whether it was successful or not.
	 * Either way, Error Message contains useful information what possibly went wrong.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool SuppressInteractor(FString& ErrorMessage) override;
	
	/**
	 * Deactivates Interactor.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void DeactivateInteractor() override;

	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddIgnoredActor(AActor* IgnoredActor) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddIgnoredActors(const TArray<AActor*> IgnoredActors) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveIgnoredActor(AActor* UnignoredActor) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveIgnoredActors(const TArray<AActor*> UnignoredActors) override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<AActor*> GetIgnoredActors() const override;


	/**
	 * Adds unique Interaction Dependency.
	 * Those Interactors will be suppressed until this one is not interacting anymore.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddInteractionDependency(const TScriptInterface<IActorInteractorInterface> InteractionDependency) override;

	/**
	 * Removes unique Interaction Dependency.
	 * Removed Dependencies won't be suppressed and can interrupt with this Interactor.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveInteractionDependency(const TScriptInterface<IActorInteractorInterface> InteractionDependency) override;

	/**
	 * Returns all Interaction Dependencies.
	 * Those Interactors will be suppressed until this one is not interacting anymore.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<TScriptInterface<IActorInteractorInterface>> GetInteractionDependencies() const override;

	/**
	 * 
	 */
	UFUNCTION(Category="Interaction")
	virtual void ProcessDependencies() override;
	
	/**
	 * Returns whether this Interactor can interact or not.
	 * Calls Internal CanInteract which is implemented in C++.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category="Interaction")
	bool CanInteractEvent() const;

	/**
	 * Optimized request for Interactor.
	 * Can be overriden in C++ for specific class needs.
	 */
	UFUNCTION()
	virtual bool CanInteract() const override;
	
	/**
	 * Interactor specific Tick.
	 * Calls Internal TickInteraction which is implemented in C++.
	 * All Interaction logic should be processed in this function instead of regular Tick!
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void TickInteractionEvent(const float DeltaTime);

	/**
	 * Optimized Tick for Interactor.
	 * Can be overriden in C++ for specific class needs.
	 */
	UFUNCTION()
	virtual void TickInteraction(const float DeltaTime) override;

	/**
	 * Returns Interactor Response Channel.
	 * This is the channel Interactor is interacting with.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual ECollisionChannel GetResponseChannel() const override;

	/**
	 * Sets Collision Channel as Response Channel for this Interactor.
	 * Tip: Use custom Collision Channel for Interaction types.
	 * * Interaction Overlap
	 * * Interaction Trace
	 * * Interaction Hover
	 * * etc.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetResponseChannel(const ECollisionChannel NewResponseChannel) override;

	
	/**
	 * Returns Interactor State.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,Category="Interaction")
	virtual EInteractorStateV2 GetState() const override;

	/**
	 * Tries to set Interactor State.
	 * State machine apply!
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetState(const EInteractorStateV2 NewState) override;


	/**
	 * Returns whether Interactor auto activates or not.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,Category="Interaction")
	virtual bool DoesAutoActivate() const override;

	/**
	 * Set Interactor to be auto activated.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction", meta=(DisplayName="Set Auto Activate"))
	virtual void ToggleAutoActivate(const bool bNewAutoActivate) override;


	/**
	 * Sets specified or nul Interactable to be Active Interactable.
	 */
	UFUNCTION(Category="Interaction")
	virtual void SetActiveInteractable(const TScriptInterface<IActorInteractableInterface> NewInteractable) override;

	/**
	 * Returns Active Interactable if there is any. Otherwise nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TScriptInterface<IActorInteractableInterface> GetActiveInteractable() const override;

	/**
	 * Development Only.
	 * Toggles debug On/Off.
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Interaction", meta=(DevelopmentOnly))
	virtual void ToggleDebug() override
	{
		bToggleDebug = !bToggleDebug;
	}

protected:

	/**
	 * This event is called once this Interactor finds its best Interactable.
	 * Interactor can iterate over multiple Interactables from the same Actor.
	 *
	 * Example:
	 * Interactor overlaps with a chest of drawers. There are multiple interactables for each drawer and for items within them.
	 * However, drawers have higher weight, thus always suppress items, unless specified otherwise.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractableSelected OnInteractableSelected;

	/**
	 * This event is called once this Interactor finds any Interactable.
	 * This event might happen for multiple Interactables. Each one is compared and if fit it is fed to OnInteractableSelected.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractableFound OnInteractableFound;

	/**
	 * This event is called one this Interactor loose its Active Interactable.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractableLost OnInteractableLost;

	/**
	 * This event should be called once starting the Interaction Action is requested and valid Key is pressed.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionKeyPressed OnInteractionKeyPressed;

	/**
	 * This event should be called once stopping the Interaction Action is requested and valid Key is released.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionKeyReleased OnInteractionKeyReleased;

	/**
	 * This event is called once SetState function sets new State.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FStateChanged OnStateChanged;

	/**
	 * This event is called once SetResponseChannel set new Collision Channel.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionChanged OnCollisionChanged;

	/**
	 * This event is called once ToggleAutoActivate sets new value.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FAutoActivateChanged OnAutoActivateChanged;

	/**
	 * This event is called once Ignored Actor is successfully added to List of Ignored Actors. 
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FIgnoredActorAdded OnIgnoredActorAdded;

	/**
	 * This event is called once Ignored Actor is successfully removed from List of Ignored Actors. 
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FIgnoredActorRemoved OnIgnoredActorRemoved;

protected:

	/**
	 * If active, debug can be drawn.
	 * Serves a general purpose as a flag.
	 * Does not affect Shipping builds by default C++ implementation.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Debug")
	uint8 bToggleDebug : 1;

	/**
	 * If auto activation is true, component will start as Awake and can start immediately interact.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(DisplayName="Auto Activate", NoResetToDefault))
	uint8 bDoesAutoActivate : 1;
	
	/**
	 * Response Collision Channel this Interactor is interacting against.
	 * Tip: Use custom Collision Channel for Interaction types.
	 * * Interaction Overlap
	 * * Interaction Trace
	 * * Interaction Hover
	 * * etc.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
	/**
	 * New and easier way to set Default State.
	 * This state will be propagated to Interactor State.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Optional", meta=(NoResetToDefault))
	EInteractorStateV2 DefaultInteractorState;

	/**
	 * A list of Actors that won't be taken in count when tracing.
	 * If left empty, only Owner Actor is ignored.
	 * If using multiple Actors (a gun, for instance), all those child/attached Actors should be ignored.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Optional", meta=(NoResetToDefault, DisplayThumbnail=false))
	TArray<AActor*> ListOfIgnoredActors;

private:

	/**
	 * Current read-only State of this Interactor.
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only", meta=(NoResetToDefault))
	EInteractorStateV2 InteractorState;
	
	// This is Interactable which is set as Active
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only")
	TScriptInterface<IActorInteractableInterface> ActiveInteractable;
	
	// List of interactors suppressed by this one
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only")
	TArray<TScriptInterface<IActorInteractorInterface>> InteractionDependencies;
};
