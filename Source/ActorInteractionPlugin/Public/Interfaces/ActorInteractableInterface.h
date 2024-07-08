// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Helpers/MounteaInteractionHelperEvents.h"
#include "UObject/Interface.h"
#include "ActorInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UActorInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IActorInteractableInterface;
class IActorInteractorInterface;

class UInputAction;

struct FDataTableRowHandle;

enum class EInteractableStateV2 : uint8;
enum class EInteractableLifecycle : uint8;
enum class ECommonInputType : uint8;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorFound, const TScriptInterface<IActorInteractorInterface>&, FoundInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorLost, const TScriptInterface<IActorInteractorInterface>&, LostInteractor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableSelected, const TScriptInterface<IActorInteractableInterface>&, SelectedInteractable);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FInteractorTraced, UPrimitiveComponent*, HitComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, FVector, NormalImpulse, const FHitResult&, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FInteractorOverlapped, UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex, bool, bFromSweep, const FHitResult &, SweepResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FInteractorStopOverlap, UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionCompleted, const float&, FinishTime, const TScriptInterface<IActorInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractionCycleCompleted, const float&, FinishTime, const int32, RemainingLifecycles, const TScriptInterface<IActorInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionStarted, const float&, StartTime, const TScriptInterface<IActorInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionStopped, const float&, StartTime, const TScriptInterface<IActorInteractorInterface>&, CausingInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionCanceled);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLifecycleCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCooldownCompleted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableDependencyChanged, const TScriptInterface<IActorInteractableInterface>&, ChagnedDependency);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableAutoSetupChanged, const bool, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableWeightChanged, const int32&, NewWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableStateChanged, const EInteractableStateV2&, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableOwnerChanged, const AActor*, NewOwner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableCollisionChannelChanged, const ECollisionChannel, NewCollisionChannel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLifecycleModeChanged, const EInteractableLifecycle&, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLifecycleCountChanged, const int32, NewLifecycleCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownPeriodChanged, const float, NewCooldownPeriod);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorChanged, const TScriptInterface<IActorInteractorInterface>&, NewInteractor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIgnoredInteractorClassAdded, const TSoftClassPtr<UObject>&, IgnoredClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIgnoredInteractorClassRemoved, const TSoftClassPtr<UObject>&, IgnoredClass);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightableComponentAdded, const UMeshComponent*, NewHighlightableComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionComponentAdded, const UPrimitiveComponent*, NewCollisionComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightableComponentRemoved, const UMeshComponent*, RemovedHighlightableComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionComponentRemoved, const UPrimitiveComponent*, RemovedCollisionComp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightTypeChanged, const EHighlightType&, NewHighlightType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightMaterialChanged, const UMaterialInterface*, NewHighlightMaterial);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableDependencyStarted, const TScriptInterface<IActorInteractableInterface>&, NewMaster);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableDependencyStopped, const TScriptInterface<IActorInteractableInterface>&, FormerMaster);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableWidgetVisibilityChanged, const bool, bIsVisible);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractionDeviceChanged, const ECommonInputType, DeviceType, const FName&, DeviceName, const FString&, DeviceHardwareName);
/**
 * 
 */
class ACTORINTERACTIONPLUGIN_API IActorInteractableInterface
{
	GENERATED_BODY()
 

#pragma region InteractionEvents

protected:
		
