// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Helpers/InteractionHelpers.h"
#include "Interfaces/ActorInteractorInterface.h"
#include "ActorInteractorComponentBase.generated.h"

struct FDebugSettings;

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

	/**
	 * Function called by OnInteractableSelected.
	 * Calls OnInteractableSelectedEvent.
	 * @param SelectedInteractable Interactable which was selected as new Active Interactable
	 */
	UFUNCTION()
	virtual void InteractableSelected(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable) override;
	/**
	 * Function called by OnInteractableFound.
	 * Suppress all Dependencies.
	 * Calls OnInteractableFoundEvent.
	 * Class EvaluateInteractable with Found Interactable, resulting in possible call to InteractableSelected with this Found Interactable.
	 * @param FoundInteractable Interactable which was found, not necessarily will be set as Active!
	 */
	UFUNCTION()
	virtual void InteractableFound(const TScriptInterface<IActorInteractableInterface>& FoundInteractable) override;
	/**
	 * Function called by OnInteractableLost. If Lost Interactable is not Active Interactable, then nothing happens.
	 * Resets Active Interactable to null. Sets all Dependencies to same State as this Interactor has.
	 * Calls OnInteractableLostEvent.
	 * @param LostInteractable Interactable which was Lost.
	 */
	UFUNCTION()
	virtual void InteractableLost(const TScriptInterface<IActorInteractableInterface>& LostInteractable) override;

#pragma endregion 

public:

	/**
	 * Returns whether this Interactor is Valid for Interactions.
	 * Disabled/Suppressed Interactors are not valid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool IsValidInteractor() const override;
	
	/**
	 * Compares Interactables.
	 * For details of Evaulation visit GitGub Wiki: https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Interactor-Evaulation
	 * @param FoundInteractable Interactable which was found
	 */
	UFUNCTION()
	virtual void EvaluateInteractable(const TScriptInterface<IActorInteractableInterface>& FoundInteractable) override;

	/**
	 * Function to start interaction.
	 * Interaction will start only if CanInteract function evaluates true.
	 * @param StartTime Time Interaction has started. 
	 * @param InputKey Optional Key. Some Interactions might require this value to perform checks.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void StartInteraction(const float StartTime, FKey InputKey) override;
	/**
	 * Function to stop interaction.
	 * No conditions required.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void StopInteraction(const float StartTime, FKey InputKey) override;


	/**
	 * Tries to Active Interactor by setting Interactor state to Active.
	 * Returns whether it was successful or not.
	 * Either way, Error Message contains useful information what possibly went wrong.
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool ActivateInteractor(FString& ErrorMessage) override;
	/**
	 * Tries to Wake Up Interactor by setting Interactor state to Stand By.
	 * Returns whether it was successful or not.
	 * Either way, Error Message contains useful information what possibly went wrong.
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool WakeUpInteractor(FString& ErrorMessage) override;
	/**
	 * Tries to Suppress Interactor by setting Interactor state to Suppressed.
	 * Returns whether it was successful or not.
	 * Either way, Error Message contains useful information what possibly went wrong.
	 * @param ErrorMessage Short explanation.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool SuppressInteractor(FString& ErrorMessage) override;
	/**
	 * Deactivates Interactor.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void DeactivateInteractor() override;

	
	/**
	 * Tries to add Actor to Ignored Actors.
	 * Duplicates and null not allowed.
	 * @param IgnoredActor Actor to be ignored.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddIgnoredActor(AActor* IgnoredActor) override;
	/**
	 * Tries to add Actors to Ignored Actors.
	 * Duplicates and null not allowed.
	 * Calls AddIgnoredActor for each.
	 * @param IgnoredActors Array of Actors to be ignored.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddIgnoredActors(const TArray<AActor*> IgnoredActors) override;
	/**
	 * Tries to remove Actor from Ignored Actors.
	 * Null not allowed.
	 * @param UnignoredActor Actor to be un-ignored.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveIgnoredActor(AActor* UnignoredActor) override;
	/**
	 * Tries to remove Actors from Ignored Actors.
	 * Null not allowed.
	 * Calls RemoveIgnoredActor for each.
	 * @param UnignoredActors Array of Actors to be un-ignored.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RemoveIgnoredActors(const TArray<AActor*> UnignoredActors) override;
	/**
	 * Returns list of Ignored Actors.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TArray<AActor*> GetIgnoredActors() const override;


	/**
	 * Adds unique Interaction Dependency.
	 * Those Interactors will be suppressed until this one is not interacting anymore.
	 * Duplicates or null not allowed.
	 * @param InteractionDependency Other Interactor to be added to List of Dependencies.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void AddInteractionDependency(const TScriptInterface<IActorInteractorInterface> InteractionDependency) override;
	/**
	 * Removes unique Interaction Dependency.
	 * Removed Dependencies won't be suppressed and can interrupt with this Interactor.
	 * Null not allowed.
	 * @param InteractionDependency Other Interactor to be removed from List of Dependencies.
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
	 * Tries to process all dependencies according to current State.
	 * For more information, visit GitHub Wiki: https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Process-Interactor-Dependencies
	 */
	UFUNCTION(Category="Interaction")
	virtual void ProcessDependencies() override;

	/**
	 * Optimized request for Interactor.
	 * Can be overriden in C++ for specific class needs.
	 */
	UFUNCTION()
	virtual bool CanInteract() const override;

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
	 * @param NewResponseChannel Channel to be used as Response Channel
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
	 * For more information, visit GitHub Wiki: https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactor-Component-Validations#state-machine
	 * @param NewState Interactor State to be used as Interactor State
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetState(const EInteractorStateV2 NewState) override;
	/**
	 * Returns Default Interactor State.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,Category="Interaction")
	virtual EInteractorStateV2 GetDefaultState() const override;
	/**
	 * Tries to set Default Interactor State.
	 *
	 * @param NewState	Value which will be set as new Default Interactor State.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetDefaultState(const EInteractorStateV2 NewState) override;

	/**
	 * Returns whether Interactor auto activates or not.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,Category="Interaction")
	virtual bool DoesAutoActivate() const override;

	/**
	 * Sets specified or nul Interactable to be Active Interactable.
	 * @param NewInteractable Interactable Interface to be used as Active Interactable
	 */
	UFUNCTION(Category="Interaction")
	virtual void SetActiveInteractable(const TScriptInterface<IActorInteractableInterface> NewInteractable) override;
	/**
	 * Returns Active Interactable if there is any. Otherwise null.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual TScriptInterface<IActorInteractableInterface> GetActiveInteractable() const override;

	/**
	 * Development Only.
	 * Toggles debug On/Off.
	 * Does not affect Editor Debug!
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Interaction", meta=(DevelopmentOnly))
	virtual void ToggleDebug() override;

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
	 * You can disable Editor Warnings. Editor Errors cannot be disabled!
	 * Serves a general purpose as a flag.
	 * Does not affect Shipping builds by default C++ implementation.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Debug", meta=(ShowOnlyInnerProperties))
	struct FDebugSettings DebugSettings;
	
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
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	mutable EInteractorStateV2 DefaultInteractorState;

	/**
	 * A list of Actors that won't be taken in count when interacting.
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

#pragma region Editor

#if WITH_EDITOR
	
protected:

	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;

#endif
	
#pragma endregion 
};
