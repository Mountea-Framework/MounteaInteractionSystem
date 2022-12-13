// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Engine/DataTable.h"

#include "Interfaces/ActorInteractableInterface.h"
#include "Helpers/InteractionHelpers.h"

#include "ActorInteractableComponentBase.generated.h"

#define LOCTEXT_NAMESPACE "InteractableComponent"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetUpdated);


/**
 * Actor Interactable Base Component
 *
 * Implements ActorInteractableInterface.
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Base
 */
UCLASS(Abstract, ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentBase : public UWidgetComponent, public IActorInteractableInterface
{
	GENERATED_BODY()

public:

	UActorInteractableComponentBase();

protected:

	virtual void BeginPlay() override;
	virtual void InitWidget() override;

	virtual void OnRegister() override;

#pragma region InteractableFunctions
	
public:

	/**
	 * Returns whether this Interactable is being autosetup or not. 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool DoesAutoSetup() const override;

	/**
	 * Sets whether this Interactable will be Autosetup.
	 * Setup Type is variable exposed in Interaction|Required panel.
	 * @param NewValue This value will be used as SetupType value.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void ToggleAutoSetup(const ESetupType& NewValue) override;


	/**
	 * Tries to set state of this Interactable to Active. 
	 * If fails, returns False and updates ErrorMessage
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool ActivateInteractable(FString& ErrorMessage) override;
	/**
	 * Tries to set state of this Interactable to Awake. 
	 * If fails, returns False and updates ErrorMessage
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool WakeUpInteractable(FString& ErrorMessage) override;
	/**
	 * Tries to set state of this Interactable to Asleep. 
	 * If fails, returns False and updates ErrorMessage
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool SnoozeInteractable(FString& ErrorMessage) override;
	/**
	 * Tries to set state of this Interactable to Completed. 
	 * If fails, returns False and updates ErrorMessage
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool CompleteInteractable(FString& ErrorMessage) override;
	/**
	 * Tries to set state of this Interactable to Disabled. 
	 * If fails, returns False and updates ErrorMessage
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void DeactivateInteractable() override;

	
	/**
	 * Optimized request for Interactables.
	 * Can be overriden in C++ for specific class needs.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool CanInteract() const override;
	/**
	 * Returns whether Interaction can be processed.
	 * Return True if is Awaken and does not have any Interactor yet.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool CanBeTriggered() const override;
	/**
	 * Returns whether Interaction is in process.
	 * Return True if is Active.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool IsInteracting() const override;

	/**
	 * Returns Default Interactable State.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,Category="Interaction")
	virtual EInteractableStateV2 GetDefaultState() const override;
	/**
	 * Tries to set Default Interactable State.
	 *
	 * @param NewState	Value which will be set as new Default Interactable State.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetDefaultState(const EInteractableStateV2 NewState) override;
	/**
	 * Returns State of Interactable.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual EInteractableStateV2 GetState() const override;
	/**
	 * Sets State of Interactable.
	 * @param NewState Value of the State to be set
	 * 
	 * SetState is driven by StateMachine. 
	 * StateMachine is available on Wiki:
	 * * https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Validations#state-machine
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetState(const EInteractableStateV2 NewState) override;


	/**
	 * Starts Highlight for all Highlightable Components.
	 * Requires Rendering Custom depth in Project Settings.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void StartHighlight() override;
	/**
	 * Stops Highlight for all Highlightable Components.
	 * Requires Rendering Custom depth in Project Settings.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void StopHighlight() override;
	
	
	/**
	 * Returns list of ignored classes.
	 * Those are classes which will be ignored for interaction events and won't trigger them.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<TSoftClassPtr<UObject>> GetIgnoredClasses() const override;
	/**
	 * Force set Ignored Classes. 
	 * @param NewIgnoredClasses New array of Ignored Classes. Can be given empty array.
	*/
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetIgnoredClasses(const TArray<TSoftClassPtr<UObject>> NewIgnoredClasses) override;
	/**
	* Will add a class to Ignored Class List.
	* @param AddIgnoredClass Class to be ignored.
	*
	* Only objects implementing ActorInteractorInterface will be affected!
	*/
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddIgnoredClass(TSoftClassPtr<UObject> AddIgnoredClass) override;
	/**
	* Will add classes to Ignored Class List.
	* @param AddIgnoredClasses Array of classes to be ignored.
	* 
	* Only objects implementing ActorInteractorInterface will be affected!
	*/
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddIgnoredClasses(TArray<TSoftClassPtr<UObject>> AddIgnoredClasses) override;
	/**
	 * Will remove a class from Ignored Class List.
	 * @param RemoveIgnoredClass Class to be accepted.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveIgnoredClass(TSoftClassPtr<UObject> RemoveIgnoredClass) override;
	/**
	 * Will remove classes from Ignored Class List.
	 * @param RemoveIgnoredClasses Array of classes to be accepted.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveIgnoredClasses(TArray<TSoftClassPtr<UObject>> RemoveIgnoredClasses) override;

	/**
	 * Will add Interaction Dependency to List of Dependencies. 
	 * All dependencies are affected by Interaction State of this Interactable. 
	 * Interaction Dependency is Suppressed while its Master is Active.
	 * Duplicates are not allowed and will be filtered out.
	 * 
	 * @param InteractionDependency Dependecy which will be added. Null or duplicates are not allowed.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddInteractionDependency(const TScriptInterface<IActorInteractableInterface> InteractionDependency) override;
	/**
	 * Will remove Interaction Dependency from List of Dependencies.
	 * All dependencies are affected by Interaction State of this Interactable. 
	 * Interaction Dependency is Suppressed while its Master is Active.
	 * If Dependency is not present, nothing happens.
	 * 
	 * @param InteractionDependency Dependency which will be removed.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveInteractionDependency(const TScriptInterface<IActorInteractableInterface> InteractionDependency) override;
	/**
	 * Return List of Dependencies.
	 * All dependencies are affected by Interaction State of this Interactable. 
	 * Interaction Dependency is Suppressed while its Master is Active.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual TArray<TScriptInterface<IActorInteractableInterface>> GetInteractionDependencies() const override;
	/**
	 * Function responsible for updating Interaction Dependencies.
	 * Does process all hooked up Interactables in predefined manner.
	 * Is called once State is updated.
	 */
	UFUNCTION(Category="Interaction")
	virtual void ProcessDependencies() override;


	/**
	 * Returns Interactor which is interacting with this Interactable.
	 * If no Interactor, will return nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TScriptInterface<IActorInteractorInterface> GetInteractor() const override;
	/**
	 * Sets Interactor as Active Interactor.
	 * OnInteractorChanged is called upon successful change.
	 * 
	 * @param NewInteractor Value to be set as a new Interactor. Can be null.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractor(const TScriptInterface<IActorInteractorInterface> NewInteractor) override;

	/**
	 * Returns Interaction Progress.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual float GetInteractionProgress() const override;
	/**
	 * Returns Interaction Period.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual float GetInteractionPeriod() const override;
	/**
	 * Sets Interaction Period.
	 * Values are clamped and verified:
	 * 
	 * @param NewPeriod Value to be set as new Interaction Period. Is validated:
	 * - -1 = immediate
	 * - values less than 0 and larger than -1 are 0.1
	 * - 0  = 0.1s
	 */	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractionPeriod(const float NewPeriod) override;

	/**
	 * Returns Interactable Weight. 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual int32 GetInteractableWeight() const override;
	/**
	 * Sets new Interactable Weight value.
	 * 
	 * @param NewWeight Value to be set as new Interactable Weight. Is validated:
	 * Min value is 0.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractableWeight(const int32 NewWeight) override;

	/**
	 * Return Interactable Owner.
	 * This will be most likely same as the GetOwner, however, there is a way to override this default value.
	 * Useful for very complex interactions.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual AActor* GetInteractableOwner() const override;
	/**
	 * Sets new InteractableOwner.
	 * 
	 * @param NewOwner Value to be set as Interactable Owner. Is validated:
	 * Nullptr is not allowed and will not be applied.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractableOwner(AActor* NewOwner) override;

	/**
	 * Returns Collision Channel.
	 * Both Object and Trace Channels are allowed.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual ECollisionChannel GetCollisionChannel() const override;
	/**
	 * Sets new Collision Channel.
	 * Both Object and Trace Channels are allowed.
	 * @param NewChannel New Collision Channel to be used for this Interactable.
	 * 
	 * Interaction specific channel are our strong recommendation.
	 * For usage and setup, take a look at 'Examples' project from Mountea Framework GitHub page.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetCollisionChannel(const ECollisionChannel& NewChannel) override;


	/**
	 * Returns Lifecycle Mode. 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual EInteractableLifecycle GetLifecycleMode() const override;
	/**
	 * Set new Lifecycle Mode.
	 * @param NewMode New Lifecycle Mode to be used for this Interactable. 
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetLifecycleMode(const EInteractableLifecycle& NewMode) override;


	/**
	 * Returns Lifecycle Count of this Interactable.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual int32 GetLifecycleCount() const override;
	/**
	 * Set new Lifecycle Count.
	 * @param NewLifecycleCount New Lifecycle Count to be used for this Interactable.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetLifecycleCount(const int32 NewLifecycleCount) override;
	/**
	 * Returns how many Lifecycles remain.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual int32 GetRemainingLifecycleCount() const override;


	/**
	 * Returns Cooldown Period in seconds.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual float GetCooldownPeriod() const override;
	/**
	 * Sets new Cooldown Period.
	 * @param NewCooldownPeriod Value in seconds to be used as Coolddown Period.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetCooldownPeriod(const float NewCooldownPeriod) override;


	/**
	 * Returns first Interaction Key for specified Platform.
	 * @param RequestedPlatform Name of platform you want to know the Interaction Key
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual FKey GetInteractionKeyForPlatform(const FString& RequestedPlatform) const override;
	/**
	 * Returns all Interaction Keys for specified Platform.
	 * @param RequestedPlatform Name of platform you want to know the Interaction Key
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<FKey> GetInteractionKeysForPlatform(const FString& RequestedPlatform) const override;
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
	 * Returns true if defined for specified platform, otherwise returns false.
	 * @param RequestedKey Key which you are looking fod.
	 * @param Platform Name of the platfom which should use this Key.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool FindKey(const FKey& RequestedKey, const FString& Platform) const override;
	
	/**
	 * Returns all Collision Components.
	 * Collision Components might be both Shape Components (Box Collision etc.) or Mesh Components (Static Mesh etc.).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<UPrimitiveComponent*> GetCollisionComponents() const override;
	/**
	 * Tries to add new Collision Component. No duplicates allowed. Null is not accepted.
	 * Calls OnCollisionComponentAddedEvent.
	 * @param CollisionComp Component to be added into list of Collision Components.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddCollisionComponent(UPrimitiveComponent* CollisionComp) override;
	/**
	 * Tries to add Collision Components. Calls AddCollisionComponent for each component.
	 * OnCollisionComponentAddedEvent is called for each component added.  No duplicates allowed. Nulls are not accepted.
	 * @param NewCollisionComponents List of components to be added into list of Collision Components.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddCollisionComponents(const TArray<UPrimitiveComponent*> NewCollisionComponents) override;
	/**
	 * Tries to remove Collision Component if registered. Null is not accepted.
	 * Calls OnCollisionComponentRemovedEvent.
	 * @param CollisionComp Component to be removed from list of Collision Components.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveCollisionComponent(UPrimitiveComponent* CollisionComp) override;
	/** 
	 * Tries to remove Collision Components. Calls RemoveCollisionComponent for each component.
	 * OnCollisionComponentRemovedEvent is called for each component removed. Nulls are not accepted.
	 * @param RemoveCollisionComponents List of components to be removed from list of Collision Components.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveCollisionComponents(const TArray<UPrimitiveComponent*> RemoveCollisionComponents) override;

	
	/**
	 * Returns array of Highlightable Components.
	 * Collision Components are Mesh Components.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<UMeshComponent*> GetHighlightableComponents() const override;
	/**
	 * Tries to add new Highlightable Component.
	 * Calls OnHighlightableComponentAdded.
	 * Duplicates or null not allowed.
	 * @param MeshComponent Mesh Component to be added to List of Highlightable Components
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddHighlightableComponent(UMeshComponent* MeshComponent) override;
	/**
	 * Tries to add new Highlightable Componentes. Calls AddHighlightableComponent for each Component.
	 * Calls OnHighlightableComponentAdded.
	 * @param AddMeshComponents List of Mesh Components to be added to List of Highlightable Components
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddHighlightableComponents(const TArray<UMeshComponent*> AddMeshComponents) override;
	/**
	 * Tries to remove Highlightable Component.
	 * Calls OnHighlightableComponentRemoved.
	 * Null not allowed.
	 * @param MeshComponent Mesh Component to be removed from List of Highlightable Components
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveHighlightableComponent(UMeshComponent* MeshComponent) override;
	/**
	 * Tries to remove Highlightable Componentes. Calls RemoveHighlightableComponent for each Component.
	 * Calls OnHighlightableComponentRemoved.
	 * @param AddMeshComponents List of Mesh Components to be removed from List of Highlightable Components
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveHighlightableComponents(const TArray<UMeshComponent*> RemoveMeshComponents) override;

	/**
	 * Tries to find MeshComponent by Name.
	 * Returns null if finds nothing.
	 * @param Name Name of searched component
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual UMeshComponent* FindMeshByName(const FName Name) const override;
	/**
	 * Tries to find MeshComponent by Tag.
	 * Returns null if finds nothing.
	 * @param Tag Tag of searched component
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual UMeshComponent* FindMeshByTag(const FName Tag) const override;
	/**
	 * Tries to find PrimitiveComponent by Name.
	 * Returns null if finds nothing.
	 * @param Name Name of searched component
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual UPrimitiveComponent* FindPrimitiveByName(const FName Name) const override;
	/**
	 * Tries to find PrimitiveComponent by Tag.
	 * Returns null if finds nothing.
	 * @param Tag Tag of searched component
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual UPrimitiveComponent* FindPrimitiveByTag(const FName Tag) const override;

	
	/**
	 * Returns all Collision Overrides.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<FName> GetCollisionOverrides() const override;
	/**
	 * Returns all Highlightable Overrides.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<FName> GetHighlightableOverrides() const override;

	/**
	 * Returns Interactable Data Asset.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual FDataTableRowHandle GetInteractableData() override;
	/**
	 * Sets new Interactable Data.
	 * @param NewData New Data to be used as Interactable Data.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractableData(FDataTableRowHandle NewData) override;

	/**
	 * Returns Interactable Name.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual FText GetInteractableName() const override;
	/**
	 * Sets new Interactable Name.
	 * @param NewName Name to set as Interactable Name.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractableName(const FText& NewName) override;

	/**
	 * Returns value of Comparison Method.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual ETimingComparison GetComparisonMethod() const override;
	/**
	 * Sets new value of Comparison Method.
	 *
	 * @param Value	Value of new Comparison Method.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetComparisonMethod(const ETimingComparison Value) override;

#pragma endregion

#pragma region EventFunctions

#pragma region NativeFunctions

protected:

	/**
	 * Event bound to Interactor's OnInteractableSelected.
	 * If Interactor selects any Interactable, this Event is called and selected Interactable is Activated, while others are set back to Awaken state.
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractableSelected(const TScriptInterface<IActorInteractableInterface>& Interactable) override;

	/**
	 * 
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractableLost(const TScriptInterface<IActorInteractableInterface>& Interactable) override;
	
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
	 * Event called once Interaction Cycle is completed.
	 * Calls OnInteractionCycleCompletedEvent
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractionCycleCompleted(const float& CompletedTime, const int32 CyclesRemaining) override;

	/**
	 * Event called once Interaction Starts.
	 * Called by OnInteractionStarted
	 * Calls OnInteractionStartedEvent
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractionStarted(const float& TimeStarted, const FKey& PressedKey) override;

	/**
	 * Event called once Interaction Stops.
	 * Called by OnInteractionStopped
	 * Calls OnInteractionStoppedEvent
	 */
	UFUNCTION(Category="Interaction")
	virtual void InteractionStopped(const float& TimeStarted, const FKey& PressedKey) override;

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
	virtual void OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

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
	virtual void OnInteractableStopOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
		
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
	virtual void OnInteractableTraced(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/**
	 * Event called once Widget Settings are updated.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnWidgetUpdatedEvent();

#pragma endregion

#pragma region IgnoredClassesEvents

protected:

	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnIgnoredClassAdded(const TSoftClassPtr<UObject>& IgnoredClass);

	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnIgnoredClassRemoved(const TSoftClassPtr<UObject>& IgnoredClass);

#pragma endregion


#pragma region AttributesEvents

protected:

	/**
	 * Event bound to OnInteractableDependencyChanged.
	 * Once OnInteractableDependencyChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnInteractableDependencyChangedEvent(const TScriptInterface<IActorInteractableInterface>& Dependency);

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

protected:

	virtual void FindAndAddCollisionShapes() override;
	virtual void FindAndAddHighlightableMeshes() override;

	virtual bool TriggerCooldown() override;

	UFUNCTION()
	virtual void ToggleWidgetVisibility(const bool IsVisible) override;

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
	virtual void ToggleDebug() override;

	/**
	 * Helper function.
	 * Looks for Collision and Highlightable Overrides and binds them.
	 * Looks for Parent Component which will be set as Collision Mesh and Highlighted Mesh.
	 */
	UFUNCTION()
	void AutoSetup();

	UFUNCTION()
	void OnCooldownCompletedCallback();
	
	bool ValidateInteractable() const;

	virtual void UpdateInteractionWidget();

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
	 * Event called once single Interaction Cycle is completed.
	 * Might be called multiple times, before 'OnInteractionCompleted' is called.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionCycleCompleted OnInteractionCycleCompleted;

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

	/**
	 * Event called once any Interaction Dependency is changed, no matter if removed or added.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableDependencyChanged OnInteractableDependencyChanged;

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
	virtual FInteractionCycleCompleted& GetOnInteractionCycleCompletedHandle() override
	{ return OnInteractionCycleCompleted; };
	virtual FInteractionStarted& GetOnInteractionStartedHandle() override
	{ return OnInteractionStarted; };
	virtual FInteractionStopped& GetOnInteractionStoppedHandle() override
	{ return OnInteractionStopped; };
	virtual FInteractionCanceled& GetOnInteractionCanceledHandle() override
	{ return OnInteractionCanceled; };
	virtual FInteractableDependencyChanged& GetInteractableDependencyChangedHandle() override
	{ return OnInteractableDependencyChanged; };

	virtual FTimerHandle& GetCooldownHandle() override
	{ return Timer_Cooldown; };
	
	virtual FOnWidgetUpdated& WidgetUpdatedHandle()
	{ return OnWidgetUpdated; };

#pragma endregion 

#pragma region Widget

	/**
	 * Event called any time any value of 'UserInterfaceSettings' has changed.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FOnWidgetUpdated OnWidgetUpdated;

#pragma endregion 

#pragma endregion 

#pragma region Attributes

#pragma region Debug

protected:
	
	/**
	 * If active, debug can be drawn.
	 * You can disable Editor Warnings. Editor Errors cannot be disabled!
	 * Serves a general purpose as a flag.
	 * Does not affect Shipping builds by default C++ implementation.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Debug", meta=(ShowOnlyInnerProperties))
	FDebugSettings DebugSettings;

#if WITH_EDITORONLY_DATA
	UBillboardComponent* InteractableSpriteComponent = nullptr;
#endif
#pragma endregion

#pragma region Required

protected:

	/**
	 * Defines how long does Interaction take.
	 * - -1 = immediate
	 * - 0  = 0.1s
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Required", meta=(UIMin=-1, ClampMin=-1, Units="seconds"))
	float InteractionPeriod;

	/**
	 * Default state of the Interactable to be set in BeginPlay.
	 * Cannot be set to:
	 * * Active
	 * * Finished
	 * * Cooldown
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	EInteractableStateV2 DefaultInteractableState;
	
	/**
	 * If set to true, Interactable will automatically assigns owning Component in Hierarchy as Highlightable Meshes and Collision Shapes.
	 * This setup might be useful for simple Actors, might cause issues with more complex ones.
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Required")
	ESetupType SetupType;

	/**
	 * Collision Channel which will be forced to all Collision Shapes as Overlap.
	 * All Collision Shapes will be updated to generate Overlap events.
	 *
	 * Could be either Trace or Object response.
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	TEnumAsByte<ECollisionChannel> CollisionChannel;
		
	/**
	 * Weight of this Interactable.
	 * Useful with multiple overlapping Interactables withing the same Actor. Interactor will always prefer the one with highest Weight value.
	 *
	 * Default value: 1
	 * Clamped in setter function to be at least 0 or higher.
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Required", meta=(UIMin=0, ClampMin=0))
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
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
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
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault, EditCondition = "LifecycleMode == EInteractableLifecycle::EIL_Cycled", UIMin=-1, ClampMin=-1, Units="times"))
	int32 LifecycleCount;

	/**
	 * How long it takes for Cooldown to finish.
	 * After this period of time the Interactable will be Awake again, unless no Interactor.
	 * Clamped in Setter.
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault, EditCondition = "LifecycleMode == EInteractableLifecycle::EIL_Cycled", UIMin=0.1, ClampMin=0.1, Units="Seconds"))
	float CooldownPeriod;

#pragma endregion

#pragma region Optional

protected:
	
	/**
	 * List of Interactable Classes which are ignored
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Optional", meta=(NoResetToDefault, AllowAbstract=false, MustImplement="ActorInteractorInterface", BlueprintBaseOnly))
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
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly,  Category="Interaction|Optional")
	uint8 bInteractionHighlight : 1;
	
	/**
	 * Defines what Stencil ID should be used to highlight the Primitive Mesh Components.
	 * In order to smoothly integrate with other logic, keep this ID unique!
	 * Default: 133
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly,  Category="Interaction|Optional", meta=(EditCondition="bInteractionHighlight == true", UIMin=0, ClampMin=0, UIMax=255, ClampMax=255))
	int32 StencilID;

	/**
	* List of Interaction Keys for each platform.
	* There is no validation for Keys validation! Nothing stops you from setting Keyboard keys for Consoles. Please, be careful with this variable!
	*/
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Optional", meta=(NoResetToDefault))
	TMap<FString, FInteractionKeySetup> InteractionKeysPerPlatform;
	
	/**
	 * Interactable Data.
	 * Could be any Data Table.
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Optional", meta=(ShowOnlyInnerProperties, NoResetToDefault))
	FDataTableRowHandle InteractableData;

	/**
	 * Display Name.
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Optional", meta=(NoResetToDefault))
	FText InteractableName = LOCTEXT("InteractableComponentBase", "Default");

	/**
	 * TODO
	 * Defines behaviour of TimeToStart.
	 * Less Than: Interaction will start if key is hold less than TimeToStart.
	 * More Than: Interaction will start if key is hold more than TimeToStart.
	 * Currently has no logic tied to it!
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Optional", meta=(NoResetToDefault))
	ETimingComparison ComparisonMethod;

	/**
	 * TODO
	 * Time in seconds it is required to start this interaction.
	 * Useful with multiple interactables.
	 * Depends on 'ComparisonMethod'.
	 * Currently has no logic tied to it!
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="Interaction|Optional", meta=(UIMin=0.001, ClampMin=0.001, Units="seconds", EditCondition="ComparisonMethod!=ETimingComparison::ECM_None", NoResetToDefault))
	float TimeToStart;

#pragma endregion 

#pragma region ReadOnly

protected:
	
	/**
	 * Current State of the Interactable.
	 * Is subject to State Machine.
	 * @see [State Machine] https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Validations#state-machine
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only")
	EInteractableStateV2 InteractableState;

	/**
	 * How many Lifecycles remain until this Interactable is Finished.
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only")
	int32 RemainingLifecycleCount;

	/**
	 * List of Interactables which are dependant on this Interactable.
	 * To manage dependencies, use following functions:
	 * - AddInteractionDependency
	 * - RemoveInteractionDependency
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only")
	TArray<TScriptInterface<IActorInteractableInterface>> InteractionDependencies;

	/**
	 * Cached Collision Shape Settings.
	 * Filled when Collision Shapes are registered.
	 * Once Collision Shape is unregistered, it reads its cached settings and returns to pre-interaction Collision Settings.
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only", meta=(DisplayThumbnail = false, ShowOnlyInnerProperties))
	mutable TMap<UPrimitiveComponent*, FCollisionShapeCache> CachedCollisionShapesSettings;
	
	/**
	 * List of Highlightable Components.
	 * * Set Overlap Events to true
	 * * Response to Collision Channel to overlap
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only")
	TArray<UMeshComponent*> HighlightableComponents;
	
	/**
	 * List of Collision Components.
	 * Those component will be updated to:
	 * * Allow Render Custom Depth
	 * * Use specific StencilID
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only")
	TArray<UPrimitiveComponent*> CollisionComponents;
	
	UPROPERTY()
	FTimerHandle Timer_Interaction;

	UPROPERTY()
	FTimerHandle Timer_Cooldown;

private:
	
	/**
	 * Owning Actor of this Interactable.
	 * By default its set to Owner.
	 * Can be overriden.
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only", meta=(DisplayThumbnail = false))
	AActor* InteractionOwner = nullptr;

	/**
	 * Interactor which is using this Interactable.
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Interaction|Read Only", meta=(DisplayThumbnail = false))
	TScriptInterface<IActorInteractorInterface> Interactor = nullptr;

#pragma endregion

#pragma endregion

#pragma region Editor

#if (!UE_BUILD_SHIPPING || WITH_EDITOR)
#if WITH_EDITOR
protected:
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;

#endif
	
	virtual void DrawDebug();

#endif
	
#pragma endregion 
};

#undef LOCTEXT_NAMESPACE