	/**
	 * Event bound to OnInteractableSelected.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractableSelectedEvent(const TScriptInterface<IActorInteractableInterface>& Interactable);
	
	/**
	 * Event bound to OnInteractorFound.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractorFoundEvent(const TScriptInterface<IActorInteractorInterface>& FoundInteractor);

	/**
	 * Event bound to OnInteractorLost.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractorLostEvent(const TScriptInterface<IActorInteractorInterface>& LostInteractor);
	
	/**
	 * Event bound to OnInteractorOverlapped.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractableBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/**
	 * Event bound to OnInteractorStopOverlap.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractableStopOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**
	 * Event bound to OnInteractorTraced.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractableTracedEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/**
	 * Event bound to OnInteractionCompleted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractionCompletedEvent(const float& FinishTime, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);

	/**
	 * Event called once Interaction Cycle is completed.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractionCycleCompletedEvent(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);

	/**
	 * Event bound to OnInteractionStarted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractionStartedEvent(const float& StartTime, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);

	/**
	 * Event bound to OnInteractionStopped.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractionStoppedEvent(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);

	/**
	 * Event bound to OnInteractionCanceled.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnInteractionCanceledEvent();
	
	/**
	 * Event bound to OnLifecycleCompleted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnLifecycleCompletedEvent();

	/**
	 * Event bound to OnCooldownCompleted.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Interaction|Interactable")
	void OnCooldownCompletedEvent();

#pragma endregion
	
public:

	/**
	 * Return whether this Interactable does have any Interactor.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	bool DoesHaveInteractor() const;
	virtual bool DoesHaveInteractor_Implementation() const = 0;

	/**
	 * Returns whether this Interactable is being autosetup or not. 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	bool DoesAutoSetup() const;
	virtual bool DoesAutoSetup_Implementation() const = 0;

	/**
	 * Sets whether this Interactable will be Autosetup.
	 * Setup Type is variable exposed in Interaction|Required panel.
	 * @param NewValue This value will be used as SetupType value.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void ToggleAutoSetup(const ESetupType& NewValue);
	virtual void ToggleAutoSetup_Implementation(const ESetupType& NewValue) = 0;


	
	/**
	 * Tries to set state of this Interactable to Active. 
	 * If fails, returns False and updates ErrorMessage
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	bool ActivateInteractable(FString& ErrorMessage);
	virtual bool ActivateInteractable_Implementation(FString& ErrorMessage) = 0;

	/**
	 * Tries to set state of this Interactable to Awake. 
	 * If fails, returns False and updates ErrorMessage
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	bool WakeUpInteractable(FString& ErrorMessage);
	virtual bool WakeUpInteractable_Implementation(FString& ErrorMessage) = 0;

	/**
	 * Tries to set state of this Interactable to Completed. 
	 * If fails, returns False and updates ErrorMessage
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	bool CompleteInteractable(FString& ErrorMessage);
	virtual bool CompleteInteractable_Implementation(FString& ErrorMessage) = 0;

	/**
	 * Tries to set state of this Interactable to Disabled. 
	 * If fails, returns False and updates ErrorMessage
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void DeactivateInteractable();
	virtual void DeactivateInteractable_Implementation() = 0;

	/**
	 * Tries to Pause Interaction.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void PauseInteraction(const float ExpirationTime, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);
	virtual void PauseInteraction_Implementation(const float ExpirationTime, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) = 0;


	
	/**
	 * Event bound to Interactor's OnInteractableSelected.
	 * If Interactor selects any Interactable, this Event is called and selected Interactable is Activated, while others are set back to Awaken state.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractableSelected(const TScriptInterface<IActorInteractableInterface>& Interactable);
	virtual void InteractableSelected_Implementation(const TScriptInterface<IActorInteractableInterface>& Interactable) = 0;

	/**
	 * 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractableLost(const TScriptInterface<IActorInteractableInterface>& Interactable);
	virtual void InteractableLost_Implementation(const TScriptInterface<IActorInteractableInterface>& Interactable) = 0;

	/**
	 * Event called once Interactor is found.
	 * Called by OnInteractorFound
	 * Calls OnInteractorFoundEvent
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractorFound(const TScriptInterface<IActorInteractorInterface>& FoundInteractor);
	virtual void InteractorFound_Implementation(const TScriptInterface<IActorInteractorInterface>& FoundInteractor) = 0;

	/**
	 * Event called once Interactor is lost.
	 * Called by OnInteractorLost
	 * Calls OnInteractorLostEvent
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractorLost(const TScriptInterface<IActorInteractorInterface>& LostInteractor);
	virtual void InteractorLost_Implementation(const TScriptInterface<IActorInteractorInterface>& LostInteractor) = 0;

	/**
	 * Event called once Interaction is completed.
	 * Called from OnInteractionCompleted
	 * Calls OnInteractionCompletedEvent
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractionCompleted(const float& TimeCompleted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);
	virtual void InteractionCompleted_Implementation(const float& TimeCompleted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) = 0;

	/**
	 * Event called once Interaction Cycle is completed.
	 * Calls OnInteractionCycleCompletedEvent
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractionCycleCompleted(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);
	virtual void InteractionCycleCompleted_Implementation(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) = 0;

	/**
	 * Event called once Interaction Starts.
	 * Called by OnInteractionStarted
	 * Calls OnInteractionStartedEvent
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractionStarted(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);
	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) = 0;

	/**
	 * Event called once Interaction Stops.
	 * Called by OnInteractionStopped
	 * Calls OnInteractionStoppedEvent
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractionStopped(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);
	virtual void InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor) = 0;

	/**
	 * Event called once Interaction is Canceled.
	 * Called by OnInteractionCanceled
	 * Calls OnInteractionCanceledEvent
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractionCanceled();
	virtual void InteractionCanceled_Implementation() = 0;

	/**
	 * Event called once Interaction Lifecycles is Completed.
	 * Called by OnLifecycleCompleted
	 * Calls OnLifecycleCompletedEvent
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractionLifecycleCompleted();
	virtual void InteractionLifecycleCompleted_Implementation() = 0;

	/**
	 * Event called once Interaction Cooldown is Completed and Interactable can be Interacted with again.
	 * Called by OnCooldownCompleted
	 * Calls OnCooldownCompletedEvent
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractionCooldownCompleted();
	virtual void InteractionCooldownCompleted_Implementation() = 0;

	
	/**
	 * Optimized request for Interactables.
	 * Can be overridden in C++ for specific class needs.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	bool CanInteract() const;
	virtual bool CanInteract_Implementation() const = 0;

	/**
	 * Returns whether Interaction can be processed.
	 * Return True if is Awaken and does not have any Interactor yet.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	bool CanBeTriggered() const;
	virtual bool CanBeTriggered_Implementation() const = 0;

	/**
	 * Returns whether Interaction is in process.
	 * Return True if is Active.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	bool IsInteracting() const;
	virtual bool IsInteracting_Implementation() const = 0;

	/**
	 * Returns Default Interactable State.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	EInteractableStateV2 GetDefaultState() const;
	virtual EInteractableStateV2 GetDefaultState_Implementation() const = 0;

	/**
	 * Tries to set Default Interactable State.
	 *
	 * @param NewState Value which will be set as new Default Interactable State.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, CallInEditor, Category="Mountea|Interaction|Interactable")
	void SetDefaultState(const EInteractableStateV2 NewState);
	virtual void SetDefaultState_Implementation(const EInteractableStateV2 NewState) = 0;

	/**
	 * Returns State of Interactable.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	EInteractableStateV2 GetState() const;
	virtual EInteractableStateV2 GetState_Implementation() const = 0;

	/**
	 * Sets State of Interactable.
	 * @param NewState Value of the State to be set
	 *
	 * SetState is driven by StateMachine.
	 * StateMachine is available on Wiki:
	 * * https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactable-Component-Validations#state-machine
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetState(const EInteractableStateV2 NewState);
	virtual void SetState_Implementation(const EInteractableStateV2 NewState) = 0;

	/**
	 * Starts Highlight for all Highlightable Components.
	 * Requires Rendering Custom depth in Project Settings.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void StartHighlight();
	virtual void StartHighlight_Implementation() = 0;

	/**
	 * Stops Highlight for all Highlightable Components.
	 * Requires Rendering Custom depth in Project Settings.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void StopHighlight();
	virtual void StopHighlight_Implementation() = 0;

	/**
	 * Returns Interaction Progress.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	float GetInteractionProgress() const;
	virtual float GetInteractionProgress_Implementation() const = 0;

	/**
	 * Returns Interaction Period.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	float GetInteractionPeriod() const;
	virtual float GetInteractionPeriod_Implementation() const = 0;

	/**
	 * Sets Interaction Period.
	 * Values are clamped and verified:
	 *
	 * @param NewPeriod Value to be set as new Interaction Period. Is validated:
	 * - -1 = immediate
	 * - values less than 0 and larger than -1 are 0.1
	 * - 0 = 0.1s
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetInteractionPeriod(const float NewPeriod);
	virtual void SetInteractionPeriod_Implementation(const float NewPeriod) = 0;

	/**
	 * Returns Interactor which is interacting with this Interactable.
	 * If no Interactor, will return nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	TScriptInterface<IActorInteractorInterface> GetInteractor() const;
	virtual TScriptInterface<IActorInteractorInterface> GetInteractor_Implementation() const = 0;

	/**
	 * Sets Interactor as Active Interactor.
	 * OnInteractorChanged is called upon successful change.
	 *
	 * @param NewInteractor Value to be set as a new Interactor. Can be null.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetInteractor(const TScriptInterface<IActorInteractorInterface>& NewInteractor);
	virtual void SetInteractor_Implementation(const TScriptInterface<IActorInteractorInterface>& NewInteractor) = 0;

	/**
	 * Returns Interactable Weight.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	int32 GetInteractableWeight() const;
	virtual int32 GetInteractableWeight_Implementation() const = 0;

	/**
	 * Sets new Interactable Weight value.
	 *
	 * @param NewWeight Value to be set as new Interactable Weight. Is validated:
	 * Min value is 0.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetInteractableWeight(const int32 NewWeight);
	virtual void SetInteractableWeight_Implementation(const int32 NewWeight) = 0;

	/**
	 * Return Interactable Owner.
	 * This will be most likely same as the GetOwner, however, there is a way to override this default value.
	 * Useful for very complex interactions.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	AActor* GetInteractableOwner() const;
	virtual AActor* GetInteractableOwner_Implementation() const = 0;

	/**
	 * Returns Collision Channel.
	 * Both Object and Trace Channels are allowed.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	ECollisionChannel GetCollisionChannel() const;
	virtual ECollisionChannel GetCollisionChannel_Implementation() const = 0;

	/**
	 * Sets new Collision Channel.
	 * Both Object and Trace Channels are allowed.
	 * @param NewChannel New Collision Channel to be used for this Interactable.
	 *
	 * Interaction specific channel are our strong recommendation.
	 * For usage and setup, take a look at 'Examples' project from Mountea Framework GitHub page.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetCollisionChannel(const TEnumAsByte<ECollisionChannel>& NewChannel);
	virtual void SetCollisionChannel_Implementation(const TEnumAsByte<ECollisionChannel>& NewChannel) = 0;

	/**
	 * Returns Lifecycle Mode.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	EInteractableLifecycle GetLifecycleMode() const;
	virtual EInteractableLifecycle GetLifecycleMode_Implementation() const = 0;

	/**
	 * Set new Lifecycle Mode.
	 * @param NewMode New Lifecycle Mode to be used for this Interactable.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetLifecycleMode(const EInteractableLifecycle& NewMode);
	virtual void SetLifecycleMode_Implementation(const EInteractableLifecycle& NewMode) = 0;

	/**
	 * Returns Lifecycle Count of this Interactable.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	int32 GetLifecycleCount() const;
	virtual int32 GetLifecycleCount_Implementation() const = 0;

	/**
	 * Set new Lifecycle Count.
	 * @param NewLifecycleCount New Lifecycle Count to be used for this Interactable.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetLifecycleCount(const int32 NewLifecycleCount);
	virtual void SetLifecycleCount_Implementation(const int32 NewLifecycleCount) = 0;

	/**
	 * Returns how many Lifecycles remain.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	int32 GetRemainingLifecycleCount() const;
	virtual int32 GetRemainingLifecycleCount_Implementation() const = 0;

	/**
	 * Returns Cooldown Period in seconds.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	float GetCooldownPeriod() const;
	virtual float GetCooldownPeriod_Implementation() const = 0;

	/**
	 * Sets new Cooldown Period.
	 * @param NewCooldownPeriod Value in seconds to be used as Coolddown Period.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetCooldownPeriod(const float NewCooldownPeriod);
	virtual void SetCooldownPeriod_Implementation(const float NewCooldownPeriod) = 0;

	/**
	 * Returns value of Comparison Method.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	ETimingComparison GetComparisonMethod() const;
	virtual ETimingComparison GetComparisonMethod_Implementation() const = 0;

	/**
	 * Sets new value of Comparison Method.
	 * @param Value Value of new Comparison Method.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetComparisonMethod(const ETimingComparison Value);
	virtual void SetComparisonMethod_Implementation(const ETimingComparison Value) = 0;
	
	/**
	 * Will add Interaction Dependency to List of Dependencies. 
	 * All dependencies are affected by Interaction State of this Interactable. 
	 * Interaction Dependency is Suppressed while its Master is Active.
	 * Duplicates are not allowed and will be filtered out.
	 * 
	 * @param InteractionDependency Dependency which will be added. Null or duplicates are not allowed.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void AddInteractionDependency(const TScriptInterface<IActorInteractableInterface>& InteractionDependency);
	virtual void AddInteractionDependency_Implementation(const TScriptInterface<IActorInteractableInterface>& InteractionDependency) = 0;

	/**
	 * Will remove Interaction Dependency from List of Dependencies.
	 * All dependencies are affected by Interaction State of this Interactable. 
	 * Interaction Dependency is Suppressed while its Master is Active.
	 * If Dependency is not present, nothing happens.
	 * 
	 * @param InteractionDependency Dependency which will be removed.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void RemoveInteractionDependency(const TScriptInterface<IActorInteractableInterface>& InteractionDependency);
	virtual void RemoveInteractionDependency_Implementation(const TScriptInterface<IActorInteractableInterface>& InteractionDependency) = 0;

	/**
	 * Return List of Dependencies.
	 * All dependencies are affected by Interaction State of this Interactable. 
	 * Interaction Dependency is Suppressed while its Master is Active.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	TArray<TScriptInterface<IActorInteractableInterface>> GetInteractionDependencies() const;
	virtual TArray<TScriptInterface<IActorInteractableInterface>> GetInteractionDependencies_Implementation() const = 0;

	/**
	 * Function responsible for updating Interaction Dependencies.
	 * Does process all hooked up Interactables in predefined manner.
	 * Is called once State is updated.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void ProcessDependencies();
	virtual void ProcessDependencies_Implementation() = 0;


	/**
	 * Triggers cooldown for this Interactable.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	bool TriggerCooldown();
	virtual bool TriggerCooldown_Implementation() = 0;

	/**
	 * Toggles visibility of the widget.
	 * @param IsVisible Whether the widget should be visible or not.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void ToggleWidgetVisibility(const bool IsVisible);
	virtual void ToggleWidgetVisibility_Implementation(const bool IsVisible) = 0;

	/**
	 * Returns list of ignored classes.
	 * Those are classes which will be ignored for interaction events and won't trigger them.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	TArray<TSoftClassPtr<UObject>> GetIgnoredClasses() const;
	virtual TArray<TSoftClassPtr<UObject>> GetIgnoredClasses_Implementation() const = 0;

	/**
	 * Force set Ignored Classes. 
	 * @param NewIgnoredClasses New array of Ignored Classes. Can be given empty array.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetIgnoredClasses(const TArray<TSoftClassPtr<UObject>>& NewIgnoredClasses);
	virtual void SetIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& NewIgnoredClasses) = 0;

	/**
	 * Will add a class to Ignored Class List.
	 * @param AddIgnoredClass Class to be ignored.
	 *
	 * Only objects implementing ActorInteractorInterface will be affected!
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void AddIgnoredClass(const TSoftClassPtr<UObject>& AddIgnoredClass);
	virtual void AddIgnoredClass_Implementation(const TSoftClassPtr<UObject>& AddIgnoredClass) = 0;

	/**
	 * Will add classes to Ignored Class List.
	 * @param AddIgnoredClasses Array of classes to be ignored.
	 * 
	 * Only objects implementing ActorInteractorInterface will be affected!
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void AddIgnoredClasses(const TArray<TSoftClassPtr<UObject>>& AddIgnoredClasses);
	virtual void AddIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& AddIgnoredClasses) = 0;

	/**
	 * Will remove a class from Ignored Class List.
	 * @param RemoveIgnoredClass Class to be accepted.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void RemoveIgnoredClass(const TSoftClassPtr<UObject>& RemoveIgnoredClass);
	virtual void RemoveIgnoredClass_Implementation(const TSoftClassPtr<UObject>& RemoveIgnoredClass) = 0;

	/**
	 * Will remove classes from Ignored Class List.
	 * @param RemoveIgnoredClasses Array of classes to be accepted.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void RemoveIgnoredClasses(const TArray<TSoftClassPtr<UObject>>& RemoveIgnoredClasses);
	virtual void RemoveIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& RemoveIgnoredClasses) = 0;

	/**
	 * Returns all Collision Components.
	 * Collision Components might be both Shape Components (Box Collision etc.) or Mesh Components (Static Mesh etc.).
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	TArray<UPrimitiveComponent*> GetCollisionComponents() const;
	virtual TArray<UPrimitiveComponent*> GetCollisionComponents_Implementation() const = 0;

	/**
	 * Tries to add new Collision Component. No duplicates allowed. Null is not accepted.
	 * Calls OnCollisionComponentAddedEvent.
	 * @param CollisionComp Component to be added into list of Collision Components.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void AddCollisionComponent(UPrimitiveComponent* CollisionComp);
	virtual void AddCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp) = 0;

	/**
	 * Tries to add Collision Components. Calls AddCollisionComponent for each component.
	 * OnCollisionComponentAddedEvent is called for each component added.  No duplicates allowed. Nulls are not accepted.
	 * @param CollisionComponents			List of components to be added into list of Collision Components.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void AddCollisionComponents(const TArray<UPrimitiveComponent*>& CollisionComponents);
	virtual void AddCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents) = 0;

	/**
	 * Tries to remove Collision Component if registered. Null is not accepted.
	 * Calls OnCollisionComponentRemovedEvent.
	 * @param CollisionComp Component to be removed from list of Collision Components.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void RemoveCollisionComponent(UPrimitiveComponent* CollisionComp);
	virtual void RemoveCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp) = 0;

	/**
	 * Tries to remove Collision Components. Calls RemoveCollisionComponent for each component.
	 * OnCollisionComponentRemovedEvent is called for each component removed. Nulls are not accepted.
	 * @param CollisionComponents List of components to be removed from list of Collision Components.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void RemoveCollisionComponents(const TArray<UPrimitiveComponent*>& CollisionComponents);
	virtual void RemoveCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& CollisionComponents)  = 0;

	/**
	 * Returns array of Highlightable Components.
	 * Collision Components are Mesh Components.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	TArray<UMeshComponent*> GetHighlightableComponents() const;
	virtual TArray<UMeshComponent*> GetHighlightableComponents_Implementation()  const = 0;

	/**
	 * Tries to add new Highlightable Component.
	 * Calls OnHighlightableComponentAdded.
	 * Duplicates or null not allowed.
	 * @param HighlightableComp Mesh Component to be added to List of Highlightable Components
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void AddHighlightableComponent(UMeshComponent* HighlightableComp);
	virtual void AddHighlightableComponent_Implementation(UMeshComponent* HighlightableComp)  = 0;

	/**
	 * Tries to add new Highlightable Components. Calls AddHighlightableComponent for each Component.
	 * Calls OnHighlightableComponentAdded.
	 * @param HighlightableComponents List of Mesh Components to be added to List of Highlightable Components
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void AddHighlightableComponents(const TArray<UMeshComponent*>& HighlightableComponents);
	virtual void AddHighlightableComponents_Implementation(const TArray<UMeshComponent*>& HighlightableComponents)  = 0;

	/**
	 * Tries to remove Highlightable Component.
	 * Calls OnHighlightableComponentRemoved.
	 * Null not allowed.
	 * @param HighlightableComp Mesh Component to be removed from List of Highlightable Components
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void RemoveHighlightableComponent(UMeshComponent* HighlightableComp);
	virtual void RemoveHighlightableComponent_Implementation(UMeshComponent* HighlightableComp)  = 0;

	/**
	 * Tries to remove Highlightable Components. Calls RemoveHighlightableComponent for each Component.
	 * Calls OnHighlightableComponentRemoved.
	 * @param HighlightableComponents List of Mesh Components to be removed from List of Highlightable Components
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void RemoveHighlightableComponents(const TArray<UMeshComponent*>& HighlightableComponents);
	virtual void RemoveHighlightableComponents_Implementation(const TArray<UMeshComponent*>& HighlightableComponents)  = 0;

	/**
	 * Returns all Collision Overrides.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	TArray<FName> GetCollisionOverrides() const;
	virtual TArray<FName> GetCollisionOverrides_Implementation() const = 0;

	/**
	 * Returns all Highlightable Overrides.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	TArray<FName> GetHighlightableOverrides() const;
	virtual TArray<FName> GetHighlightableOverrides_Implementation() const = 0;

	/**
	 * Returns Interactable Name.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	FText GetInteractableName() const;
	virtual FText GetInteractableName_Implementation() const = 0;

	/**
	 * Sets new Interactable Name.
	 * @param NewName Name to set as Interactable Name.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetInteractableName(const FText& NewName);
	virtual void SetInteractableName_Implementation(const FText& NewName) = 0;

	/**
	 * Development Only.
	 * Toggles debug On/Off.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void ToggleDebug();
	virtual void ToggleDebug_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	FDebugSettings GetDebugSettings() const;
	virtual FDebugSettings GetDebugSettings_Implementation() const = 0;

	/**
	 * Helper function.
	 * Looks for Collision and Highlightable Overrides and binds them.
	 * Looks for Parent Component which will be set as Collision Mesh and Highlighted Mesh.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void FindAndAddCollisionShapes();
	virtual void FindAndAddCollisionShapes_Implementation() = 0;

	/**
	 * Helper function.
	 * Looks for Collision and Highlightable Overrides and binds them.
	 * Looks for Parent Component which will be set as Collision Mesh and Highlighted Mesh.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void FindAndAddHighlightableMeshes();
	virtual void FindAndAddHighlightableMeshes_Implementation() = 0;

	/**
	 * Binds Collision Events for specified Primitive Component.
	 * Caches Primitive Component collision settings.
	 * Automatically called when Interactable is:
	 * * Awaken
	 * * Asleep
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void BindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const;
	virtual void BindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const = 0;

	/**
	 * Unbinds Collision Events for specified Primitive Component.
	 * Is using cached values to return Primitive Component to pre-interaction state.
	 * Automatically called when Interactable is:
	 * * Deactivated
	 * * Finished
	 * * Cooldown
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void UnbindCollisionShape(UPrimitiveComponent* PrimitiveComponent) const;
	virtual void UnbindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const = 0;

	/**
	 * Binds Highlightable Events for specified Mesh Component.
	 * Automatically called when Interactable is:
	 * * Awaken
	 * * Asleep
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void BindHighlightableMesh(UMeshComponent* MeshComponent) const;
	virtual void BindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const = 0;

	/**
	 * Unbinds Highlightable Events for specified Mesh Component.
	 * Automatically called when Interactable is:
	 * * Deactivated
	 * * Finished
	 * * Cooldown
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void UnbindHighlightableMesh(UMeshComponent* MeshComponent) const;
	virtual void UnbindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const = 0;

	/**
	 * Returns Interactable Data.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	FDataTableRowHandle GetInteractableData() const;
	virtual FDataTableRowHandle GetInteractableData_Implementation() const = 0;

	/**
	 * Sets new Interactable Data.
	 * @param NewData New Data to be used as Interactable Data.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetInteractableData(FDataTableRowHandle NewData);
	virtual void SetInteractableData_Implementation(FDataTableRowHandle NewData) = 0;

	/**
	 * Return Highlightable Type of this Interactable Component.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	EHighlightType GetHighlightType() const;
	virtual EHighlightType GetHighlightType_Implementation() const = 0;

	/**
	 * Tries to set new Highlight Type.
	 * @param NewHighlightType Value of Highlight type.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetHighlightType(const EHighlightType NewHighlightType);
	virtual void SetHighlightType_Implementation(const EHighlightType NewHighlightType) = 0;

	/**
	 * Returns Highlight Material if any specified.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactable")
	UMaterialInterface* GetHighlightMaterial() const;
	virtual UMaterialInterface* GetHighlightMaterial_Implementation() const = 0;

	/**
	 * Tries to set new Highlight Material.
	 * @param NewHighlightMaterial Material Instance to be used as new HighlightMaterial.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetHighlightMaterial(UMaterialInterface* NewHighlightMaterial);
	virtual void SetHighlightMaterial_Implementation(UMaterialInterface* NewHighlightMaterial) = 0;

	/**
	 * Callback function when Interactable Dependency starts.
	 * @param NewMaster The new master interactable.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractableDependencyStartedCallback(const TScriptInterface<IActorInteractableInterface>& NewMaster);
	virtual void InteractableDependencyStartedCallback_Implementation(const TScriptInterface<IActorInteractableInterface>& NewMaster) = 0;

	/**
	 * Callback function when Interactable Dependency stops.
	 * @param FormerMaster The former master interactable.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactable")
	void InteractableDependencyStoppedCallback(const TScriptInterface<IActorInteractableInterface>& FormerMaster);
	virtual void InteractableDependencyStoppedCallback_Implementation(const TScriptInterface<IActorInteractableInterface>& FormerMaster) = 0;

	/**
	 * Finds default values from Developer settings and tries to set them for this component.
	 * Will override current settings!
	 * Will set those values only if not null.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable", meta=(DisplayName="SetDefaults"))
	void SetDefaults();
	virtual void SetDefaults_Implementation()  = 0;

	/**
	 *	Gets Interactable Filter Gameplay Tags.
	 * 
	 * @return		Container of allowed filter Gameplay Tags. 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	FGameplayTagContainer GetInteractableCompatibleTags() const;
	virtual FGameplayTagContainer GetInteractableCompatibleTags_Implementation() const = 0;

	/**
	 * Sets the Interactable Filter Gameplay Tags.
	 * 
	 * @param Tags Container of tags to set.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void SetInteractableCompatibleTags(const FGameplayTagContainer& Tags);
	virtual void SetInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) = 0;

	/**
	 * Adds a tag to the Interactable Filter Gameplay Tags.
	 * 
	 * @param Tag Tag to add.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void AddInteractableCompatibleTag(const FGameplayTag& Tag);
	virtual void AddInteractableCompatibleTag_Implementation(const FGameplayTag& Tag) = 0;

	/**
	 * Adds multiple tags to the Interactable Filter Gameplay Tags.
	 * 
	 * @param Tags Container of tags to add.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void AddInteractableCompatibleTags(const FGameplayTagContainer& Tags);
	virtual void AddInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) = 0;

	/**
	 * Removes a tag from the Interactable Filter Gameplay Tags.
	 * 
	 * @param Tag Tag to remove.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void RemoveInteractableCompatibleTag(const FGameplayTag& Tag);
	virtual void RemoveInteractableCompatibleTag_Implementation(const FGameplayTag& Tag) = 0;

	/**
	 * Removes multiple tags from the Interactable Filter Gameplay Tags.
	 * 
	 * @param Tags Container of tags to remove.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void RemoveInteractableCompatibleTags(const FGameplayTagContainer& Tags);
	virtual void RemoveInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) = 0;

	/**
	 * Clears all tags from the Interactable Filter Gameplay Tags.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void ClearInteractableCompatibleTags();
	virtual void ClearInteractableCompatibleTags_Implementation() = 0;

	/**
	 * Checks if this interactable has an associated interactor.
	 * This function is used to determine if there is an interactor currently interacting with this interactable.
	 *
	 * @return True if an interactor is present, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	bool HasInteractor() const;
	virtual bool HasInteractor_Implementation() const = 0;

	/**
	 * Gets the owning actor of this Interactable.
	 * This function returns the actor that owns or Interactable or the Interactable itself if it's an Actor.
	 *
	 * @return The actor owning this Interactable.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	/**
	 * Helper function to provide debug and useful information.
	 * This function returns a string representation of the Interactable, useful for logging and debugging.
	 *
	 * @return A string containing debug and useful information about the Interactable.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	FString ToString() const;
	virtual FString ToString_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactable")
	void OnInputDeviceChanged(const ECommonInputType DeviceType, const FName& DeviceName, const FString& DeviceHardwareName);
	virtual void OnInputDeviceChanged_Implementation(const ECommonInputType DeviceType, const FName& DeviceName, const FString& DeviceHardwareName) =0;
	
	
	virtual FOnInteractableSelected& GetOnInteractableSelectedHandle() = 0;
	virtual FInteractorFound& GetOnInteractorFoundHandle() = 0;
	virtual FInteractorLost& GetOnInteractorLostHandle() = 0;
	virtual FInteractorTraced& GetOnInteractorTracedHandle() = 0;
	virtual FInteractorOverlapped& GetOnInteractorOverlappedHandle() = 0;
	virtual FInteractorStopOverlap& GetOnInteractorStopOverlapHandle() = 0;
	virtual FInteractionCompleted& GetOnInteractionCompletedHandle() = 0;
	virtual FInteractionCycleCompleted& GetOnInteractionCycleCompletedHandle() = 0;
	virtual FInteractionStarted& GetOnInteractionStartedHandle() = 0;
	virtual FInteractionStopped& GetOnInteractionStoppedHandle() = 0;
	virtual FInteractionCanceled& GetOnInteractionCanceledHandle() = 0;
	virtual FInteractableDependencyChanged& GetInteractableDependencyChangedHandle() = 0;

	virtual FInteractableDependencyStarted& GetInteractableDependencyStarted() = 0;
	virtual FInteractableDependencyStopped& GetInteractableDependencyStopped() = 0;

	virtual FHighlightTypeChanged& GetHighlightTypeChanged() = 0;
	virtual FHighlightMaterialChanged& GetHighlightMaterialChanged() = 0;

	virtual FTimerHandle& GetCooldownHandle() = 0;
	virtual FInteractableStateChanged& GetInteractableStateChanged() = 0;

	virtual FInteractableWidgetVisibilityChanged& GetInteractableWidgetVisibilityChangedHandle() = 0;

	virtual FInputActionConsumed& GetInputActionConsumedHandle() = 0;
	virtual FInteractionDeviceChanged& GetInteractionDeviceChangedHandle() = 0;
};