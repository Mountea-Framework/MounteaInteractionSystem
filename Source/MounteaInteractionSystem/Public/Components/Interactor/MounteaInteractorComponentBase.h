// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "Interfaces/MounteaInteractorInterface.h"
#include "MounteaInteractorComponentBase.generated.h"

class UInputAction;
class UInputMappingContext;
struct FDebugSettings;

/**
 * v5.0 Actor Interactor Base Component
 *
 * Two-Tier Focus Model:
 *   Tier 1: DetectedSet → RefreshFocus() selects one FocusedActor (highest weight, tie-broken by distance).
 *   Tier 2: ActionList contains all IMounteaInteractableInterface components on FocusedActor, sorted by weight.
 *           Player cycles through ActionList with CycleAction(). StartInteraction() locks the highlighted action.
 *
 * Networking: State changes are server-authoritative via Server RPCs.
 * WorldSubsystem: Assigns/releases attention widgets as FocusedActor changes.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactor-Component-Base
 */
UCLASS(Abstract, ClassGroup=(Mountea), Blueprintable, BlueprintType,
	hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Rendering),
	meta=(BlueprintSpawnableComponent, DisplayName="Interactor Component", Keywords="Base, Default, Interactor"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractorComponentBase : public UActorComponent, public IMounteaInteractorInterface
{
	GENERATED_BODY()

	// Automation tests exercise the locked-interaction teardown paths (Phase 2) without driving
	// the full RefreshFocus/ActionList detection pipeline. This grants that one test helper
	// direct access to bSelectionLocked/LockedAction instead of adding test-only public API.
	friend struct FMounteaInteractionTestAccess;

public:

	UMounteaInteractorComponentBase();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region IMounteaInteractorInterface_Impl

public:

	virtual AActor* GetFocusedActor_Implementation() const override;
	virtual TArray<FInteractionActionCandidate> GetActionList_Implementation() const override;
	virtual int32 GetHighlightedActionIndex_Implementation() const override;
	virtual TScriptInterface<IMounteaInteractableInterface> GetLockedAction_Implementation() const override;
	virtual bool IsSelectionLocked_Implementation() const override;
	virtual void CycleAction_Implementation(const int32 Direction) override;

	virtual void StartInteraction_Implementation(const float StartTime) override;
	virtual void StopInteraction_Implementation(const float StopTime) override;
	virtual void ForceReleaseInteraction_Implementation() override;
	virtual bool CanInteract_Implementation() const override;
	virtual bool IsValidInteractor_Implementation() const override;
	virtual bool HasInteractable_Implementation() const override;

	virtual bool ActivateInteractor_Implementation(FString& ErrorMessage) override;
	virtual bool EnableInteractor_Implementation(FString& ErrorMessage) override;
	virtual bool SuppressInteractor_Implementation(FString& ErrorMessage) override;
	virtual void DeactivateInteractor_Implementation() override;
	virtual EInteractorStateV3 GetState_Implementation() const override;
	virtual void SetState_Implementation(const EInteractorStateV3 NewState) override;
	virtual EInteractorStateV3 GetDefaultState_Implementation() const override;
	virtual void SetDefaultState_Implementation(const EInteractorStateV3 NewState) override;

	virtual ECollisionChannel GetResponseChannel_Implementation() const override;
	virtual void SetResponseChannel_Implementation(const ECollisionChannel NewResponseChannel) override;
	virtual FGameplayTag GetInteractorTag_Implementation() const override;
	virtual void SetInteractorTag_Implementation(const FGameplayTag& NewInteractorTag) override;
	virtual bool DoesAutoActivate_Implementation() const override;
	virtual AActor* GetOwningActor_Implementation() const override;
	virtual FString ToString_Implementation() const override;
	virtual void SetDefaults_Implementation() override;

	virtual FSafetyTracingSetup GetSafetyTracingSetup_Implementation() const override;
	virtual void SetSafetyTracingSetup_Implementation(const FSafetyTracingSetup& NewSafetyTracingSetup) override;
	virtual bool PerformSafetyTrace_Implementation(const AActor* InteractableActor) override;

	virtual void AddIgnoredActor_Implementation(AActor* IgnoredActor) override;
	virtual void AddIgnoredActors_Implementation(const TArray<AActor*>& IgnoredActors) override;
	virtual void RemoveIgnoredActor_Implementation(AActor* UnignoredActor) override;
	virtual void RemoveIgnoredActors_Implementation(const TArray<AActor*>& UnignoredActors) override;
	virtual TArray<AActor*> GetIgnoredActors_Implementation() const override;

	virtual void AddInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractorInterface>& InteractionDependency) override;
	virtual void RemoveInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractorInterface>& InteractionDependency) override;
	virtual TArray<TScriptInterface<IMounteaInteractorInterface>> GetInteractionDependencies_Implementation() const override;
	virtual void ProcessDependencies_Implementation() override;

	virtual void ToggleDebug_Implementation() override;
	virtual FDebugSettings GetDebugSettings_Implementation() const override;
	virtual void OnInteractorComponentActivated_Implementation(UActorComponent* Component, bool bReset) override;
	virtual void ConsumeInput_Implementation(UInputAction* ConsumedInput) override;

