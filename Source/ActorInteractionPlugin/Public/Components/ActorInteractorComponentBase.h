// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Helpers/InteractionHelpers.h"
#include "Interfaces/ActorInteractorInterface.h"
#include "ActorInteractorComponentBase.generated.h"

struct FDebugSettings;
class UInputMappingContext;

/**
 * Actor Interactor Base Component
 *
 * Implements ActorInteractorInterface.
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Actor-Interactor-Component-Base
 */
UCLASS(Abstract, ClassGroup=(Mountea), Blueprintable, BlueprintType, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Rendering), meta=(BlueprintSpawnableComponent, DisplayName = "Interactor Component"))
class ACTORINTERACTIONPLUGIN_API UActorInteractorComponentBase : public UActorComponent, public IActorInteractorInterface
{
	GENERATED_BODY()

public:

	UActorInteractorComponentBase();

protected:
	
	virtual void BeginPlay() override;

#pragma region Handles

public:

	virtual FInteractableSelected&		GetOnInteractableSelectedHandle() override
	{ return OnInteractableUpdated; };
	virtual FInteractableFound&			GetOnInteractableFoundHandle() override
	{ return OnInteractableFound; };
	virtual FInteractableLost&				GetOnInteractableLostHandle() override
	{ return OnInteractableLost; };
	virtual FInteractionKeyPressed&		GetOnInteractionKeyPressedHandle() override
	{ return OnInteractionKeyPressed; };
	virtual FInteractionKeyReleased&	GetOnInteractionKeyReleasedHandle() override
	{ return OnInteractionKeyReleased; };
	virtual FInteractorTagChanged&		GetOnInteractorTagChangedHandle() override
	{ return OnInteractorTagChanged; };

#pragma endregion
	
protected:

#pragma region NativeFunctions

	
	virtual void InteractableSelected_Implementation(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable) override;
	
	virtual void InteractableFound_Implementation(const TScriptInterface<IActorInteractableInterface>& FoundInteractable) override;
	
	virtual void InteractableLost_Implementation(const TScriptInterface<IActorInteractableInterface>& LostInteractable) override;

#pragma endregion 

public:

	virtual bool IsValidInteractor_Implementation() const override;
	virtual void EvaluateInteractable_Implementation(const TScriptInterface<IActorInteractableInterface>& FoundInteractable) override;
	virtual void StartInteraction_Implementation(const float StartTime) override;
	virtual void StopInteraction_Implementation(const float StopTime) override;
	virtual bool ActivateInteractor_Implementation(FString& ErrorMessage) override;
	virtual bool EnableInteractor_Implementation(FString& ErrorMessage) override;
	virtual bool SuppressInteractor_Implementation(FString& ErrorMessage) override;
	virtual void DeactivateInteractor_Implementation() override;
	virtual void AddIgnoredActor_Implementation(AActor* IgnoredActor) override;
	virtual void AddIgnoredActors_Implementation(const TArray<AActor*>& IgnoredActors) override;
	virtual void RemoveIgnoredActor_Implementation(AActor* UnignoredActor) override;
	virtual void RemoveIgnoredActors_Implementation(const TArray<AActor*>& UnignoredActors) override;
	virtual TArray<AActor*> GetIgnoredActors_Implementation() const override;
	virtual void AddInteractionDependency_Implementation(const TScriptInterface<IActorInteractorInterface>& InteractionDependency) override;
	virtual void RemoveInteractionDependency_Implementation(const TScriptInterface<IActorInteractorInterface>& InteractionDependency) override;
	virtual TArray<TScriptInterface<IActorInteractorInterface>> GetInteractionDependencies_Implementation() const override;
	virtual void ProcessDependencies_Implementation() override;
	virtual bool CanInteract_Implementation() const override;
	virtual ECollisionChannel GetResponseChannel_Implementation() const override;
	virtual void SetResponseChannel_Implementation(const ECollisionChannel NewResponseChannel) override;
	virtual EInteractorStateV2 GetState_Implementation() const override;
	virtual void SetState_Implementation(const EInteractorStateV2 NewState) override;
	virtual EInteractorStateV2 GetDefaultState_Implementation() const override;
	virtual void SetDefaultState_Implementation(const EInteractorStateV2 NewState) override;
	virtual bool DoesAutoActivate_Implementation() const override;
	virtual void SetActiveInteractable_Implementation(const TScriptInterface<IActorInteractableInterface>& NewInteractable) override;
	virtual TScriptInterface<IActorInteractableInterface> GetActiveInteractable_Implementation() const override;
	virtual void ToggleDebug_Implementation() override;
	virtual FGameplayTag GetInteractorTag_Implementation() const override;
	virtual void SetInteractorTag_Implementation(const FGameplayTag& NewInteractorTag) override;
	virtual void OnInteractorComponentActivated_Implementation(UActorComponent* Component, bool bReset) override;
	virtual FString ToString_Implementation() const override;
	virtual AActor* GetOwningActor_Implementation() const override;
	virtual bool PerformSafetyTrace_Implementation(AActor* InteractableActor) override;
	
protected:

