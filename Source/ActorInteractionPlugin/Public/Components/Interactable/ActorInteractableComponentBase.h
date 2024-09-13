// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Engine/DataTable.h"

#include "Interfaces/ActorInteractableInterface.h"
#include "Helpers/InteractionHelpers.h"
#include "Helpers/MounteaInteractionHelperEvents.h"

#include "ActorInteractableComponentBase.generated.h"

#define LOCTEXT_NAMESPACE "InteractableComponent"

class UInputMappingContext;
enum class ECommonInputType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetUpdated);


/**
 * Actor Interactable Base Component
 *
 * Abstract base class which contains underlying logic for child components.
 *
 * Implements ActorInteractableInterface.
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Base
 */
UCLASS(Abstract, ClassGroup=(Mountea), Blueprintable, BlueprintType, hideCategories=(Collision, AssetUserData, Cooking, Physics), ShowCategories=(Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component"), meta=(Keywords = "Base, Default, Interactable"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentBase : public UWidgetComponent, public IActorInteractableInterface
{
	GENERATED_BODY()

public:

	UActorInteractableComponentBase();

protected:
	
	virtual void BeginPlay() override;
	virtual void InitWidget() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnComponentCreated() override;
	virtual void OnRegister() override;

#pragma region InteractableFunctions
	
public:

	virtual bool DoesHaveInteractor_Implementation() const override;
	virtual bool DoesAutoSetup_Implementation() const override;
	virtual void ToggleAutoSetup_Implementation(const ESetupType& NewValue) override;
	virtual bool ActivateInteractable_Implementation(FString& ErrorMessage) override;
	virtual bool WakeUpInteractable_Implementation(FString& ErrorMessage) override;
	virtual bool CompleteInteractable_Implementation(FString& ErrorMessage) override;
	virtual void DeactivateInteractable_Implementation() override;
	virtual void PauseInteraction_Implementation(const float ExpirationTime, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;
	virtual void InteractableSelected_Implementation(const TScriptInterface<IActorInteractableInterface>& Interactable) override;
	virtual void InteractableLost_Implementation(const TScriptInterface<IActorInteractableInterface>& Interactable) override;
	virtual void InteractorFound_Implementation(const TScriptInterface<IActorInteractorInterface>& FoundInteractor) override;
	virtual void InteractorLost_Implementation(const TScriptInterface<IActorInteractorInterface>& LostInteractor) override;
	virtual void InteractionCompleted_Implementation(const float& TimeCompleted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;
	virtual void InteractionCycleCompleted_Implementation(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;
	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;
	virtual void InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) override;
	virtual void InteractionCanceled_Implementation() override;
	virtual void InteractionLifecycleCompleted_Implementation() override;
	virtual void InteractionCooldownCompleted_Implementation() override;
	virtual bool CanInteract_Implementation() const override;
	virtual bool CanBeTriggered_Implementation() const override;
	virtual bool IsInteracting_Implementation() const override;
	virtual EInteractableStateV2 GetDefaultState_Implementation() const override;
	virtual void SetDefaultState_Implementation(const EInteractableStateV2 NewState) override;
	virtual EInteractableStateV2 GetState_Implementation() const override;
	virtual void SetState_Implementation(const EInteractableStateV2 NewState) override;
	virtual void StartHighlight_Implementation() override;
	virtual void StopHighlight_Implementation() override;
	virtual TScriptInterface<IActorInteractorInterface> GetInteractor_Implementation() const override;
	virtual void SetInteractor_Implementation(const TScriptInterface<IActorInteractorInterface>& NewInteractor) override;
	virtual float GetInteractionProgress_Implementation() const override;
	virtual float GetInteractionPeriod_Implementation() const override;
	virtual void SetInteractionPeriod_Implementation(const float NewPeriod) override;
	virtual int32 GetInteractableWeight_Implementation() const override;
	virtual void SetInteractableWeight_Implementation(const int32 NewWeight) override;
	virtual AActor* GetInteractableOwner_Implementation() const override;
	
	virtual ECollisionChannel GetCollisionChannel_Implementation() const override;
	virtual void SetCollisionChannel_Implementation(const TEnumAsByte<ECollisionChannel>& NewChannel) override;
	virtual EInteractableLifecycle GetLifecycleMode_Implementation() const override;
	virtual void SetLifecycleMode_Implementation(const EInteractableLifecycle& NewMode) override;
	virtual int32 GetLifecycleCount_Implementation() const override;
	virtual void SetLifecycleCount_Implementation(const int32 NewLifecycleCount) override;
	virtual int32 GetRemainingLifecycleCount_Implementation() const override;
	virtual float GetCooldownPeriod_Implementation() const override;
	virtual void SetCooldownPeriod_Implementation(const float NewCooldownPeriod) override;
	
	virtual ETimingComparison GetComparisonMethod_Implementation() const override;
	virtual void SetComparisonMethod_Implementation(const ETimingComparison Value) override;
	
	virtual void AddInteractionDependency_Implementation(const TScriptInterface<IActorInteractableInterface>& InteractionDependency) override;
	virtual void RemoveInteractionDependency_Implementation(const TScriptInterface<IActorInteractableInterface>& InteractionDependency) override;
	virtual TArray<TScriptInterface<IActorInteractableInterface>> GetInteractionDependencies_Implementation() const override;
	virtual void ProcessDependencies_Implementation() override;
	
	virtual bool TriggerCooldown_Implementation() override;
	virtual void ToggleWidgetVisibility_Implementation(const bool IsVisible) override;
	
	virtual TArray<TSoftClassPtr<UObject>> GetIgnoredClasses_Implementation() const override;
	virtual void SetIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& NewIgnoredClasses) override;
	virtual void AddIgnoredClass_Implementation(const TSoftClassPtr<UObject>& AddIgnoredClass) override;
	virtual void AddIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& AddIgnoredClasses) override;
	virtual void RemoveIgnoredClass_Implementation(const TSoftClassPtr<UObject>& AddIgnoredClass) override;
	virtual void RemoveIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& AddIgnoredClasses) override;
	
	virtual TArray<UPrimitiveComponent*> GetCollisionComponents_Implementation() const override;
	virtual void AddCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp) override;
	virtual void AddCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents) override;
	virtual void RemoveCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp) override;
	virtual void RemoveCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents) override;
	
	virtual TArray<UMeshComponent*> GetHighlightableComponents_Implementation() const override;
	virtual void AddHighlightableComponent_Implementation(UMeshComponent* HighlightableComp) override;
	virtual void AddHighlightableComponents_Implementation(const TArray<UMeshComponent*>& HighlightableComponents) override;
	virtual void RemoveHighlightableComponent_Implementation(UMeshComponent* HighlightableComp) override;
	virtual void RemoveHighlightableComponents_Implementation(const TArray<UMeshComponent*>& HighlightableComponents) override;

	virtual TArray<FName> GetCollisionOverrides_Implementation() const override;
	virtual TArray<FName> GetHighlightableOverrides_Implementation() const override;
	virtual FText GetInteractableName_Implementation() const override;
	virtual void SetInteractableName_Implementation(const FText& NewName) override;
		
	virtual void FindAndAddCollisionShapes_Implementation() override;
	virtual void FindAndAddHighlightableMeshes_Implementation() override;
	
	virtual void BindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const override;
	virtual void UnbindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const override;
	virtual void BindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const override;
	virtual void UnbindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const override;

	virtual FDataTableRowHandle GetInteractableData_Implementation() const override;
	virtual void SetInteractableData_Implementation(FDataTableRowHandle NewData) override;
	
	virtual EHighlightType GetHighlightType_Implementation() const override;
	virtual void SetHighlightType_Implementation(const EHighlightType NewHighlightType) override;
	virtual UMaterialInterface* GetHighlightMaterial_Implementation() const override;
	virtual void SetHighlightMaterial_Implementation(UMaterialInterface* NewHighlightMaterial) override;
	
	virtual void InteractableDependencyStartedCallback_Implementation(const TScriptInterface<IActorInteractableInterface>& NewMaster) override;
	virtual void InteractableDependencyStoppedCallback_Implementation(const TScriptInterface<IActorInteractableInterface>& FormerMaster) override;

	virtual void ToggleDebug_Implementation() override;
	virtual FDebugSettings GetDebugSettings_Implementation() const override;
	virtual void SetDefaults_Implementation() override;

	virtual FGameplayTagContainer GetInteractableCompatibleTags_Implementation() const override;

	virtual void SetInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) override;
	virtual void AddInteractableCompatibleTag_Implementation(const FGameplayTag& Tag) override;
	virtual void AddInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) override;
	virtual void RemoveInteractableCompatibleTag_Implementation(const FGameplayTag& Tag) override;
	virtual void RemoveInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) override;
	
	virtual void ClearInteractableCompatibleTags_Implementation() override;

	virtual bool HasInteractor_Implementation() const override;

	virtual AActor* GetOwningActor_Implementation() const override;

	virtual FString ToString_Implementation() const override;

	virtual void OnInputDeviceChanged_Implementation(const ECommonInputType DeviceType, const FName& DeviceName) override;

