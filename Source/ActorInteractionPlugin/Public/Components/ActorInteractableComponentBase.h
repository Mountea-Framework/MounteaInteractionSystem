// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Interfaces/ActorInteractableInterface.h"
#include "ActorInteractableComponentBase.generated.h"

enum class EInteractableLifecycle : uint8;

UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentBase : public UWidgetComponent, public IActorInteractableInterface
{
	GENERATED_BODY()

public:

	UActorInteractableComponentBase();

protected:

	virtual void BeginPlay() override;

protected:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool DoesAutoSetup() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void ToggleAutoSetup(const bool NewValue) override;

	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool ActivateInteractable(FString& ErrorMessage) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool WakeUpInteractable(FString& ErrorMessage) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool SnoozeInteractable(FString& ErrorMessage) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool CompleteInteractable(FString& ErrorMessage) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void DeactivateInteractable() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual EInteractableStateV2 GetState() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetState(const EInteractableStateV2 NewState) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TScriptInterface<IActorInteractorInterface> GetInteractor() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractor(const TScriptInterface<IActorInteractorInterface> NewInteractor) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual int32 GetInteractableWeight() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractableWeight(const int32 NewWeight) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual AActor* GetInteractableOwner() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractableOwner(AActor* NewOwner) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual ECollisionChannel GetCollisionChannel() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetCollisionChannel(const ECollisionChannel& NewChannel) override;


	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual EInteractableLifecycle GetLifecycleMode() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetLifecycleMode(const EInteractableLifecycle& NewMode) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual int32 GetLifecycleCount() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetLifecycleCount(const int32 NewLifecycleCount) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual float GetCooldownPeriod() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetCooldownPeriod(const float NewCooldownPeriod) override;

	
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<UPrimitiveComponent*> GetCollisionComponents() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddCollisionComponent(UPrimitiveComponent* CollisionComp) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddCollisionComponents(const TArray<UPrimitiveComponent*> NewCollisionComponents) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveCollisionComponent(UPrimitiveComponent* CollisionComp) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveCollisionComponents(const TArray<UPrimitiveComponent*> RemoveCollisionComponents) override;

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<UMeshComponent*> GetHighlightableComponents() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddHighlightableComponent(UMeshComponent* HighlightableComp) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddHighlightableComponents(const TArray<UMeshComponent*> AddHighlightableComponents) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveHighlightableComponent(UMeshComponent* HighlightableComp) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveHighlightableComponents(const TArray<UMeshComponent*> RemoveHighlightableComponents) override;

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual UMeshComponent* FindMeshByTag(const FName Tag) const override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual UPrimitiveComponent* FindPrimitiveByTag(const FName Tag) const override;

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<FName> GetCollisionOverrides() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddCollisionOverride(const FName Tag) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddCollisionOverrides(const TArray<FName> Tags) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveCollisionOverride(const FName Tag) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveCollisionOverrides(const TArray<FName> Tags) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<FName> GetHighlightableOverrides() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddHighlightableOverride(const FName Tag) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddHighlightableOverrides(const TArray<FName> Tags) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveHighlightableOverride(const FName Tag) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveHighlightableOverrides(const TArray<FName> Tags) override;

#pragma region EventFunctions

#pragma region NativeFunctions

	
	/**
	 * Event called once Interactor is found.
	 * Called by OnInteractorFound
	 * Calls OnInteractorFoundEvent
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractorFound(const TScriptInterface<IActorInteractorInterface>& FoundInteractor) override;

	/**
	 * Event called once Interactor is lost.
	 * Called by OnInteractorLost
	 * Calls OnInteractorLostEvent
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractorLost(const TScriptInterface<IActorInteractorInterface>& LostInteractor) override;

	/**
	 * Event called once Interaction is completed.
	 * Called from OnInteractionCompleted
	 * Calls OnInteractionCompletedEvent
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractionCompleted(const float& TimeCompleted) override;

	/**
	 * Event called once Interaction Starts.
	 * Called by OnInteractionStarted
	 * Calls OnInteractionStartedEvent
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractionStarted(const float& TimeStarted) override;

	/**
	 * Event called once Interaction Stops.
	 * Called by OnInteractionStopped
	 * Calls OnInteractionStoppedEvent
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractionStopped() override;

	/**
	 * Event called once Interaction Lifecycles is Completed.
	 * Called by OnLifecycleCompleted
	 * Calls OnLifecycleCompletedEvent
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractionLifecycleCompleted() override;

	/**
	 * Event called once Interaction Cooldown is Completed and Interactable can be Interacted with again.
	 * Called by OnCooldownCompleted
	 * Calls OnCooldownCompletedEvent
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractionCooldownCompleted() override;

	
	/**
	 * Function called once any of Collision Shapes is overlapped.
	 * Called by OnInteractorOverlapped
	 * Calls OnInteractableBeginOverlapEvent
	 * Overlap event is called only if:
	 * * Other Actor Implements Interactor Interface
	 * * Other Actor contains Component which Implements Interactor Interface
	 * * Interactor Interface does have same CollisionChannel as Interactable
	 */
	UFUNCTION(Category="Interaction")
	void OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/**
	 * Event called once any of Collision Shapes stops overlapping.
	 * Called by OnInteractorStopOverlap
	 * Calls OnInteractorStopOverlapEvent
	 * Stop overlap event is called only if:
	 * * Other Actor Implements Interactor Interface
	 * * Other Actor contains Component which Implements Interactor Interface
	 * * Interactor Interface does have same CollisionChannel as Interactable
	 */
	UFUNCTION(Category="Interaction")
	void OnInteractableStopOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**
	 * Event called once any of Collision Shapes is hit by trace.
	 * Called by OnInteractorTraced
	 * Calls OnInteractorTracedEvent
	 * Hit by trace event is called only if:
	 * * Other Actor Implements Interactor Interface
	 * * Other Actor contains Component which Implements Interactor Interface
	 * * Interactor Interface does have same CollisionChannel as Interactable
	 */
	UFUNCTION(Category="Interaction")
	void OnInteractableTraced(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

#pragma endregion

#pragma region InteractionEvents
	
	/**
	 * Event bound to OnInteractorFound.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractorFoundEvent(const TScriptInterface<IActorInteractorInterface>& FoundInteractor);

	/**
	 * Event bound to OnInteractorLost.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractorLostEvent(const TScriptInterface<IActorInteractorInterface>& LostInteractor);
	
	/**
	 * Event bound to OnInteractorOverlapped.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/**
	 * Event bound to OnInteractorStopOverlap.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableStopOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**
	 * Event bound to OnInteractorTraced.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableTracedEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/**
	 * Event bound to OnInteractionCompleted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractionCompletedEvent(const float& FinishTime);

	/**
	 * Event bound to OnInteractionStarted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractionStartedEvent(const float& StartTime);

	/**
	 * Event bound to OnInteractionStopped.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractionStoppedEvent();

	/**
	 * Event bound to OnLifecycleCompleted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnLifecycleCompletedEvent();

	/**
	 * Event bound to OnCooldownCompleted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnCooldownCompletedEvent();

#pragma endregion

#pragma region SetupHelpers
	
	/**
	 * Event bound to OnHighlightableOverrideAdded event.
	 * Once OnHighlightableOverrideAdded is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewTag Tag which has been found and added to Highlightable Meshes
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnHighlightableOverrideAddedEvent(const FName NewTag);

	/**
	 * Event bound to OnHighlightableOverrideRemoved event.
	 * Once OnHighlightableOverrideRemoved is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param RemovedTag Tag which has been found and removed from Highlightable Meshes
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnHighlightableOverrideRemovedEvent(const FName RemovedTag);
	
	/**
	 * Event bound to OnCollisionOverrideAdded event.
	 * Once OnCollisionOverrideAdded is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewTag Tag which has been found and added to list of Colliders 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnCollisionOverrideAddedEvent(const FName NewTag);

	/**
	 * Event bound to OnCollisionOverrideRemoved event.
	 * Once OnCollisionOverrideRemoved is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param RemovedTag Tag which has been found and removed from list of Colliders 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnCollisionOverrideRemovedEvent(const FName RemovedTag);

#pragma endregion 

#pragma region AttributesEvents

	/**
	 * Event bound to OnInteractableAutoSetupChanged event.
	 * Once OnInteractableAutoSetupChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewValue New value of the Auto Setup
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableAutoSetupChangedEvent(const bool NewValue);

	/**
	 * Event bound to OnInteractableWeightChanged event.
	 * Once OnInteractableWeightChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewWeight New value of the Interactable Weight
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableWeightChangedEvent(const int32& NewWeight);

	/**
	 * Event bound to OnInteractableStateChanged event.
	 * Once OnInteractableStateChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewState New value of the Interactable State
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableStateChangedEvent(const EInteractableStateV2& NewState);

	/**
	 * Event bound to OnInteractableOwnerChanged event.
	 * Once OnInteractableOwnerChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewOwner New value of the Interactable Owner
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableOwnerChangedEvent(const AActor* NewOwner);

	/**
	 * Event bound to OnInteractableCollisionChannelChanged event.
	 * Once OnInteractableCollisionChannelChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewChannel New value of the Interactable Collision Channel
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableCollisionChannelChangedEvent(const ECollisionChannel NewChannel);

	/**
	 * Event bound to OnLifecycleModeChanged event.
	 * Once OnLifecycleModeChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewMode New value of the Interactable Lifecycle
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnLifecycleModeChangedEvent(const EInteractableLifecycle& NewMode);

	/**
	 * Event bound to OnLifecycleCountChanged event.
	 * Once OnLifecycleCountChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewLifecycleCount New value of the Lifecycle Count
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnLifecycleCountChangedEvent(const int32 NewLifecycleCount);

	/**
	 * Event bound to OnCooldownPeriodChanged event.
	 * Once OnCooldownPeriodChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewCooldownPeriod New value of the Cooldown Period
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnCooldownPeriodChangedEvent(const float NewCooldownPeriod);

	/**
	 * Event bound to OnHighlightableComponentAdded event.
	 * Once OnHighlightableComponentAdded is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewHighlightableComp New Highlightable Component added to list of Highlightables 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnHighlightableComponentAddedEvent(const UMeshComponent* NewHighlightableComp);

	/**
	 * Event bound to OnHighlightableComponentRemoved event.
	 * Once OnHighlightableComponentRemoved is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param RemovedHighlightableComp Highlightable Component removed from the list of Highlightables 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnHighlightableComponentRemovedEvent(const UMeshComponent* RemovedHighlightableComp);

	/**
	 * Event bound to OnCollisionComponentAdded event.
	 * Once OnCollisionComponentAdded is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewCollisionComp New Collision Component added to list of Colliders 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnCollisionComponentAddedEvent(const UPrimitiveComponent* NewCollisionComp);

	/**
	 * Event bound to OnCollisionComponentRemoved event.
	 * Once OnCollisionComponentRemoved is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param RemovedCollisionComp Collision Component removed from list of Colliders 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnCollisionComponentRemovedEvent(const UPrimitiveComponent* RemovedCollisionComp);

#pragma endregion 

#pragma endregion

#pragma region InteractionHelpers

	virtual void FindAndAddCollisionShapes() override;
	virtual void FindAndAddHighlightableMeshes() override;

	/**
	 * Binds Collision Events for specified Primitive Component.
	 * Automatically called when Interactable is:
	 * * Awaken
	 * * Asleep
	 */
	virtual void BindCollisionEvents(UPrimitiveComponent* PrimitiveComponent) const override;
	
	/**
	 * Unbinds Collision Events for specified Primitive Component.
	 * Automatically called when Interactable is:
	 * * Deactivated
	 * * Finished
	 * * Cooldown
	 */
	virtual void UnbindCollisionEvents(UPrimitiveComponent* PrimitiveComponent) const override;

	/**
	 * Binds Highlightable Events for specified Mesh Component.
	 * Automatically called when Interactable is:
	 * * Awaken
	 * * Asleep
	 */
	virtual void BindHighlightable(UMeshComponent* MeshComponent) const override;

	/**
	 * Unbinds Highlightable Events for specified Mesh Component.
	 * Automatically called when Interactable is:
	 * * Deactivated
	 * * Finished
	 * * Cooldown
	 */
	virtual void UnbindHighlightable(UMeshComponent* MeshComponent) const override;
	
	/**
	 * Development Only.
	 * Toggles debug On/Off.
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Interaction", meta=(DevelopmentOnly))
	virtual void ToggleDebug() override
	{
		bToggleDebug = !bToggleDebug;
	}

	/**
	 * Helper function.
	 * Looks for Collision and Highlightable Overrides and binds them.
	 * Looks for Parent Component which will be set as Collision Mesh and Highlighted Mesh.
	 */
	UFUNCTION()
	void AutoSetup();

#pragma endregion 

protected:
	
	/**
	 * Event called once any of Collision Shapes is hit by trace.
	 * Hit by trace event is called only if:
	 * * Other Actor Implements Interactor Interface
	 * * Other Actor contains Component which Implements Interactor Interface
	 * * Interactor Interface does have same CollisionChannel as Interactable
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractorTraced OnInteractorTraced;

	/**
	 * Event called once any of Collision Shapes is overlapped.
	 * Overlap event is called only if:
	 * * Other Actor Implements Interactor Interface
	 * * Other Actor contains Component which Implements Interactor Interface
	 * * Interactor Interface does have same CollisionChannel as Interactable
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractorOverlapped OnInteractorOverlapped;
	
	/**
	 * Event called once any of Collision Shapes stops overlapping.
	 * Stop overlap event is called only if:
	 * * Other Actor Implements Interactor Interface
	 * * Other Actor contains Component which Implements Interactor Interface
	 * * Interactor Interface does have same CollisionChannel as Interactable
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractorStopOverlap OnInteractorStopOverlap;


	/**
	 * Event called once Interactor is found.
	 * Called by OnInteractorFound
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractorFound OnInteractorFound;

	/**
	 * Event called once Interactor is lost.
	 * Called by OnInteractorLost
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractorLost OnInteractorLost;

	/**
	 * Event called once Interaction is completed.
	 * Called from OnInteractionCompleted
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionCompleted OnInteractionCompleted;

	/**
	 * Event called once Interaction Starts.
	 * Called by OnInteractionStarted
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionStarted OnInteractionStarted;

	/**
	 * Event called once Interaction Stops.
	 * Called by OnInteractionStopped
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionStopped OnInteractionStopped;

	/**
	 * Event called once Interaction Lifecycles is Completed.
	 * Called by OnLifecycleCompleted
	 */	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FLifecycleCompleted OnLifecycleCompleted;

	/**
	 * Event called once Interaction Cooldown is Completed and Interactable can be Interacted with again.
	 * Called by OnCooldownCompleted
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCooldownCompleted OnCooldownCompleted;

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableAutoSetupChanged OnInteractableAutoSetupChanged;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableWeightChanged OnInteractableWeightChanged;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableStateChanged OnInteractableStateChanged;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableOwnerChanged OnInteractableOwnerChanged;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableCollisionChannelChanged OnInteractableCollisionChannelChanged;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FLifecycleModeChanged OnLifecycleModeChanged;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FLifecycleCountChanged OnLifecycleCountChanged;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCooldownPeriodChanged OnCooldownPeriodChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FHighlightableComponentAdded OnHighlightableComponentAdded;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FHighlightableComponentRemoved OnHighlightableComponentRemoved;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionComponentAdded OnCollisionComponentAdded;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionComponentRemoved OnCollisionComponentRemoved;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FHighlightableOverrideAdded OnHighlightableOverrideAdded;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FHighlightableOverrideRemoved OnHighlightableOverrideRemoved;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionOverrideAdded OnCollisionOverrideAdded;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionOverrideRemoved OnCollisionOverrideRemoved;

#pragma region Attributes
	
protected:

#pragma region Debug

	/**
	 * If active, debug can be drawn.
	 * Serves a general purpose as a flag.
	 * Does not affect Shipping builds by default C++ implementation.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Debug")
	uint8 bToggleDebug : 1;

#pragma endregion

#pragma region Required

	/**
	 * Default state of the Interactable to be set in BeginPlay.
	 * Cannot be set to:
	 * * Active
	 * * Finished
	 * * Cooldown
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	EInteractableStateV2 DefaultInteractableState;
	
	/**
	 * If set to true, Interactable will automatically assigns owning Component in Hierarchy as Highlightable Meshes and Collision Shapes.
	 * This setup might be useful for simple Actors, might cause issues with more complex ones.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required")
	uint8 bInteractableAutoSetup : 1;

	/**
	 * Collision Channel which will be forced to all Collision Shapes as Overlap.
	 * All Collision Shapes will be updated to generate Overlap events.
	 *
	 * Could be either Trace or Object response.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	/**
	 * Weight of this Interactable.
	 * Useful with multiple overlapping Interactables withing the same Actor. Interactor will always prefer the one with highest Weight value.
	 *
	 * Default value: 1
	 * Clamped in setter function to be at least 0 or higher.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(UIMin=0, ClampMin=0))
	int32 InteractionWeight;

	/**
	 * Defines Lifecycle Mode of this Interactable.
	 * Cycled:
	 * * Can be used multiple times
	 * * Good for NPCs
	 * Once:
	 * * Can be used only once
	 * * Good for pickup items
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	EInteractableLifecycle LifecycleMode;

	/**
	 * How many times this Interactable can be used.
	 *
	 * Clamped in Setter.
	 * Expected range:
	 * * -1 | Can be used forever
	 * *  0 | Invalid, will be set to 2
	 * *  1 | Invalid, will be set to 2
	 * * 2+ | Will be used defined number of times
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault, EditCondition = "LifecycleMode == EInteractableLifecycle::EIL_Cycled", UIMin=-1, ClampMin=-1))
	int32 LifecycleCount;

	/**
	 * How long it takes for Cooldown to finish.
	 * After this period of time the Interactable will be Awake again, unless no Interactor.
	 * Clamped in Setter.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault, EditCondition = "LifecycleMode == EInteractableLifecycle::EIL_Cycled", UIMin=0.1, ClampMin=0.1))
	float CooldownPeriod;

#pragma endregion

#pragma region Optional
	
	/**
	 * Expects: Actor Tags
	 * List of Actor Tags which define which Primitive Components should be added to Collision Shapes.
	 * This way, you can easily specify what Components will act as Collision Shapes, and without using any nodes.
	 *
	 * Is used even with Auto Setup.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Optional", meta=(NoResetToDefault))
	TArray<FName> CollisionOverrides;

	/**
	 * Expects: Actor Tags
	 * List of Actor Tags which define which Mesh Components should be added to Highlightable Meshes.
	 * This way, you can easily specify what Components will act as Highlightable Meshes, and without using any nodes.
	 *
	 * Is used even with Auto Setup.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Optional", meta=(NoResetToDefault))
	TArray<FName> HighlightableOverrides;

	/**
	 * Defines whether Interactable should be highlighted with defined Material when Interaction is possible.
	 *
	 * For this option to work following steps must be done:
	 * * In Level must be PostProcessVolume
	 * * PostProcessVolume must have set PostProcessMaterials
	 * * * Custom Depth-Stencil Pass must be enabled
	 * * * * Projects Settings -> Engine -> Rendering -> PostProcessing
	 * * * * * Set to value: Enabled with Stencil
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="Interaction|Optional")
	uint8 bInteractionHighlight : 1;
	
	/**
	 * Defines what Stencil ID should be used to highlight the Primitive Mesh Components.
	 * In order to smoothly integrate with other logic, keep this ID unique!
	 * Default: 133
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="Interaction|Optional", meta=(EditCondition="bInteractionHighlight == true", UIMin=0, ClampMin=0, UIMax=255, ClampMax=255))
	int32 StencilID ;

#pragma endregion 

#pragma region ReadOnly
	
	/**
	 * Current State of the Interactable.
	 * Is subject to State Machine.
	 * @see [State Machine] https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Validations#state-machine
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only", meta=(NoResetToDefault))
	EInteractableStateV2 InteractableState;
	
	/**
	 * List of Highlightable Components.
	 * * Set Overlap Events to true
	 * * Response to Collision Channel to overlap
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only", meta=(NoResetToDefault))
	TArray<UMeshComponent*> HighlightableComponents;
	
	/**
	 * List of Collision Components.
	 * Those component will be updated to:
	 * * Allow Render Custom Depth
	 * * Use specific StencilID
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only", meta=(NoResetToDefault))
	TArray<UPrimitiveComponent*> CollisionComponents;

private:

	/**
	 * Owning Actor of this Interactable.
	 * By default its set to Owner.
	 * Can be overriden.
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only", meta=(NoResetToDefault, DisplayThumbnail = false))
	AActor* InteractionOwner;

	/**
	 * Interactor which is using this Interactable.
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only", meta=(NoResetToDefault, DisplayThumbnail = false))
	TScriptInterface<IActorInteractorInterface> Interactor;

#pragma endregion

#pragma endregion 
};