#pragma endregion

#pragma region DelegateAccessors

public:

	virtual FStateChanged&             GetOnStateChangedHandle()            override { return OnStateChanged; }
	virtual FCollisionChanged&         GetOnCollisionChangedHandle()        override { return OnCollisionChanged; }
	virtual FInteractorTagChanged&     GetOnInteractorTagChangedHandle()    override { return OnInteractorTagChanged; }
	virtual FFocusedActorChanged&      GetOnFocusedActorChangedHandle()     override { return OnFocusedActorChanged; }
	virtual FActionListChanged&        GetOnActionListChangedHandle()       override { return OnActionListChanged; }
	virtual FHighlightedActionChanged& GetOnHighlightedActionChangedHandle() override { return OnHighlightedActionChanged; }
	virtual FInteractionKeyPressed&    GetOnInteractionKeyPressedHandle()   override { return OnInteractionKeyPressed; }
	virtual FInteractionKeyReleased&   GetOnInteractionKeyReleasedHandle()  override { return OnInteractionKeyReleased; }
	virtual FIgnoredActorAdded&        GetOnIgnoredActorAddedHandle()       override { return OnIgnoredActorAdded; }
	virtual FIgnoredActorRemoved&      GetOnIgnoredActorRemovedHandle()     override { return OnIgnoredActorRemoved; }

#pragma endregion

#pragma region FocusSystem

