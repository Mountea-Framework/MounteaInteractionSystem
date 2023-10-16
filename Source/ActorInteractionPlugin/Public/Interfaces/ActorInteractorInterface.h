// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InputCoreTypes.h"
#include "Engine/EngineTypes.h"

#include "ActorInteractorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UActorInteractorInterface : public UInterface
{
	GENERATED_BODY()
};

enum class EInteractorStateV2 : uint8;
class IActorInteractableInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableSelected, const TScriptInterface<IActorInteractableInterface>&, SelectedInteractable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableFound, const TScriptInterface<IActorInteractableInterface>&, FoundInteractable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableLost, const TScriptInterface<IActorInteractableInterface>&, LostInteractable);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionKeyPressed, const float&, TimeKeyPressed, FKey, PressedKey);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionKeyReleased, const float&, TimeKeyReleased, FKey, ReleasedKey);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIgnoredActorAdded, const AActor*, AddedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIgnoredActorRemoved, const AActor*, RemovedActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStateChanged, const EInteractorStateV2&, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionChanged, const TEnumAsByte<ECollisionChannel>&, NewCollisionChannel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAutoActivateChanged, const bool, NewAutoActivate);

/**
 * 
 */
class ACTORINTERACTIONPLUGIN_API IActorInteractorInterface
{
	GENERATED_BODY()

#pragma region Events

public:
	
	/**
	 * Event bound to OnInteractableSelected event.
	 * Once OnInteractableSelected is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param SelectedInteractable Interactable Component which is being interacted with
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnInteractableSelectedEvent(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable);

	/**
	 * Event bound to OnInteractableFound event.
	 * Once OnInteractableFound is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param FoundInteractable Interactable Component which is found
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnInteractableFoundEvent(const TScriptInterface<IActorInteractableInterface>& FoundInteractable);
	
	/**
	 * Event bound to OnInteractableLost event.
	 * Once OnInteractableLost is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param LostInteractable Interactable Component which is lost
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnInteractableLostEvent(const TScriptInterface<IActorInteractableInterface>& LostInteractable);

	/**
	 * Event bound to OnInteractionKeyPressed event.
	 * Once OnInteractionKeyPressed is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param TimeKeyPressed Time Key was pressed
	 * @param PressedKey Key which was pressed
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void OnInteractionKeyPressedEvent(const float& TimeKeyPressed, FKey PressedKey);
	void OnInteractionKeyPressedEvent_Implementation(const float& TimeKeyPressed, const FKey& PressedKey)
	{
		StartInteraction(TimeKeyPressed, PressedKey);
	};

	/**
	 * Event bound to OnInteractionKeyReleased event.
	 * Once OnInteractionKeyReleased is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param TimeKeyReleased Time Key was released
	 * @param ReleasedKey Key which was released
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void OnInteractionKeyReleasedEvent(const float& TimeKeyReleased, FKey ReleasedKey);
	void OnInteractionKeyReleasedEvent_Implementation(const float& TimeKeyReleased, const FKey& ReleasedKey)
	{
		StopInteraction(TimeKeyReleased, ReleasedKey);
	};
	
	/**
	 * Event bound to OnStateChanged event.
	 * Once OnStateChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewState New State if this Interactor
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnInteractorStateChanged(const EInteractorStateV2& NewState);

	/**
	 * Event bound to OnCollisionChanged event.
	 * Once OnCollisionChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewCollisionChannel New Collision Channel set as Response Channel
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnInteractorCollisionChanged(const TEnumAsByte<ECollisionChannel>& NewCollisionChannel);

	/**
	 * Event bound to OnAutoActivateChanged event.
	 * Once OnAutoActivateChanged is called this event is, too.
	 * Be sure to call Parent event to access all C++ implementation!
	 * 
	 * @param NewAutoActivate Whether this Interactor is Auto Activated or not
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnInteractorAutoActivateChanged(const bool NewAutoActivate);

#pragma endregion

#pragma region Validation

public:

	/**
	 * Returns whether this Interactor can interact or not.
	 * Parent Calls 'CanInteract' function which is implemented in C++.
	 * For more information, visit GitHub Wiki: https://github.com/Mountea-Framework/ActorInteractionPlugin/wiki/Can-Interactor-Interact 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Interaction", meta=(DisplayName = "Can Interact"))
	bool CanInteractEvent() const;

protected:
	
	bool CanInteractEvent_Implementation() const
	{
		return CanInteract();
	}

#pragma endregion
	
public:

	virtual bool IsValidInteractor() const = 0;
		
	virtual void StartInteraction(const float StartTime, FKey InputKey) = 0;
	virtual void StopInteraction(const float StartTime, FKey InputKey) = 0;
	
	virtual bool ActivateInteractor(FString& ErrorMessage) = 0;
	virtual bool WakeUpInteractor(FString& ErrorMessage) = 0;
	virtual bool SuppressInteractor(FString& ErrorMessage) = 0;
	virtual void DeactivateInteractor() = 0;

	virtual void InteractableFound(const TScriptInterface<IActorInteractableInterface>& FoundInteractable) = 0;
	virtual void InteractableLost(const TScriptInterface<IActorInteractableInterface>& LostInteractable) = 0;
	virtual void InteractableSelected(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable) = 0;

	virtual void AddIgnoredActor(AActor* IgnoredActor) = 0;
	virtual void AddIgnoredActors(const TArray<AActor*> IgnoredActors) = 0;
	virtual void RemoveIgnoredActor(AActor* UnignoredActor) = 0;
	virtual void RemoveIgnoredActors(const TArray<AActor*> UnignoredActors) = 0;
	virtual TArray<AActor*> GetIgnoredActors() const = 0;
		
	virtual void AddInteractionDependency(const TScriptInterface<IActorInteractorInterface> InteractionDependency) = 0;
	virtual void RemoveInteractionDependency(const TScriptInterface<IActorInteractorInterface> InteractionDependency) = 0;
	virtual TArray<TScriptInterface<IActorInteractorInterface>> GetInteractionDependencies() const = 0;
	virtual void ProcessDependencies() = 0;

	virtual void EvaluateInteractable(const TScriptInterface<IActorInteractableInterface>& FoundInteractable) = 0;

	virtual bool CanInteract() const = 0;

	virtual ECollisionChannel GetResponseChannel() const = 0;
	virtual void SetResponseChannel(const ECollisionChannel NewResponseChannel) = 0;

	virtual EInteractorStateV2 GetState() const = 0;
	virtual void SetState(const EInteractorStateV2 NewState) = 0;
	virtual EInteractorStateV2 GetDefaultState() const = 0;
	virtual void SetDefaultState(const EInteractorStateV2 NewState) = 0;

	virtual bool DoesAutoActivate() const = 0;

	virtual TScriptInterface<IActorInteractableInterface> GetActiveInteractable() const = 0;
	virtual void SetActiveInteractable(const TScriptInterface<IActorInteractableInterface> NewInteractable) = 0;

	virtual void ToggleDebug() = 0;

	virtual FInteractableSelected& GetOnInteractableSelectedHandle() = 0;
	virtual FInteractableFound& GetOnInteractableFoundHandle() = 0;
	virtual FInteractableLost& GetOnInteractableLostHandle() = 0;
	virtual FInteractionKeyPressed& OnInteractionKeyPressedHandle() = 0;
	virtual FInteractionKeyReleased& OnInteractionKeyReleasedHandle() = 0;
};