	UFUNCTION(Server, Reliable)
	void SetState_Server(const EInteractorStateV2 NewState);
	
	UFUNCTION(Server, Reliable)
	void StartInteraction_Server(const float StartTime);
	UFUNCTION(Server, Reliable)
	void StopInteraction_Server(const float StopTime);

	UFUNCTION(Server, Unreliable)
	void AddIgnoredActor_Server(AActor* IgnoredActor);
	UFUNCTION(Server, Unreliable)
	void AddIgnoredActors_Server(const TArray<AActor*>& IgnoredActors);
	UFUNCTION(Server, Unreliable)
	void RemoveIgnoredActor_Server(AActor* IgnoredActor);
	UFUNCTION(Server, Unreliable)
	void RemoveIgnoredActors_Server(const TArray<AActor*>& IgnoredActors);
	
	UFUNCTION(Server, Unreliable)
	void AddInteractionDependency_Server(const TScriptInterface<IActorInteractorInterface>& InteractionDependency);
	UFUNCTION(Server, Unreliable)
	void RemoveInteractionDependency_Server(const TScriptInterface<IActorInteractorInterface>& InteractionDependency);

	UFUNCTION(Server, Unreliable)
	void ProcessDependencies_Server();

	UFUNCTION(Server, Unreliable)
	void SetResponseChannel_Server(const ECollisionChannel NewResponseCollision);
	
	UFUNCTION(Server, Unreliable)
	void SetDefaultState_Server(const EInteractorStateV2 NewState);

	UFUNCTION(Server, Reliable)
	void SetActiveInteractable_Server(const TScriptInterface<IActorInteractableInterface>& NewInteractable);

	UFUNCTION(Server, Reliable)
	void SetInteractorTag_Server(const FGameplayTag& NewInteractorTag);

	UFUNCTION(Client, Reliable)
	void SetActiveInteractable_Client(const TScriptInterface<IActorInteractableInterface>& NewInteractable);

	UFUNCTION()
	void OnRep_InteractorState();
	
	UFUNCTION()
	void OnRep_ActiveInteractable();

	virtual void ProcessStateChanged();
	virtual void ProcessStateChanged_Client();