#pragma endregion

#pragma region InteractableFunctions_Networking

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void SetState_Server(const EInteractableStateV2 NewState);

	UFUNCTION(Client, Reliable)
	void InteractionStarted_Client(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);
	UFUNCTION(Client, Reliable)
	void InteractionStopped_Client(const float& TimeStopped, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);
	UFUNCTION(Client, Reliable)
	void InteractionCancelled_Client(const float& TimeStopped, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);
	UFUNCTION(Client, Reliable)
	void InteractorFound_Client(const TScriptInterface<IActorInteractorInterface>& DirtyInteractor);
	UFUNCTION(Client, Reliable)
	void InteractorLost_Client(const TScriptInterface<IActorInteractorInterface>& DirtyInteractor);

	UFUNCTION(Client, Unreliable)
	void ProcessToggleActive_Client(const bool bIsEnabled);

	UFUNCTION(Client, Unreliable)
	void StartHighlight_Client();
	UFUNCTION(Client, Unreliable)
	void StopHighlight_Client();
	UFUNCTION(Client, Unreliable)
	void ShowWidget_Client();
	UFUNCTION(Client, Unreliable)
	void HideWidget_Client();

	UFUNCTION()
	void OnRep_InteractableState();
	
	UFUNCTION()
	void OnRep_ActiveInteractor();

