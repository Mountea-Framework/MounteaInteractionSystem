// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"

#include "Interfaces/ActorInteractableInterface.h"
#include "Helpers/InteractionHelpers.h"

#include "ActorInteractableComponentBase.generated.h"

USTRUCT(BlueprintType)
struct FCollisionShapeCache
{
	GENERATED_BODY()

	FCollisionShapeCache()
	{
		bGenerateOverlapEvents = false;
		CollisionEnabled = ECollisionEnabled::QueryOnly;
		CollisionResponse = ECR_Overlap;
	};

	FCollisionShapeCache(bool GeneratesOverlaps, TEnumAsByte<ECollisionEnabled::Type> collisionEnabled, TEnumAsByte<ECollisionResponse> collisionResponse) :
	bGenerateOverlapEvents(GeneratesOverlaps),
	CollisionEnabled(collisionEnabled),
	CollisionResponse(collisionResponse)
	{};

	UPROPERTY(VisibleAnywhere)
	uint8 bGenerateOverlapEvents : 1;
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled;
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ECollisionResponse> CollisionResponse;
	
};

UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentBase : public UWidgetComponent, public IActorInteractableInterface
{
	GENERATED_BODY()

public:

	UActorInteractableComponentBase();

protected:

	virtual void BeginPlay() override;

#pragma region InteractableFunctions
	
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

	/**
	 * Returns whether this Interactable can interacted with or not.
	 * Calls Internal CanInteract which is implemented in C++.
	 * Be sure to call Parent Event!
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category="Interaction", meta=(DisplayName = "Can Interact"))
	bool CanInteractEvent() const;

	/**
	 * Optimized request for Interactables.
	 * Can be overriden in C++ for specific class needs.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool CanInteract() const override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool CanBeTriggered() const override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool IsInteracting() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual EInteractableStateV2 GetState() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetState(const EInteractableStateV2 NewState) override;


	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<TSoftClassPtr<UObject>> GetIgnoredClasses() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetIgnoredClasses(const TArray<TSoftClassPtr<UObject>> NewIgnoredClasses) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddIgnoredClass(TSoftClassPtr<UObject> AddIgnoredClass) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddIgnoredClasses(TArray<TSoftClassPtr<UObject>> AddIgnoredClasses) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveIgnoredClass(TSoftClassPtr<UObject> RemoveIgnoredClass) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveIgnoredClasses(TArray<TSoftClassPtr<UObject>> RemoveIgnoredClasses) override;

	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddInteractionDependency(const TScriptInterface<IActorInteractableInterface> InteractionDependency) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveInteractionDependency(const TScriptInterface<IActorInteractableInterface> InteractionDependency) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual TArray<TScriptInterface<IActorInteractableInterface>> GetInteractionDependencies() const override;
	/**
	 * Function responsible for updating Interaction Dependencies.
	 * Does process all hooked up Interactables in predefined manner.
	 * Is called once State is updated.
	 */
	UFUNCTION(Category="Interaction")
	virtual void ProcessDependencies() override;


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
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual int32 GetRemainingLifecycleCount() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual float GetCooldownPeriod() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetCooldownPeriod(const float NewCooldownPeriod) override;


	/**
	 * Returns Interaction Key for specified Platform.
	 * @param RequestedPlatform Name of platform you want to know the Interaction Key
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual FKey GetInteractionKey(const FString& RequestedPlatform) const override;
	
	/**
	 * Sets or Updates Interaction Key for specified Platform.
	 * There is no validation for Keys validation! Nothing stops you from setting Keyboard keys for Consoles. Please, be careful with this variable!
	 * @param Platform Name of platform you want to set or update the Interaction Key
	 * @param NewInteractorKey The interaction key to setup.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractionKey(const FString& Platform, const FKey NewInteractorKey) override;

	/**
	 * Returns all Interaction Keys.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TMap<FString, FInteractionKeySetup> GetInteractionKeys() const override;

	/**
	 * Checks for Key in the list of Interaction keys.
	 * Returns true if defined, otherwise returns false.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool FindKey(const FKey& RequestedKey, const FString& Platform) const override;
	
	
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
	virtual void AddHighlightableComponent(UMeshComponent* MeshComponent) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddHighlightableComponents(const TArray<UMeshComponent*> AddMeshComponents) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveHighlightableComponent(UMeshComponent* MeshComponent) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveHighlightableComponents(const TArray<UMeshComponent*> RemoveMeshComponents) override;

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual UMeshComponent* FindMeshByTag(const FName Tag) const override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual UPrimitiveComponent* FindPrimitiveByTag(const FName Tag) const override;

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<FName> GetCollisionOverrides() const override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<FName> GetHighlightableOverrides() const override;


#pragma endregion

#pragma region EventFunctions

#pragma region NativeFunctions

	/**
	 * Event bound to Interactor's OnInteractableSelected.
	 * If Interactor selects any Interactable, this Event is called and selected Interactable is Activated, while others are set back to Awaken state.
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractableSelected(const TScriptInterface<IActorInteractableInterface>& Interactable) override;
	
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
	 * Event called once Interaction is Canceled.
	 * Called by OnInteractionCanceled
	 * Calls OnInteractionCanceledEvent
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractionCanceled() override;

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
	 * Event bound to OnInteractableSelected.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableSelectedEvent(const TScriptInterface<IActorInteractableInterface>& Interactable);
	
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
	 * Event bound to OnInteractionCanceled.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractionCanceledEvent();
	
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

#pragma region IgnoredClassesEvents

	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnIgnoredClassAdded(const TSoftClassPtr<UObject>& IgnoredClass);

	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnIgnoredClassRemoved(const TSoftClassPtr<UObject>& IgnoredClass);

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

	/**
	 * Event bound to OnInteractorChanged event.
	 * Once OnInteractorChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewInteractor Interactor set as new Interactor, could be nullptr
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractorChangedEvent(const TScriptInterface<IActorInteractorInterface>& NewInteractor);

#pragma endregion 

#pragma region InteractionHelpers

	virtual void FindAndAddCollisionShapes() override;
	virtual void FindAndAddHighlightableMeshes() override;

	virtual void TriggerCooldown() override;

	/**
	 * Binds Collision Events for specified Primitive Component.
	 * Caches Primitive Component collision settings.
	 * Automatically called when Interactable is:
	 * * Awaken
	 * * Asleep
	 */
	virtual void BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const override;
	
	/**
	 * Unbinds Collision Events for specified Primitive Component.
	 * Is using cached values to return Primitive Component to pre-interaction state.
	 * Automatically called when Interactable is:
	 * * Deactivated
	 * * Finished
	 * * Cooldown
	 */
	virtual void UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const override;

	/**
	 * Binds Highlightable Events for specified Mesh Component.
	 * Automatically called when Interactable is:
	 * * Awaken
	 * * Asleep
	 */
	virtual void BindHighlightableMesh(UMeshComponent* MeshComponent) const override;

	/**
	 * Unbinds Highlightable Events for specified Mesh Component.
	 * Automatically called when Interactable is:
	 * * Deactivated
	 * * Finished
	 * * Cooldown
	 */
	virtual void UnbindHighlightableMesh(UMeshComponent* MeshComponent) const override;
	
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

#pragma endregion

#pragma region Events
	
protected:

#pragma region InteractionReactionEvents
	
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

#pragma endregion

#pragma region IgnoredClasses

	/**
	 * Event called once Ignored Interactor Class is successfully added.
	 * Called by OnIgnoredInteractorClassAdded.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Interaction")
	FIgnoredInteractorClassAdded OnIgnoredInteractorClassAdded;

	/**
	 * Event called once Ignored Interactor Class is successfully removed.
	 * Called by OnIgnoredInteractorClassRemoved.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Interaction")
	FIgnoredInteractorClassRemoved OnIgnoredInteractorClassRemoved;

#pragma endregion 

#pragma region InteractionEvents

	/**
	 * Event called once Interactor is selected.
	 * Has native C++ implementation.
	 * Calls OnInteractableSelectedEvent.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FOnInteractableSelected OnInteractableSelected;

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
	 * Event called once Interaction is Canceled.
	 * Called by OnInteractionCanceled
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionCanceled OnInteractionCanceled;

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

#pragma endregion

#pragma region InteractionComponents

	/**
	 * Event called once Highlightable Component is added to Highlightable Meshes.
	 * Returns which MeshComponent has been added.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FHighlightableComponentAdded OnHighlightableComponentAdded;

	/**
	 * Event called once Highlightable Component is removed from Highlightable Meshes.
	 * Returns which MeshComponent has been removed.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FHighlightableComponentRemoved OnHighlightableComponentRemoved;

	/**
	 * Event called once Collision Component is added to Collision Shapes.
	 * Returns which PrimitiveComponent has been added.
	 */	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionComponentAdded OnCollisionComponentAdded;

	/**
	 * Event called once Collision Component is removed from Collision Shapes.
	 * Returns which PrimitiveComponent has been removed.
	 */		
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionComponentRemoved OnCollisionComponentRemoved;

#pragma endregion 

#pragma region InteractionHelperEvents

	/**
	 * Event called once Auto Setup has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableAutoSetupChanged OnInteractableAutoSetupChanged;

	/**
	 * Event called once Interaction Weight has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableWeightChanged OnInteractableWeightChanged;

	/**
	 * Event called once Interactable State has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableStateChanged OnInteractableStateChanged;

	/**
	 * Event called once Interactable Owner has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableOwnerChanged OnInteractableOwnerChanged;

	/**
	 * Event called once Interactable Collision Channel has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableCollisionChannelChanged OnInteractableCollisionChannelChanged;

	/**
	 * Event called once Lifecycle Mode has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FLifecycleModeChanged OnLifecycleModeChanged;

	/**
	 * Event called once Lifecycle Count has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FLifecycleCountChanged OnLifecycleCountChanged;

	/**
	 * Event called once Cooldown Period has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCooldownPeriodChanged OnCooldownPeriodChanged;
		
	/**
	 * Event called once Interactor has been removed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractorChanged OnInteractorChanged;

#pragma endregion 

#pragma region Handles

	virtual FOnInteractableSelected& GetOnInteractableSelectedHandle() override
	{ return OnInteractableSelected; };
	virtual FInteractorFound& GetOnInteractorFoundHandle() override
	{ return OnInteractorFound; };
	virtual FInteractorLost& GetOnInteractorLostHandle() override
	{ return OnInteractorLost; };
	virtual FInteractorTraced& GetOnInteractorTracedHandle() override
	{ return OnInteractorTraced; };
	virtual FInteractorOverlapped& GetOnInteractorOverlappedHandle() override
	{ return OnInteractorOverlapped; };
	virtual FInteractorStopOverlap& GetOnInteractorStopOverlapHandle() override
	{ return OnInteractorStopOverlap; };
	virtual FInteractionCompleted& GetOnInteractionCompletedHandle() override
	{ return OnInteractionCompleted; };
	virtual FInteractionStarted& GetOnInteractionStartedHandle() override
	{ return OnInteractionStarted; };
	virtual FInteractionStopped& GetOnInteractionStoppedHandle() override
	{ return OnInteractionStopped; };
	virtual FInteractionCanceled& GetOnInteractionCanceledHandle() override
	{ return OnInteractionCanceled; };

	virtual FTimerHandle& GetCooldownHandle() override
	{ return CooldownHandle; }

#pragma endregion 

#pragma endregion 

#pragma region Attributes

#pragma region Debug

protected:

	/**
	 * If active, debug can be drawn.
	 * Serves a general purpose as a flag.
	 * Does not affect Shipping builds by default C++ implementation.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Debug")
	uint8 bToggleDebug : 1;

	/**
	 * Cached Collision Shape Settings.
	 * Filled when Collision Shapes are registered.
	 * Once Collision Shape is unregistered, it reads its cached settings and returns to pre-interaction Collision Settings.
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Debug", meta=(DisplayThumbnail = false, ShowOnlyInnerProperties))
	mutable TMap<UPrimitiveComponent*, FCollisionShapeCache> CachedCollisionShapesSettings;

#pragma endregion

#pragma region Required

protected:

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
	* List of Interaction Keys for each platform.
	* There is no validation for Keys validation! Nothing stops you from setting Keyboard keys for Consoles. Please, be careful with this variable!
	*/
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	TMap<FString, FInteractionKeySetup> InteractionKeysPerPlatform;
	
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

protected:

	
	/**
	 * List of Interactable Classes which are ignored
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Optional", meta=(NoResetToDefault, AllowAbstract=false, MustImplement="ActorInteractorInterface", BlueprintBaseOnly))
	TArray<TSoftClassPtr<UObject>> IgnoredClasses;
	
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

protected:
	
	/**
	 * Current State of the Interactable.
	 * Is subject to State Machine.
	 * @see [State Machine] https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Validations#state-machine
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only")
	EInteractableStateV2 InteractableState;

	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only")
	int32 RemainingLifecycleCount;

	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only")
	TArray<TScriptInterface<IActorInteractableInterface>> InteractionDependencies;
	
	/**
	 * List of Highlightable Components.
	 * * Set Overlap Events to true
	 * * Response to Collision Channel to overlap
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only")
	TArray<UMeshComponent*> HighlightableComponents;
	
	/**
	 * List of Collision Components.
	 * Those component will be updated to:
	 * * Allow Render Custom Depth
	 * * Use specific StencilID
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only")
	TArray<UPrimitiveComponent*> CollisionComponents;

private:

	UPROPERTY()
	FTimerHandle CooldownHandle;

	/**
	 * Owning Actor of this Interactable.
	 * By default its set to Owner.
	 * Can be overriden.
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only", meta=(DisplayThumbnail = false))
	AActor* InteractionOwner;

	/**
	 * Interactor which is using this Interactable.
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only", meta=(DisplayThumbnail = false))
	TScriptInterface<IActorInteractorInterface> Interactor;

#pragma endregion

#pragma endregion 
};