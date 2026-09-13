// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "Engine/EngineTypes.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "MounteaInteractorInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInteractorInterface : public UInterface
{
	GENERATED_BODY()
};

class IMounteaInteractableInterface;
class UInputAction;
struct FGameplayTag;

// --- State delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStateChanged,        const EInteractorStateV3&,           NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionChanged,    const TEnumAsByte<ECollisionChannel>&, NewChannel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAutoActivateChanged, const bool,                          NewAutoActivate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractorTagChanged, const FGameplayTag&,                NewTag);

// --- Focus / action cycling delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFocusedActorChanged, const AActor*,                                NewFocusedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionListChanged,   const TArray<FInteractionActionCandidate>&,   NewActionList);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightedActionChanged, const int32,                            NewIndex);

// --- Interaction delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionKeyPressed,  const float&, TimeKeyPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionKeyReleased, const float&, TimeKeyReleased);

// --- Ignored actors ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIgnoredActorAdded,   const AActor*, AddedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIgnoredActorRemoved, const AActor*, RemovedActor);

/**
 * v5.0 Interactor Interface.
 * Two-tier focus model: Tier 1 = focused actor selection by weight+distance,
 * Tier 2 = action cycling through all interactable components on the focused actor.
 */
class MOUNTEAINTERACTIONSYSTEM_API IMounteaInteractorInterface
{
	GENERATED_BODY()

protected:

#pragma region BlueprintImplementableEvents

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea Interaction|Interactor")
	void OnInteractorStateChangedEvent(const EInteractorStateV3& NewState);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea Interaction|Interactor")
	void OnFocusedActorChangedEvent(const AActor* NewFocusedActor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea Interaction|Interactor")
	void OnActionListChangedEvent(const TArray<FInteractionActionCandidate>& NewActionList);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea Interaction|Interactor")
	void OnHighlightedActionChangedEvent(const int32 NewIndex);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea Interaction|Interactor")
	void OnInteractionKeyPressedEvent(const float& TimeKeyPressed);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea Interaction|Interactor")
	void OnInteractionKeyReleasedEvent(const float& TimeKeyReleased);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea Interaction|Interactor")
	void OnInteractorCollisionChangedEvent(const TEnumAsByte<ECollisionChannel>& NewCollisionChannel);

#pragma endregion

public:

#pragma region FocusAPI

	/** Returns the actor currently focused by Tier 1 selection (highest weight + distance). Null if none in range. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor|Focus")
	AActor* GetFocusedActor() const;
	virtual AActor* GetFocusedActor_Implementation() const = 0;

	/** Returns the Tier 2 action list — all interactable components on the focused actor, sorted by weight descending. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor|Focus")
	TArray<FInteractionActionCandidate> GetActionList() const;
	virtual TArray<FInteractionActionCandidate> GetActionList_Implementation() const = 0;

	/** Returns the currently highlighted action index within the action list. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor|Focus")
	int32 GetHighlightedActionIndex() const;
	virtual int32 GetHighlightedActionIndex_Implementation() const = 0;

	/** Returns the locked action (currently being interacted with), or null if not interacting. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor|Focus")
	TScriptInterface<IMounteaInteractableInterface> GetLockedAction() const;
	virtual TScriptInterface<IMounteaInteractableInterface> GetLockedAction_Implementation() const = 0;

	/** True if an interaction is in progress and the action selection is locked. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor|Focus")
	bool IsSelectionLocked() const;
	virtual bool IsSelectionLocked_Implementation() const = 0;

	/**
	 * Advance the highlighted action index by Direction (+1 = next, -1 = prev).
	 * Wraps around. No-op while selection is locked or action list is empty.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor|Focus")
	void CycleAction(const int32 Direction);
	virtual void CycleAction_Implementation(const int32 Direction) = 0;

#pragma endregion

#pragma region Interaction

	/** Begin interaction with the currently highlighted action. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void StartInteraction(const float StartTime);
	virtual void StartInteraction_Implementation(const float StartTime) = 0;

	/** End interaction with the locked action. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void StopInteraction(const float StopTime);
	virtual void StopInteraction_Implementation(const float StopTime) = 0;

	/**
	 * Forcibly clears the current lock/interaction state without notifying the interactable.
	 * Called by an interactable's EndPlay when it is destroyed while occupied, so the interactor
	 * isn't left permanently locked out of starting any other interaction. Unlike StopInteraction,
	 * this never calls back into the interactable — the caller is typically mid-teardown itself.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void ForceReleaseInteraction();
	virtual void ForceReleaseInteraction_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	bool CanInteract() const;
	virtual bool CanInteract_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	bool IsValidInteractor() const;
	virtual bool IsValidInteractor_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	bool HasInteractable() const;
	virtual bool HasInteractable_Implementation() const = 0;

#pragma endregion

#pragma region StateMachine

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	bool ActivateInteractor(FString& ErrorMessage);
	virtual bool ActivateInteractor_Implementation(FString& ErrorMessage) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	bool EnableInteractor(FString& ErrorMessage);
	virtual bool EnableInteractor_Implementation(FString& ErrorMessage) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	bool SuppressInteractor(FString& ErrorMessage);
	virtual bool SuppressInteractor_Implementation(FString& ErrorMessage) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void DeactivateInteractor();
	virtual void DeactivateInteractor_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	EInteractorStateV3 GetState() const;
	virtual EInteractorStateV3 GetState_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void SetState(const EInteractorStateV3 NewState);
	virtual void SetState_Implementation(const EInteractorStateV3 NewState) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	EInteractorStateV3 GetDefaultState() const;
	virtual EInteractorStateV3 GetDefaultState_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void SetDefaultState(const EInteractorStateV3 NewState);
	virtual void SetDefaultState_Implementation(const EInteractorStateV3 NewState) = 0;

#pragma endregion

#pragma region Properties

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	ECollisionChannel GetResponseChannel() const;
	virtual ECollisionChannel GetResponseChannel_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void SetResponseChannel(const ECollisionChannel NewResponseChannel);
	virtual void SetResponseChannel_Implementation(const ECollisionChannel NewResponseChannel) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	FGameplayTag GetInteractorTag() const;
	virtual FGameplayTag GetInteractorTag_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void SetInteractorTag(const FGameplayTag& NewInteractorTag);
	virtual void SetInteractorTag_Implementation(const FGameplayTag& NewInteractorTag) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	bool DoesAutoActivate() const;
	virtual bool DoesAutoActivate_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	FString ToString() const;
	virtual FString ToString_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor", meta=(DisplayName="SetDefaults"))
	void SetDefaults();
	virtual void SetDefaults_Implementation() = 0;

#pragma endregion

#pragma region SafetyTrace

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	FSafetyTracingSetup GetSafetyTracingSetup() const;
	virtual FSafetyTracingSetup GetSafetyTracingSetup_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void SetSafetyTracingSetup(const FSafetyTracingSetup& NewSafetyTracingSetup);
	virtual void SetSafetyTracingSetup_Implementation(const FSafetyTracingSetup& NewSafetyTracingSetup) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	bool PerformSafetyTrace(const AActor* InteractableActor);
	virtual bool PerformSafetyTrace_Implementation(const AActor* InteractableActor) = 0;

#pragma endregion

#pragma region IgnoredActors

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void AddIgnoredActor(AActor* IgnoredActor);
	virtual void AddIgnoredActor_Implementation(AActor* IgnoredActor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void AddIgnoredActors(const TArray<AActor*>& IgnoredActors);
	virtual void AddIgnoredActors_Implementation(const TArray<AActor*>& IgnoredActors) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void RemoveIgnoredActor(AActor* UnignoredActor);
	virtual void RemoveIgnoredActor_Implementation(AActor* UnignoredActor) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void RemoveIgnoredActors(const TArray<AActor*>& UnignoredActors);
	virtual void RemoveIgnoredActors_Implementation(const TArray<AActor*>& UnignoredActors) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	TArray<AActor*> GetIgnoredActors() const;
	virtual TArray<AActor*> GetIgnoredActors_Implementation() const = 0;

#pragma endregion

#pragma region Dependencies

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void AddInteractionDependency(const TScriptInterface<IMounteaInteractorInterface>& InteractionDependency);
	virtual void AddInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractorInterface>& InteractionDependency) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void RemoveInteractionDependency(const TScriptInterface<IMounteaInteractorInterface>& InteractionDependency);
	virtual void RemoveInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractorInterface>& InteractionDependency) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	TArray<TScriptInterface<IMounteaInteractorInterface>> GetInteractionDependencies() const;
	virtual TArray<TScriptInterface<IMounteaInteractorInterface>> GetInteractionDependencies_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void ProcessDependencies();
	virtual void ProcessDependencies_Implementation() = 0;

#pragma endregion

#pragma region Debug

	UFUNCTION(BlueprintNativeEvent, CallInEditor, Category="Mountea Interaction|Interactor")
	void ToggleDebug();
	virtual void ToggleDebug_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	FDebugSettings GetDebugSettings() const;
	virtual FDebugSettings GetDebugSettings_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void OnInteractorComponentActivated(UActorComponent* Component, bool bReset);
	virtual void OnInteractorComponentActivated_Implementation(UActorComponent* Component, bool bReset) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Interaction|Interactor")
	void ConsumeInput(UInputAction* ConsumedInput);
	virtual void ConsumeInput_Implementation(UInputAction* ConsumedInput) = 0;

#pragma endregion

#pragma region DelegateAccessors

	virtual FStateChanged&              GetOnStateChangedHandle() = 0;
	virtual FCollisionChanged&          GetOnCollisionChangedHandle() = 0;
	virtual FInteractorTagChanged&      GetOnInteractorTagChangedHandle() = 0;
	virtual FFocusedActorChanged&       GetOnFocusedActorChangedHandle() = 0;
	virtual FActionListChanged&         GetOnActionListChangedHandle() = 0;
	virtual FHighlightedActionChanged&  GetOnHighlightedActionChangedHandle() = 0;
	virtual FInteractionKeyPressed&     GetOnInteractionKeyPressedHandle() = 0;
	virtual FInteractionKeyReleased&    GetOnInteractionKeyReleasedHandle() = 0;
	virtual FIgnoredActorAdded&         GetOnIgnoredActorAddedHandle() = 0;
	virtual FIgnoredActorRemoved&       GetOnIgnoredActorRemovedHandle() = 0;

#pragma endregion
};