#pragma endregion

#pragma region Functions

	virtual void ProcessToggleActive(const bool bIsEnabled);
	virtual void ProcessStartHighlight();
	virtual void ProcessStopHighlight();
	virtual void ProcessShowWidget();
	virtual void ProcessHideWidget();

	UFUNCTION()
	virtual void InteractorActionConsumed(UInputAction* ConsumedAction);
	UFUNCTION()
	void OnInputModeChanged(ECommonInputType CommonInput);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	TSubclassOf<UUserWidget> GetInteractableWidgetClass() const
	{ return WidgetClass; };

public:
	
#if WITH_EDITOR || WITH_EDITORONLY_DATA

	/**
	 * Overrides data with default values from Project Settings.
	 * Project Settings must be defined!
	 */
	UFUNCTION(BlueprintCallable, Category="MounteaInteraction")
	void SetDefaultValues();
	
#endif

#pragma endregion

#pragma region EventFunctions
	
#pragma region NativeFunctions
	
	/**
	 * Function called once any of Collision Shapes is overlapped.
	 * Called by OnInteractorOverlapped
	 * Calls OnInteractableBeginOverlapEvent
	 * Overlap event is called only if:
	 * * Other Actor Implements Interactor Interface
	 * * Other Actor contains Component which Implements Interactor Interface
	 * * Interactor Interface does have same CollisionChannel as Interactable
	 */
	UFUNCTION(Category="Mountea|Interaction|Interactable")
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
	UFUNCTION(Category="Mountea|Interaction|Interactable")
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
	UFUNCTION(Category="Mountea|Interaction|Interactable")
	virtual void OnInteractableTraced(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/**
	 * Event called once Widget Settings are updated.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnWidgetUpdatedEvent();

	/**
	* Event called once Highlight Type has changed.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnHighlightTypeChangedEvent(const EHighlightType& NewHighlightType);

	/**
	* Event called once Highlight Material has changed.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnHighlightMaterialChangedEvent(const UMaterialInterface* NewHighlightMaterial);
	
	UFUNCTION()
	void OnInteractionProgressExpired(const float ExpirationTime, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);

	UFUNCTION()
	void InteractableComponentActivated(UActorComponent* Component, bool bReset);
	
#pragma endregion

#pragma region IgnoredClassesEvents

protected:

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnIgnoredClassAdded(const TSoftClassPtr<UObject>& IgnoredClass);

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnIgnoredClassRemoved(const TSoftClassPtr<UObject>& IgnoredClass);

#pragma endregion


#pragma region AttributesEvents

protected:

	/**
	 * Event bound to OnInteractableDependencyChanged.
	 * Once OnInteractableDependencyChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractableDependencyChangedEvent(const TScriptInterface<IActorInteractableInterface>& Dependency);

	/**
	 * Event bound to OnInteractableAutoSetupChanged event.
	 * Once OnInteractableAutoSetupChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewValue New value of the Auto Setup
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractableAutoSetupChangedEvent(const bool NewValue);

	/**
	 * Event bound to OnInteractableWeightChanged event.
	 * Once OnInteractableWeightChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewWeight New value of the Interactable Weight
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractableWeightChangedEvent(const int32& NewWeight);

	/**
	 * Event bound to OnInteractableStateChanged event.
	 * Once OnInteractableStateChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewState New value of the Interactable State
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractableStateChangedEvent(const EInteractableStateV2& NewState);

	/**
	 * Event bound to OnInteractableOwnerChanged event.
	 * Once OnInteractableOwnerChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewOwner New value of the Interactable Owner
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractableOwnerChangedEvent(const AActor* NewOwner);

	/**
	 * Event bound to OnInteractableCollisionChannelChanged event.
	 * Once OnInteractableCollisionChannelChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewChannel New value of the Interactable Collision Channel
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractableCollisionChannelChangedEvent(const ECollisionChannel NewChannel);

	/**
	 * Event bound to OnLifecycleModeChanged event.
	 * Once OnLifecycleModeChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewMode New value of the Interactable Lifecycle
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnLifecycleModeChangedEvent(const EInteractableLifecycle& NewMode);

	/**
	 * Event bound to OnLifecycleCountChanged event.
	 * Once OnLifecycleCountChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewLifecycleCount New value of the Lifecycle Count
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnLifecycleCountChangedEvent(const int32 NewLifecycleCount);

	/**
	 * Event bound to OnCooldownPeriodChanged event.
	 * Once OnCooldownPeriodChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewCooldownPeriod New value of the Cooldown Period
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnCooldownPeriodChangedEvent(const float NewCooldownPeriod);

	/**
	 * Event bound to OnHighlightableComponentAdded event.
	 * Once OnHighlightableComponentAdded is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewHighlightableComp New Highlightable Component added to list of Highlightables 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnHighlightableComponentAddedEvent(const UMeshComponent* NewHighlightableComp);

	/**
	 * Event bound to OnHighlightableComponentRemoved event.
	 * Once OnHighlightableComponentRemoved is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param RemovedHighlightableComp Highlightable Component removed from the list of Highlightables 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnHighlightableComponentRemovedEvent(const UMeshComponent* RemovedHighlightableComp);

	/**
	 * Event bound to OnCollisionComponentAdded event.
	 * Once OnCollisionComponentAdded is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewCollisionComp New Collision Component added to list of Colliders 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnCollisionComponentAddedEvent(const UPrimitiveComponent* NewCollisionComp);

	/**
	 * Event bound to OnCollisionComponentRemoved event.
	 * Once OnCollisionComponentRemoved is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param RemovedCollisionComp Collision Component removed from list of Colliders 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnCollisionComponentRemovedEvent(const UPrimitiveComponent* RemovedCollisionComp);

	/**
	 * Event bound to OnInteractorChanged event.
	 * Once OnInteractorChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewInteractor Interactor set as new Interactor, could be nullptr
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractorChangedEvent(const TScriptInterface<IActorInteractorInterface>& NewInteractor);

#pragma endregion 

#pragma region InteractionHelpers

protected:
	
	virtual void CleanupComponent();


	/**
	 * Helper function.
	 * Looks for Collision and Highlightable Overrides and binds them.
	 * Looks for Parent Component which will be set as Collision Mesh and Highlighted Mesh.
	 */
	UFUNCTION()
	void AutoSetup();
	
	bool ValidateInteractable() const;

	virtual void UpdateInteractionWidget();
	
	UFUNCTION()	virtual void OnCooldownCompletedCallback();

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
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactable")
	FInteractorTraced OnInteractorTraced;

	/**
	 * Event called once any of Collision Shapes is overlapped.
	 * Overlap event is called only if:
	 * * Other Actor Implements Interactor Interface
	 * * Other Actor contains Component which Implements Interactor Interface
	 * * Interactor Interface does have same CollisionChannel as Interactable
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactable")
	FInteractorOverlapped OnInteractorOverlapped;
	
	/**
	 * Event called once any of Collision Shapes stops overlapping.
	 * Stop overlap event is called only if:
	 * * Other Actor Implements Interactor Interface
	 * * Other Actor contains Component which Implements Interactor Interface
	 * * Interactor Interface does have same CollisionChannel as Interactable
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactable")
	FInteractorStopOverlap OnInteractorStopOverlap;

#pragma endregion

#pragma region IgnoredClasses

	/**
	 * Event called once Ignored Interactor Class is successfully added.
	 * Called by OnIgnoredInteractorClassAdded.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	FIgnoredInteractorClassAdded OnIgnoredInteractorClassAdded;

	/**
	 * Event called once Ignored Interactor Class is successfully removed.
	 * Called by OnIgnoredInteractorClassRemoved.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	FIgnoredInteractorClassRemoved OnIgnoredInteractorClassRemoved;

#pragma endregion 

#pragma region InteractionEvents

	/**
	 * Event called once Interactor selects any Interactable. Provides info which Interactable has been selected.
	 * Selected Interactable might differ to this one. In such case, this event calls OnInteractorLost and cancels any interaction which might be in progress.
	 * Has native C++ implementation.
	 * Calls OnInteractableSelectedEvent.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FOnInteractableSelected OnInteractableSelected;

	/**
	 * Event called once Interactor is found. Provides info which Interactor is found.
	 * This event doesn't usually start the interaction, only notifies that this Interactable has found an Interactor.
	 * Called by OnInteractorFound
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractorFound OnInteractorFound;

	/**
	 * Event called once Interactor is lost. Provides info which Interactor is lost.
	 * This event is usually the first one in chain leading to Interaction Canceled.
	 * Called by OnInteractorLost.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractorLost OnInteractorLost;

	/**
	 * Event called once Interaction is completed. Provides information which Interactor caused completion.
	 * This event is the last event in chain.
	 * Called when Type is Once or after Lifecycles run out.
	 * Called from OnInteractionCompleted
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractionCompleted OnInteractionCompleted;
	
	/**
	 * Event called once single Interaction Cycle is completed. Provides information which Interactor caused completion.
	 * Might be called multiple times, before 'OnInteractionCompleted' is called.
	 * Never called if Type is Once.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractionCycleCompleted OnInteractionCycleCompleted;

	/**
	 * Event called once Interaction Starts. 
	 * Called by OnInteractionStarted
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractionStarted OnInteractionStarted;

	/**
	 * Event called once Interaction Stops.
	 * Called by OnInteractionStopped
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractionStopped OnInteractionStopped;

	/**
	 * Event called once Interaction is Canceled.
	 * Called by OnInteractionCanceled
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractionCanceled OnInteractionCanceled;

	/**
	 * Event called once Interaction Lifecycles is Completed.
	 * Called by OnLifecycleCompleted
	 */	
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FLifecycleCompleted OnLifecycleCompleted;

	/**
	 * Event called once Interaction Cooldown is Completed and Interactable can be Interacted with again.
	 * Called by OnCooldownCompleted
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FCooldownCompleted OnCooldownCompleted;

	/**
	 * Event called once any Interaction Dependency is changed, no matter if removed or added.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractableDependencyChanged OnInteractableDependencyChanged;

#pragma endregion

#pragma region InteractionComponents

	/**
	 * Event called once Highlightable Component is added to Highlightable Meshes.
	 * Returns which MeshComponent has been added.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FHighlightableComponentAdded OnHighlightableComponentAdded;

	/**
	 * Event called once Highlightable Component is removed from Highlightable Meshes.
	 * Returns which MeshComponent has been removed.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FHighlightableComponentRemoved OnHighlightableComponentRemoved;

	/**
	 * Event called once Collision Component is added to Collision Shapes.
	 * Returns which PrimitiveComponent has been added.
	 */	
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FCollisionComponentAdded OnCollisionComponentAdded;

	/**
	 * Event called once Collision Component is removed from Collision Shapes.
	 * Returns which PrimitiveComponent has been removed.
	 */		
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FCollisionComponentRemoved OnCollisionComponentRemoved;

#pragma endregion 

#pragma region InteractionHelperEvents

	/**
	 * Event called once Auto Setup has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractableAutoSetupChanged OnInteractableAutoSetupChanged;

	/**
	 * Event called once Interaction Weight has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractableWeightChanged OnInteractableWeightChanged;

	/**
	 * Event called once Interactable State has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractableStateChanged OnInteractableStateChanged;

	/**
	 * Event called once Interactable Owner has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractableOwnerChanged OnInteractableOwnerChanged;

	/**
	 * Event called once Interactable Collision Channel has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractableCollisionChannelChanged OnInteractableCollisionChannelChanged;

	/**
	 * Event called once Lifecycle Mode has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FLifecycleModeChanged OnLifecycleModeChanged;

	/**
	 * Event called once Lifecycle Count has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FLifecycleCountChanged OnLifecycleCountChanged;

	/**
	 * Event called once Cooldown Period has changed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FCooldownPeriodChanged OnCooldownPeriodChanged;
		
	/**
	 * Event called once Interactor has been removed.
	 * Expected to be more a debugging event rather than in-game event.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractorChanged OnInteractorChanged;

	/**
	 * Event called once HighlightType has changed.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FHighlightTypeChanged OnHighlightTypeChanged;

	/**
	 * Event called once HighlightMaterial has changed.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FHighlightMaterialChanged OnHighlightMaterialChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInputActionConsumed OnInputActionConsumed;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractionDeviceChanged OnInteractionDeviceChanged;

	FInteractableDependencyStarted InteractableDependencyStarted;

	FInteractableDependencyStopped InteractableDependencyStopped;

#pragma endregion 

#pragma region Handles

public:

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
	virtual FHighlightTypeChanged& GetHighlightTypeChanged() override
	{ return OnHighlightTypeChanged; };
	virtual FHighlightMaterialChanged& GetHighlightMaterialChanged() override
	{ return OnHighlightMaterialChanged; };
	virtual FInteractableDependencyStarted& GetInteractableDependencyStarted() override
	{ return InteractableDependencyStarted; };
	virtual FInteractableDependencyStopped& GetInteractableDependencyStopped() override
	{ return InteractableDependencyStopped; };

	virtual FTimerHandle& GetCooldownHandle() override
	{ return Timer_Cooldown; };

	virtual FInteractableStateChanged& GetInteractableStateChanged() override
	{ return OnInteractableStateChanged; };
	
	virtual FOnWidgetUpdated& WidgetUpdatedHandle()
	{ return OnWidgetUpdated; };

	
	virtual FInteractableWidgetVisibilityChanged& GetInteractableWidgetVisibilityChangedHandle() override
	{ return  OnInteractableWidgetVisibilityChanged; };

	virtual FInputActionConsumed& GetInputActionConsumedHandle() override
	{ return OnInputActionConsumed; };
	virtual FInteractionDeviceChanged& GetInteractionDeviceChangedHandle() override
	{ return OnInteractionDeviceChanged; };

#pragma endregion 

#pragma region Widget
	
	/**
	 * Event called any time any value of 'UserInterfaceSettings' has changed.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FOnWidgetUpdated OnWidgetUpdated;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractableWidgetVisibilityChanged OnInteractableWidgetVisibilityChanged;

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
	UPROPERTY(EditAnywhere, Category="MounteaInteraction|Debug", meta=(ShowOnlyInnerProperties))
	FDebugSettings																								DebugSettings;

#if WITH_EDITORONLY_DATA
	TObjectPtr<UBillboardComponent>																	InteractableSpriteComponent = nullptr;
#endif
#pragma endregion

#pragma region Required

protected:

	/**
	 * Defines how long does Interaction take.
	 * - -1 = immediate
	 * - 0  = 0.1s
	 */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="MounteaInteraction|Required", meta=(UIMin=-1, ClampMin=-1, Units="seconds", NoResetToDefault))
	float																													InteractionPeriod;

	/**
	 * Default state of the Interactable to be set in BeginPlay.
	 * Cannot be set to:
	 * * Active
	 * * Finished
	 * * Cooldown
	 */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="MounteaInteraction|Required", meta=(NoResetToDefault))
	EInteractableStateV2																						DefaultInteractableState;
	
	/**
	 * If set to true, Interactable will automatically assigns owning Component in Hierarchy as Highlightable Meshes and Collision Shapes.
	 * This setup might be useful for simple Actors, might cause issues with more complex ones.
	 */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="MounteaInteraction|Required", meta=(NoResetToDefault))
	ESetupType																										SetupType;

	/**
	 * Collision Channel which will be forced to all Collision Shapes as Overlap.
	 * All Collision Shapes will be updated to generate Overlap events.
	 *
	 * Could be either Trace or Object response.
	 */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="MounteaInteraction|Required", meta=(NoResetToDefault))
	TEnumAsByte<ECollisionChannel>																	CollisionChannel;
	
	/**
	 * How long it takes for Cooldown to finish.
	 * After this period of time the Interactable will be Awake again, unless no Interactor.
	 * Clamped in Setter.
	 */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="MounteaInteraction|Required", meta=(NoResetToDefault, EditCondition = "LifecycleMode == EInteractableLifecycle::EIL_Cycled", UIMin=0.1, ClampMin=0.1, Units="Seconds"))
	float																													CooldownPeriod;

	/**
	 * Defines Lifecycle Mode of this Interactable.
	 * Cycled:
	 * * Can be used multiple times
	 * * Good for NPCs
	 * Once:
	 * * Can be used only once
	 * * Good for pickup items
	 */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="MounteaInteraction|Required", meta=(NoResetToDefault))
	EInteractableLifecycle																						LifecycleMode;

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
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="MounteaInteraction|Required", meta=(NoResetToDefault, EditCondition = "LifecycleMode == EInteractableLifecycle::EIL_Cycled", UIMin=-1, ClampMin=-1, Units="times"))
	int32																												LifecycleCount;

	/**
	 * Weight of this Interactable.
	 * Useful with multiple overlapping Interactables withing the same Actor. Interactor will always prefer the one with highest Weight value.
	 *
	 * Default value: 1
	 * Clamped in setter function to be at least 0 or higher.
	 */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="MounteaInteraction|Required", meta=(UIMin=0, ClampMin=0, NoResetToDefault))
	int32																												InteractionWeight;
	