protected:

	/**
	 * Rebuilds FocusedActor and ActionList from DetectedSet.
	 * Called whenever DetectedSet changes (overlap begin/end, trace refresh).
	 * On focus change: updates WorldSubsystem attention widgets.
	 */
	virtual void RefreshFocus();

	/**
	 * Adds an interactable to the detected set and calls RefreshFocus().
	 * Subclasses (Overlap, Trace, Mouse) call this when they discover an interactable.
	 */
	virtual void AddDetected(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/**
	 * Removes an interactable from the detected set and calls RefreshFocus().
	 */
	virtual void RemoveDetected(const TScriptInterface<IMounteaInteractableInterface>& Interactable);

	/** Clears all detected interactables and resets focus. */
	virtual void ClearDetected();

#pragma endregion

#pragma region Networking

protected:

	UFUNCTION(Server, Reliable)
	void SetState_Server(const EInteractorStateV3 NewState);

	UFUNCTION(Server, Reliable)
	void StartInteraction_Server(const float StartTime);

	UFUNCTION(Server, Reliable)
	void StopInteraction_Server(const float StopTime);

	UFUNCTION(Server, Unreliable)
	void SetResponseChannel_Server(const ECollisionChannel NewResponseCollision);

	UFUNCTION(Server, Unreliable)
	void SetDefaultState_Server(const EInteractorStateV3 NewState);

	UFUNCTION(Server, Unreliable)
	void SetInteractorTag_Server(const FGameplayTag& NewInteractorTag);

	UFUNCTION(Server, Unreliable)
	void AddIgnoredActor_Server(AActor* IgnoredActor);

	UFUNCTION(Server, Unreliable)
	void RemoveIgnoredActor_Server(AActor* IgnoredActor);

	UFUNCTION(Server, Unreliable)
	void SetSafetyTracingSetup_Server(const FSafetyTracingSetup& NewSafetyTracingSetup);

	UFUNCTION()
	void OnRep_InteractorState();

	/** Broadcasts the state-changed delegates. Called by ProcessStateChanged() — the base
	 *  implementation of the hook subclasses override to bind/unbind detection on transition. */
	void BroadcastStateChanged();

	virtual void ProcessStateChanged();

#pragma endregion

#pragma region Delegates

public:

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FStateChanged OnStateChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FCollisionChanged OnCollisionChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FAutoActivateChanged OnAutoActivateChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FInteractionKeyPressed OnInteractionKeyPressed;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FInteractionKeyReleased OnInteractionKeyReleased;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FIgnoredActorAdded OnIgnoredActorAdded;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FIgnoredActorRemoved OnIgnoredActorRemoved;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FInteractorTagChanged OnInteractorTagChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FFocusedActorChanged OnFocusedActorChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FActionListChanged OnActionListChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea|Interaction|Interactor",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FHighlightedActionChanged OnHighlightedActionChanged;

#pragma endregion

#pragma region Properties

protected:

	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required")
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_Camera;

	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required")
	EInteractorStateV3 DefaultInteractorState = EInteractorStateV3::EIS_Scanning;

	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Optional")
	FGameplayTag InteractorTag;

	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required")
	FSafetyTracingSetup SafetyTraceSetup;

	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Optional")
	TArray<TObjectPtr<AActor>> ListOfIgnoredActors;

	UPROPERTY(EditAnywhere, Category="MounteaInteraction|Debug")
	FDebugSettings DebugSettings;

	UPROPERTY(ReplicatedUsing=OnRep_InteractorState, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	EInteractorStateV3 InteractorState = EInteractorStateV3::EIS_Disabled;

	UPROPERTY(Replicated, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	TArray<TScriptInterface<IMounteaInteractorInterface>> InteractionDependencies;

	/** All currently detected interactable components (populated by subclasses).
	 *  NOTE: TArray, not TSet — UHT does not support TScriptInterface as a TSet key.
	 *  AddDetected() uses AddUnique() to preserve set-like (no duplicates) semantics. */
	UPROPERTY()
	TArray<TScriptInterface<IMounteaInteractableInterface>> DetectedSet;

	/** The actor selected by Tier 1 focus (highest weight component). */
	UPROPERTY()
	TWeakObjectPtr<AActor> FocusedActor;

	/** Interactable components on FocusedActor, sorted by weight descending (Tier 2 action list). */
	UPROPERTY()
	TArray<FInteractionActionCandidate> ActionList;

	/** Currently highlighted action index within ActionList. */
	UPROPERTY()
	int32 HighlightedActionIndex = 0;

	/** True while StartInteraction has been called and not yet completed or stopped. */
	UPROPERTY()
	bool bSelectionLocked = false;

	/** The interactable currently being interacted with (null if not interacting). */
	UPROPERTY()
	TScriptInterface<IMounteaInteractableInterface> LockedAction;

#pragma endregion

#pragma region Editor

#if WITH_EDITOR
protected:

	DECLARE_EVENT(UMounteaInteractorComponentBase, FRequestInteractorEditorDefaults)
	FRequestInteractorEditorDefaults RequestEditorDefaults;

	virtual void ResetDefaults();
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;

public:

	UFUNCTION(BlueprintCallable, CallInEditor, Category="MounteaInteraction")
	void SetDefaultValues();

#endif

#pragma endregion
};
