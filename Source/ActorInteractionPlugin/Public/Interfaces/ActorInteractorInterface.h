// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"

#include "Engine/EngineTypes.h"
#include "Helpers/MounteaInteractionHelperEvents.h"

#include "ActorInteractorInterface.generated.h"

struct FGameplayTag;
class UInputMappingContext;

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable, MinimalAPI)
class UActorInteractorInterface : public UInterface
{
	GENERATED_BODY()
};

enum class EInteractorStateV2 : uint8;
class IActorInteractableInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableSelected,			const TScriptInterface<IActorInteractableInterface>&, SelectedInteractable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableFound,				const TScriptInterface<IActorInteractableInterface>&, FoundInteractable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableLost,					const TScriptInterface<IActorInteractableInterface>&, LostInteractable);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionKeyPressed,		const float&, TimeKeyPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionKeyReleased,		const float&, TimeKeyReleased);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIgnoredActorAdded,			const AActor*, AddedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIgnoredActorRemoved,		const AActor*, RemovedActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStateChanged,					const EInteractorStateV2&, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionChanged,				const TEnumAsByte<ECollisionChannel>&, NewCollisionChannel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAutoActivateChanged,		const bool, NewAutoActivate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorTagChanged,		const FGameplayTag&, NewTag);

/**
 * 
 */
class ACTORINTERACTIONPLUGIN_API IActorInteractorInterface
{
	GENERATED_BODY()

public:

#pragma region Events

public:
	