	virtual void ProcessInteractableChanged();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	/**
	 * This event is called once this Interactor finds its best Interactable.
	 * Interactor can iterate over multiple Interactables from the same Actor.
	 *
	 * Example:
	 * Interactor overlaps with a chest of drawers. There are multiple interactables for each drawer and for items within them.
	 * However, drawers have higher weight, thus always suppress items, unless specified otherwise.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractableSelected			OnInteractableUpdated;
	
	/**
	 * This event is called once this Interactor finds any Interactable.
	 * This event might happen for multiple Interactables. Each one is compared and if fit it is fed to OnInteractableSelected.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractableFound				OnInteractableFound;
	
	/**
	 * This event is called one this Interactor loose its Active Interactable.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractableLost				OnInteractableLost;
	
	/**
	 * This event should be called once starting the Interaction Action is requested and valid Key is pressed.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionKeyPressed		OnInteractionKeyPressed;
	
	/**
	 * This event should be called once stopping the Interaction Action is requested and valid Key is released.
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionKeyReleased	OnInteractionKeyReleased;
	
	/**
	 * This event is called once SetState function sets new State.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FStateChanged					OnStateChanged;
	
	/**
	 * This event is called once SetState function sets new State.
	 * This Event is called on Clients only!
	 *
	 * Servers purpose of driving Client only data, like UI.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FStateChanged					OnStateChanged_Client;
	
	/**
	 * This event is called once SetResponseChannel set new Collision Channel.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionChanged				OnCollisionChanged;
	
	/**
	 * This event is called once ToggleAutoActivate sets new value.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FAutoActivateChanged		OnAutoActivateChanged;
	
	/**
	 * This event is called once Ignored Actor is successfully added to List of Ignored Actors. 
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FIgnoredActorAdded			OnIgnoredActorAdded;
	
	/**
	 * This event is called once Ignored Actor is successfully removed from List of Ignored Actors. 
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FIgnoredActorRemoved		OnIgnoredActorRemoved;
	
	/**
	 * This event is called once Interactor's GameplayTag has changed.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractorTagChanged		OnInteractorTagChanged;

protected:
	
	/**
	 * Gameplay Tag which helps further filter out Interaction.
	 * Requires match in Interactable's `Interactable Tags` container.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Optional", meta=(NoResetToDefault))
	FGameplayTag											InteractorTag;

	/**
	 * If active, debug can be drawn.
	 * You can disable Editor Warnings. Editor Errors cannot be disabled!
	 * Serves a general purpose as a flag.
	 * Does not affect Shipping builds by default C++ implementation.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Debug", meta=(ShowOnlyInnerProperties))
	FDebugSettings											DebugSettings;
	
	/**
	 * Response Collision Channel this Interactor is interacting against.
	 * Tip: Use custom Collision Channel for Interaction types.
	 * * Interaction Overlap
	 * * Interaction Trace
	 * * Interaction Hover
	 * * etc.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	TEnumAsByte<ECollisionChannel>				CollisionChannel;
	
	/**
	 * New and easier way to set Default State.
	 * This state will be propagated to Interactor State.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	EInteractorStateV2										DefaultInteractorState;

	
	/**
	 * Used for validation. If allowed a single LineTrace will be shot from Owner's location towards Overlapped Actor. `ValidationCollisionChannel` is used for this check.
	 * This is useful to prevent overlapping collision which is clipping through a wall, for instance.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault))
	uint8																									bUseSafetyTrace : 1;
	
	/**
	 * Additional collision channel used for validation Trace after initial overlap.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Required", meta=(NoResetToDefault), meta=(EditCondition="bUseSafetyTrace == true"))
	TEnumAsByte<ECollisionChannel>														ValidationCollisionChannel;
	
	/**
	 * A list of Actors that won't be taken in count when interacting.
	 * If left empty, only Owner Actor is ignored.
	 * If using multiple Actors (a gun, for instance), all those child/attached Actors should be ignored.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Optional", meta=(NoResetToDefault, DisplayThumbnail=false))
	TArray<TObjectPtr<AActor>>					ListOfIgnoredActors;

private:

	/**
	 * Current read-only State of this Interactor.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_InteractorState, VisibleAnywhere, Category="Interaction|Read Only", meta=(NoResetToDefault))
	EInteractorStateV2 InteractorState;
	
	// This is Interactable which is set as Active
	UPROPERTY(ReplicatedUsing=OnRep_ActiveInteractable, VisibleAnywhere, Category="Interaction|Read Only")
	TScriptInterface<IActorInteractableInterface> ActiveInteractable;
	
	// List of interactors suppressed by this one
	UPROPERTY(Replicated, VisibleAnywhere, Category="Interaction|Read Only")
	TArray<TScriptInterface<IActorInteractorInterface>> InteractionDependencies;

#pragma region Editor

#if WITH_EDITOR
	
protected:
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;

#endif
	
#pragma endregion 
};