#pragma endregion

#pragma region Optional

protected:
	
	/**
	 * 
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Optional", meta=(NoResetToDefault))
	FGameplayTagContainer																					InteractableCompatibleTags;
	
	/**
	 * List of Interactable Classes which are ignored
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="MounteaInteraction|Optional", meta=(NoResetToDefault, AllowAbstract=false, MustImplement="/Script/ActorInteractionPlugin.ActorInteractorInterface", BlueprintBaseOnly))
	TArray<TSoftClassPtr<UObject>>																	IgnoredClasses;
	
	/**
	 * Expects: Actor Tags
	 * List of Actor Tags which define which Primitive Components should be added to Collision Shapes.
	 * This way, you can easily specify what Components will act as Collision Shapes, and without using any nodes.
	 *
	 * Is used even with Auto Setup.
	 */
	UPROPERTY(EditAnywhere, Category="MounteaInteraction|Optional", meta=(NoResetToDefault))
	TArray<FName>																								CollisionOverrides;

	/**
	 * Expects: Actor Tags
	 * List of Actor Tags which define which Mesh Components should be added to Highlightable Meshes.
	 * This way, you can easily specify what Components will act as Highlightable Meshes, and without using any nodes.
	 *
	 * Is used even with Auto Setup.
	 */
	UPROPERTY(EditAnywhere, Category="MounteaInteraction|Optional", meta=(NoResetToDefault))
	TArray<FName>																								HighlightableOverrides;

	/**
	* Defines what Highlight Type is used.
	*/
	UPROPERTY(Replicated, SaveGame, EditAnywhere, BlueprintReadOnly,  Category="MounteaInteraction|Optional")
	EHighlightType																									HighlightType;
	
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
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly,  Category="MounteaInteraction|Optional")
	uint8																												bInteractionHighlight : 1;
	
	/**
	 * Defines what Stencil ID should be used to highlight the Primitive Mesh Components.
	 * In order to smoothly integrate with other logic, keep this ID unique!
	 * Default: 133
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly,  Category="MounteaInteraction|Optional", meta=(EditCondition="bInteractionHighlight==true", UIMin=0, ClampMin=0, UIMax=255, ClampMax=255))
	int32																												StencilID;
	
	UPROPERTY(Replicated, SaveGame, EditAnywhere, BlueprintReadOnly,  Category="MounteaInteraction|Optional", meta=(EditCondition="bInteractionHighlight==true"))
	TObjectPtr<UMaterialInterface>																		HighlightMaterial = nullptr;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly,  Category="MounteaInteraction|Optional")
	uint8																												bCanPersist : 1;
	
	/**
	 * Provides a simple way to determine how fast Interaction Progress is kept before interaction is cancelled.
	 * * -1 means never while Interactor is valid
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly,  Category="MounteaInteraction|Optional", meta=(UIMin=-1.f, ClampMin=-1.f), meta=(EditCondition="bCanPersist!=false"))
	float																													InteractionProgressExpiration = 0.f;
	
	/**
	 * Interactable Data.
	 * Could be any Data Table.
	 */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="MounteaInteraction|Optional", meta=(ShowOnlyInnerProperties, NoResetToDefault)) //, RequiredAssetDataTags = "RowStructure=/Script/ActorInteractionSystem.InteractionData"))
	FDataTableRowHandle																					InteractableData;

	/**
	 * Display Name.
	 */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="MounteaInteraction|Optional", meta=(NoResetToDefault))
	FText																												InteractableName = NSLOCTEXT("InteractableComponentBase", "DefaultInteractable", "Default");

	/**
	 * TODO
	 * Defines behaviour of TimeToStart.
	 * Less Than: Interaction will start if key is hold less than TimeToStart.
	 * More Than: Interaction will start if key is hold more than TimeToStart.
	 * Currently has no logic tied to it!
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="MounteaInteraction|Optional", meta=(NoResetToDefault))
	ETimingComparison																							ComparisonMethod;

	/**
	 * TODO
	 * Time in seconds it is required to start this interaction.
	 * Useful with multiple interactables.
	 * Depends on 'ComparisonMethod'.
	 * Currently has no logic tied to it!
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="MounteaInteraction|Optional", meta=(UIMin=0.001, ClampMin=0.001, Units="seconds", EditCondition="ComparisonMethod!=ETimingComparison::ECM_None", NoResetToDefault))
	float																													TimeToStart;

#pragma endregion 

#pragma region ReadOnly

protected:
	
	/**
	 * Current State of the Interactable.
	 * Is subject to State Machine.
	 * @see [State Machine] https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Validations#state-machine
	 */
	UPROPERTY(ReplicatedUsing=OnRep_InteractableState, SaveGame, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	EInteractableStateV2																						InteractableState;
	
	/**
	 * How many Lifecycles remain until this Interactable is Finished.
	 */
	UPROPERTY(Replicated, SaveGame, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	int32																												RemainingLifecycleCount;

	/**
	 * List of Interactables which are dependant on this Interactable.
	 * To manage dependencies, use following functions:
	 * - AddInteractionDependency
	 * - RemoveInteractionDependency
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	TArray<TScriptInterface<IActorInteractableInterface>>								InteractionDependencies;

	/**
	 * Cached Collision Shape Settings.
	 * Filled when Collision Shapes are registered.
	 * Once Collision Shape is unregistered, it reads its cached settings and returns to pre-interaction Collision Settings.
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="MounteaInteraction|Read Only", meta=(DisplayThumbnail = false, ShowOnlyInnerProperties))
	mutable TMap<TObjectPtr<UPrimitiveComponent>, FCollisionShapeCache>	CachedCollisionShapesSettings;
	
	/**
	 * List of Highlightable Components.
	 * * Set Overlap Events to true
	 * * Response to Collision Channel to overlap
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	TArray<TObjectPtr<UMeshComponent>>														HighlightableComponents;
	
	/**
	 * List of Collision Components.
	 * Those component will be updated to:
	 * * Allow Render Custom Depth
	 * * Use specific StencilID
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	TArray<TObjectPtr<UPrimitiveComponent>>													CollisionComponents;

	/**
	 * Cached value which is by default set to Interaction Weight.
	 * Used when removing Interactable from Dependencies.
	 */
	UPROPERTY(VisibleAnywhere, Category="MounteaInteraction|Read Only")
	int32																												CachedInteractionWeight;
	
	UPROPERTY()
	FTimerHandle																									Timer_Interaction;
	UPROPERTY()
	FTimerHandle																									Timer_Cooldown;
	UPROPERTY()
	FTimerHandle																									Timer_ProgressExpiration;

private:

	/**
	 * Interactor which is using this Interactable.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_ActiveInteractor, SaveGame, VisibleAnywhere, Category="MounteaInteraction|Read Only", meta=(DisplayThumbnail = false))
	TScriptInterface<IActorInteractorInterface>													Interactor = nullptr;

	UPROPERTY(VisibleAnywhere, Category="MounteaInteraction|Read Only", meta=(NoResetToDefault))
	uint8 bInteractableInitialized : 1;
	
#pragma endregion

#pragma endregion

#pragma region Editor

#if (!UE_BUILD_SHIPPING || WITH_EDITOR)
#if WITH_EDITOR
protected:

	DECLARE_EVENT(UActorInteractableComponentBase, FRequestEditorDefaults);
	FRequestEditorDefaults RequestEditorDefaults;
	virtual void ResetDefaults();
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;

#endif
	
	virtual void DrawDebug();

#endif
	
#pragma endregion 
};

#undef LOCTEXT_NAMESPACE