	/**
	 * Event bound to OnInteractableSelected event.
	 * Once OnInteractableSelected is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param SelectedInteractable Interactable Component which is being interacted with
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void OnInteractableSelectedEvent(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable);

	/**
	 * Event bound to OnInteractableFound event.
	 * Once OnInteractableFound is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param FoundInteractable Interactable Component which is found
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void OnInteractableFoundEvent(const TScriptInterface<IActorInteractableInterface>& FoundInteractable);
	
	/**
	 * Event bound to OnInteractableLost event.
	 * Once OnInteractableLost is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param LostInteractable Interactable Component which is lost
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void OnInteractableLostEvent(const TScriptInterface<IActorInteractableInterface>& LostInteractable);

	/**
	 * Event bound to OnInteractionKeyPressed event.
	 * Once OnInteractionKeyPressed is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param TimeKeyPressed Time Key was pressed
	 * @param PressedKey Key which was pressed
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void OnInteractionKeyPressedEvent(const float& TimeKeyPressed);

	/**
	 * Event bound to OnInteractionKeyReleased event.
	 * Once OnInteractionKeyReleased is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param TimeKeyReleased Time Key was released
	 * @param ReleasedKey Key which was released
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void OnInteractionKeyReleasedEvent(const float& TimeKeyReleased);
	
	/**
	 * Event bound to OnStateChanged event.
	 * Once OnStateChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewState New State if this Interactor
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void OnInteractorStateChanged(const EInteractorStateV2& NewState);

	/**
	 * Event bound to OnCollisionChanged event.
	 * Once OnCollisionChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewCollisionChannel New Collision Channel set as Response Channel
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void OnInteractorCollisionChanged(const TEnumAsByte<ECollisionChannel>& NewCollisionChannel);

#pragma endregion


	/**
	 * Returns whether this Interactor is Valid for Interactions.
	 * Disabled/Suppressed Interactors are not valid.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	bool IsValidInteractor() const;
	virtual bool IsValidInteractor_Implementation() const = 0;

	/**
	 * Compares Interactables.
	 * For details of Evaluation visit GitHub Wiki: https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Interactor-Evaluation
	 * 
	 * @param FoundInteractable Interactable which was found
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void EvaluateInteractable(const TScriptInterface<IActorInteractableInterface>& FoundInteractable);
	virtual void EvaluateInteractable_Implementation(const TScriptInterface<IActorInteractableInterface>& FoundInteractable) = 0;

	

	/**
	 * Function to start interaction.
	 * Interaction will start only if CanInteract function evaluates true.
	 * 
	 * @param StartTime Time Interaction has started. 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void StartInteraction(const float StartTime);
	virtual void StartInteraction_Implementation(const float StartTime) = 0;

	/**
	 * Function to stop interaction.
	 * No conditions required.
	 * 
	 * @param StartTime Time Interaction has started.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void StopInteraction(const float StartTime);
	virtual void StopInteraction_Implementation(const float StartTime) = 0;

	

	/**
	 * Tries to Active Interactor by setting Interactor state to Active.
	 * Returns whether it was successful or not.
	 * Either way, Error Message contains useful information what possibly went wrong.
	 * 
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	bool ActivateInteractor(FString& ErrorMessage);
	virtual bool ActivateInteractor_Implementation(FString& ErrorMessage) = 0;

	/**
	 * Tries to Wake Up Interactor by setting Interactor state to Stand By.
	 * Returns whether it was successful or not.
	 * Either way, Error Message contains useful information what possibly went wrong.
	 * 
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	bool EnableInteractor(FString& ErrorMessage);
	virtual bool EnableInteractor_Implementation(FString& ErrorMessage) = 0;

	/**
	 * Tries to Suppress Interactor by setting Interactor state to Suppressed.
	 * Returns whether it was successful or not.
	 * Either way, Error Message contains useful information what possibly went wrong.
	 * 
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	bool SuppressInteractor(FString& ErrorMessage);
	virtual bool SuppressInteractor_Implementation(FString& ErrorMessage) = 0;

	/**
	 * Deactivates Interactor.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void DeactivateInteractor();
	virtual void DeactivateInteractor_Implementation() = 0;
	

	/**
	 * Function called by OnInteractableFound.
	 * Suppress all Dependencies.
	 * Calls OnInteractableFoundEvent.
	 * Class EvaluateInteractable with Found Interactable, resulting in possible call to InteractableSelected with this Found Interactable.
	 * @param FoundInteractable Interactable which was found, not necessarily will be set as Active!
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void InteractableFound(const TScriptInterface<IActorInteractableInterface>& FoundInteractable);
	virtual void InteractableFound_Implementation(const TScriptInterface<IActorInteractableInterface>& FoundInteractable) = 0;
	
	/**
	 * Function called by OnInteractableLost. If Lost Interactable is not Active Interactable, then nothing happens.
	 * Resets Active Interactable to null. Sets all Dependencies to same State as this Interactor has.
	 * Calls OnInteractableLostEvent.
	 * @param LostInteractable Interactable which was Lost.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void InteractableLost(const TScriptInterface<IActorInteractableInterface>& LostInteractable);
	virtual void InteractableLost_Implementation(const TScriptInterface<IActorInteractableInterface>& LostInteractable) = 0;

	/**
	 * Function called by OnInteractableSelected.
	 * Calls OnInteractableSelectedEvent.
	 * @param SelectedInteractable Interactable which was selected as new Active Interactable
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void InteractableSelected(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable);
	virtual void InteractableSelected_Implementation(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable) = 0;


	
	/**
	 * Tries to add Actor to Ignored Actors.
	 * Duplicates and null not allowed.
	 * 
	 * @param IgnoredActor Actor to be ignored.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void AddIgnoredActor(AActor* IgnoredActor);
	virtual void AddIgnoredActor_Implementation(AActor* IgnoredActor) = 0;

	/**
	 * Tries to add Actors to Ignored Actors.
	 * Duplicates and null not allowed.
	 * Calls AddIgnoredActor for each.
	 * 
	 * @param IgnoredActors Array of Actors to be ignored.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void AddIgnoredActors(const TArray<AActor*>& IgnoredActors);
	virtual void AddIgnoredActors_Implementation(const TArray<AActor*>& IgnoredActors) = 0;

	/**
	 * Tries to remove Actor from Ignored Actors.
	 * Null not allowed.
	 * 
	 * @param UnignoredActor Actor to be un-ignored.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void RemoveIgnoredActor(AActor* UnignoredActor);
	virtual void RemoveIgnoredActor_Implementation(AActor* UnignoredActor) = 0;

	/**
	 * Tries to remove Actors from Ignored Actors.
	 * Null not allowed.
	 * Calls RemoveIgnoredActor for each.
	 * 
	 * @param UnignoredActors Array of Actors to be un-ignored.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void RemoveIgnoredActors(const TArray<AActor*>& UnignoredActors);
	virtual void RemoveIgnoredActors_Implementation(const TArray<AActor*>& UnignoredActors) = 0;

	/**
	 * Returns list of Ignored Actors.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	TArray<AActor*> GetIgnoredActors() const;
	virtual TArray<AActor*> GetIgnoredActors_Implementation() const = 0;

	

	/**
	 * Adds unique Interaction Dependency.
	 * Those Interactors will be suppressed until this one is not interacting anymore.
	 * Duplicates or null not allowed.
	 * 
	 * @param InteractionDependency Other Interactor to be added to List of Dependencies.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void AddInteractionDependency(const TScriptInterface<IActorInteractorInterface>& InteractionDependency);
	virtual void AddInteractionDependency_Implementation(const TScriptInterface<IActorInteractorInterface>& InteractionDependency) = 0;

	/**
	 * Removes unique Interaction Dependency.
	 * Removed Dependencies won't be suppressed and can interrupt with this Interactor.
	 * Null not allowed.
	 * 
	 * @param InteractionDependency Other Interactor to be removed from List of Dependencies.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void RemoveInteractionDependency(const TScriptInterface<IActorInteractorInterface>& InteractionDependency);
	virtual void RemoveInteractionDependency_Implementation(const TScriptInterface<IActorInteractorInterface>& InteractionDependency) = 0;

	/**
	 * Returns all Interaction Dependencies.
	 * Those Interactors will be suppressed until this one is not interacting anymore.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	TArray<TScriptInterface<IActorInteractorInterface>> GetInteractionDependencies() const;
	virtual TArray<TScriptInterface<IActorInteractorInterface>> GetInteractionDependencies_Implementation() const = 0;

	/**
	 * Tries to process all dependencies according to current State.
	 * For more information, visit GitHub Wiki: https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Process-Interactor-Dependencies
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void ProcessDependencies();
	virtual void ProcessDependencies_Implementation() = 0;

	

	/**
	 * Optimized request for Interactor.
	 * Can be overridden in C++ for specific class needs.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	bool CanInteract() const;
	virtual bool CanInteract_Implementation() const = 0;

	

	/**
	 * Returns Interactor Response Channel.
	 * This is the channel Interactor is interacting with.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	ECollisionChannel GetResponseChannel() const;
	virtual ECollisionChannel GetResponseChannel_Implementation() const = 0;

	/**
	 * Sets Collision Channel as Response Channel for this Interactor.
	 * Tip: Use custom Collision Channel for Interaction types.
	 * * Interaction Overlap
	 * * Interaction Trace
	 * * Interaction Hover
	 * * etc.
	 * 
	 * @param NewResponseChannel Channel to be used as Response Channel
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void SetResponseChannel(const ECollisionChannel NewResponseChannel);
	virtual void SetResponseChannel_Implementation(const ECollisionChannel NewResponseChannel) = 0;

	

	/**
	 * Returns Interactor State.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	EInteractorStateV2 GetState() const;
	virtual EInteractorStateV2 GetState_Implementation() const = 0;

	/**
	 * Tries to set Interactor State.
	 * State machine apply!
	 * For more information, visit GitHub Wiki: https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactor-Component-Validations#state-machine
	 * 
	 * @param NewState Interactor State to be used as Interactor State
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void SetState(const EInteractorStateV2 NewState);
	virtual void SetState_Implementation(const EInteractorStateV2 NewState) = 0;

	/**
	 * Returns Default Interactor State.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	EInteractorStateV2 GetDefaultState() const;
	virtual EInteractorStateV2 GetDefaultState_Implementation() const = 0;

	/**
	 * Tries to set Default Interactor State.
	 * 
	 * @param NewState Value which will be set as new Default Interactor State.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void SetDefaultState(const EInteractorStateV2 NewState);
	virtual void SetDefaultState_Implementation(const EInteractorStateV2 NewState) = 0;

	

	/**
	 * Returns whether Interactor auto activates or not.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	bool DoesAutoActivate() const;
	virtual bool DoesAutoActivate_Implementation() const = 0;

	

	/**
	 * Sets specified or null Interactable to be Active Interactable.
	 * 
	 * @param NewInteractable Interactable Interface to be used as Active Interactable
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void SetActiveInteractable(const TScriptInterface<IActorInteractableInterface>& NewInteractable);
	virtual void SetActiveInteractable_Implementation(const TScriptInterface<IActorInteractableInterface>& NewInteractable) = 0;

	/**
	 * Returns Active Interactable if there is any. Otherwise null.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	TScriptInterface<IActorInteractableInterface> GetActiveInteractable() const;
	virtual TScriptInterface<IActorInteractableInterface> GetActiveInteractable_Implementation() const = 0;

	

	/**
	 * Development Only.
	 * Toggles debug On/Off.
	 * Does not affect Editor Debug!
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, CallInEditor, Category="Mountea|Interaction|Interactor")
	void ToggleDebug();
	virtual void ToggleDebug_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	FDebugSettings GetDebugSettings() const;
	virtual FDebugSettings GetDebugSettings_Implementation() const = 0;

	
	/**
	 * Retrieves the interactor's gameplay tag.
	 * 
	 * @return The gameplay tag associated with this interactor.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	FGameplayTag GetInteractorTag() const;
	virtual FGameplayTag GetInteractorTag_Implementation() const = 0;

	/**
	 * Sets the interactor's gameplay tag.
	 * If tags are same then nothing will be updated.
	 * 
	 * @param NewInteractorTag The new gameplay tag to associate with this interactor.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void SetInteractorTag(const FGameplayTag& NewInteractorTag);
	virtual void SetInteractorTag_Implementation(const FGameplayTag& NewInteractorTag) = 0;
	

	/**
	 * Event bound to OnComponentActivated event.
	 * Once OnComponentActivated is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param Component		Whether this Interactor is Auto Activated or not
	 * @param bReset				Whether the component should be reset
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void OnInteractorComponentActivated(UActorComponent* Component, bool bReset);
	virtual void OnInteractorComponentActivated_Implementation(UActorComponent* Component, bool bReset) = 0;

	/**
	 * Gets the owning actor of this Interactor.
	 * This function returns the actor that owns or Interactor or the Interactor itself if it's an Actor.
	 *
	 * @return The actor owning this Interactor.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;
	
	/**
	 * Helper function to provide debug and useful information.
	 * This function returns a string representation of the interactor, useful for logging and debugging.
	 *
	 * @return A string containing debug and useful information about the interactor.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	FString ToString() const;
	virtual FString ToString_Implementation() const = 0;

	/**
	 * Returns the current safety tracing setup.
	 *
	 * @return FSafetyTracingSetup containing the current safety tracing mode, start location, and start socket name.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	FSafetyTracingSetup GetSafetyTracingSetup() const;
	virtual FSafetyTracingSetup GetSafetyTracingSetup_Implementation() const = 0;

	/**
	 * Sets a new safety tracing setup.
	 *
	 * @param NewSafetyTracingSetup The new setup to be used, containing the safety tracing mode, start location, and start socket name.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void SetSafetyTracingSetup(const FSafetyTracingSetup& NewSafetyTracingSetup);
	virtual void SetSafetyTracingSetup_Implementation(const FSafetyTracingSetup& NewSafetyTracingSetup) = 0;
	
	/**
	 * Performs a safety trace to check for obstacles between the interactor and the specified interactable actor.
	 * This function is used to ensure that there are no obstructions that would prevent interaction with the specified actor.
	 *
	 * @param InteractableActor The actor to perform the safety trace against.
	 * @return True if the trace is successful and there are no obstructions, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	bool PerformSafetyTrace(const AActor* InteractableActor);
	virtual bool PerformSafetyTrace_Implementation(const AActor* InteractableActor) = 0;

	/**
	 * Checks if this Interactor has an interactable.
	 * This function is used to determine if this interactor has any interactable at given time.
	 *
	 * @return True if an interactable is present, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	bool HasInteractable() const;
	virtual bool HasInteractable_Implementation() const = 0;

	/**
	 * Finds default values from Developer settings and tries to set them for this component.
	 * Will override current settings!
	 * Will set those values only if not null.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor", meta=(DisplayName="SetDefaults"))
	void SetDefaults();
	virtual void SetDefaults_Implementation()  = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor")
	void ConsumeInput(UInputAction* ConsumedInput);
	virtual void ConsumeInput_Implementation(UInputAction* ConsumedInput) = 0;
	
	virtual FInteractableSelected& GetOnInteractableSelectedHandle() = 0;
	virtual FInteractableFound& GetOnInteractableFoundHandle() = 0;
	virtual FInteractableLost& GetOnInteractableLostHandle() = 0;
	virtual FInteractionKeyPressed& GetOnInteractionKeyPressedHandle() = 0;
	virtual FInteractionKeyReleased& GetOnInteractionKeyReleasedHandle() = 0;
	virtual FInteractorTagChanged& GetOnInteractorTagChangedHandle() = 0;
	virtual FStateChanged& GetOnStateChangedHandle() = 0;

	virtual FInputActionConsumed& GetInputActionConsumedHandle() = 0;
};