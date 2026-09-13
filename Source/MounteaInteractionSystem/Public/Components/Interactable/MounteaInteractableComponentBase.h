// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Engine/DataTable.h"
#include "Interfaces/MounteaInteractableInterface.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "MounteaInteractableComponentBase.generated.h"

class UInputMappingContext;
enum class ECommonInputType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetUpdated);

/**
 * v5.0 Actor Interactable Base Component
 *
 * Abstract base class for all interactable types.
 * Supports N concurrent interactors via FInteractorSlot array (replicated).
 * Progress timing uses InteractionStartServerTime (server world time) instead of FTimerHandles.
 * Action cycling is controlled by bStandaloneInteraction.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactable-Component-Base
 */
UCLASS(Abstract, ClassGroup=(Mountea), Blueprintable, BlueprintType,
	hideCategories=(Collision, AssetUserData, Cooking, Physics),
	ShowCategories=(Activation),
	meta=(BlueprintSpawnableComponent, DisplayName="Interactable Component", Keywords="Base, Default, Interactable"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableComponentBase : public UWidgetComponent, public IMounteaInteractableInterface
{
	GENERATED_BODY()

	// Automation tests need to set RequiredInteractorTags directly (no public setter exists) to
	// exercise the focus-time tag gate without depending on Blueprint/editor-only setup.
	friend struct FMounteaInteractionTestAccess;

public:

	UMounteaInteractableComponentBase();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitWidget() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentCreated() override;
	virtual void OnRegister() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region IMounteaInteractableInterface_Impl

public:

	virtual TArray<FInteractorSlot>    GetActiveInteractorSlots_Implementation()    const override;
	virtual int32                      GetMaxInteractors_Implementation()            const override;
	virtual FGameplayTagContainer      GetRequiredInteractorTags_Implementation()    const override;
	virtual FGameplayTagContainer      GetExcludedInteractorTags_Implementation()    const override;
	virtual bool                       IsSlotAvailable_Implementation()              const override;
	virtual bool                       HasInteractor_Implementation()                const override;
	virtual void                       RequestInteractorJoin_Implementation(const TScriptInterface<IMounteaInteractorInterface>& RequestingInteractor) override;
	virtual void                       RequestInteractorLeave_Implementation(const TScriptInterface<IMounteaInteractorInterface>& LeavingInteractor) override;

	virtual FInteractableWidgetRanges  GetWidgetRanges_Implementation()             const override;
	virtual void                       SetWidgetRanges_Implementation(const FInteractableWidgetRanges& NewRanges) override;

	virtual bool                       IsStandaloneInteraction_Implementation()      const override;
	virtual void                       SetStandaloneInteraction_Implementation(const bool bStandalone) override;

	virtual bool                       ActivateInteractable_Implementation(FString& ErrorMessage) override;
	virtual bool                       WakeUpInteractable_Implementation(FString& ErrorMessage) override;
	virtual bool                       CompleteInteractable_Implementation(FString& ErrorMessage) override;
	virtual void                       DeactivateInteractable_Implementation() override;
	virtual void                       PauseInteraction_Implementation(const float ExpirationTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;
	virtual bool                       CanInteract_Implementation()                  const override;
	virtual bool                       CanBeTriggered_Implementation()               const override;
	virtual bool                       IsInteracting_Implementation()                const override;
	virtual EInteractableStateV3       GetDefaultState_Implementation()              const override;
	virtual void                       SetDefaultState_Implementation(const EInteractableStateV3 NewState) override;
	virtual EInteractableStateV3       GetState_Implementation()                     const override;
	virtual void                       SetState_Implementation(const EInteractableStateV3 NewState) override;
	virtual bool                       TriggerCooldown_Implementation() override;

	virtual void InteractorFound_Implementation(const TScriptInterface<IMounteaInteractorInterface>& FoundInteractor) override;
	virtual void InteractorLost_Implementation(const TScriptInterface<IMounteaInteractorInterface>& LostInteractor) override;
	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;
	virtual void InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;
	virtual void InteractionCompleted_Implementation(const float& TimeCompleted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;
	virtual void InteractionCycleCompleted_Implementation(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;
	virtual void InteractionCanceled_Implementation() override;
	virtual void InteractionLifecycleCompleted_Implementation() override;
	virtual void InteractionCooldownCompleted_Implementation() override;

	virtual float GetInteractionProgress_Implementation() const override;
	virtual float GetInteractionPeriod_Implementation() const override;
	virtual void  SetInteractionPeriod_Implementation(const float NewPeriod) override;
	virtual int32 GetInteractableWeight_Implementation() const override;
	virtual void  SetInteractableWeight_Implementation(const int32 NewWeight) override;
	virtual AActor* GetInteractableOwner_Implementation() const override;
	virtual AActor* GetOwningActor_Implementation() const override;
	virtual ECollisionChannel GetCollisionChannel_Implementation() const override;
	virtual void SetCollisionChannel_Implementation(const TEnumAsByte<ECollisionChannel>& NewChannel) override;
	virtual EInteractableLifecycle GetLifecycleMode_Implementation() const override;
	virtual void SetLifecycleMode_Implementation(const EInteractableLifecycle& NewMode) override;
	virtual int32 GetLifecycleCount_Implementation() const override;
	virtual void  SetLifecycleCount_Implementation(const int32 NewLifecycleCount) override;
	virtual int32 GetRemainingLifecycleCount_Implementation() const override;
	virtual float GetCooldownPeriod_Implementation() const override;
	virtual void  SetCooldownPeriod_Implementation(const float NewCooldownPeriod) override;
	virtual ETimingComparison GetComparisonMethod_Implementation() const override;
	virtual void SetComparisonMethod_Implementation(const ETimingComparison Value) override;
	virtual FText GetInteractableName_Implementation() const override;
	virtual void  SetInteractableName_Implementation(const FText& NewName) override;
	virtual FDataTableRowHandle GetInteractableData_Implementation() const override;
	virtual void SetInteractableData_Implementation(FDataTableRowHandle NewData) override;

	virtual bool         DoesAutoSetup_Implementation() const override;
	virtual void         ToggleAutoSetup_Implementation(const ESetupType& NewValue) override;
	virtual void         FindAndAddCollisionShapes_Implementation() override;
	virtual void         FindAndAddHighlightableMeshes_Implementation() override;
	virtual void         SetDefaults_Implementation() override;

	virtual void StartHighlight_Implementation() override;
	virtual void StopHighlight_Implementation() override;
	virtual EHighlightType       GetHighlightType_Implementation() const override;
	virtual void                 SetHighlightType_Implementation(const EHighlightType NewHighlightType) override;
	virtual UMaterialInterface*  GetHighlightMaterial_Implementation() const override;
	virtual void                 SetHighlightMaterial_Implementation(UMaterialInterface* NewHighlightMaterial) override;

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
	virtual void BindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const override;
	virtual void UnbindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const override;
	virtual void BindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const override;
	virtual void UnbindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const override;

	virtual FGameplayTagContainer GetInteractableCompatibleTags_Implementation() const override;
	virtual void SetInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) override;
	virtual void AddInteractableCompatibleTag_Implementation(const FGameplayTag& Tag) override;
	virtual void AddInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) override;
	virtual void RemoveInteractableCompatibleTag_Implementation(const FGameplayTag& Tag) override;
	virtual void RemoveInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags) override;
	virtual void ClearInteractableCompatibleTags_Implementation() override;

	virtual TArray<TSoftClassPtr<UObject>> GetIgnoredClasses_Implementation() const override;
	virtual void SetIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& NewIgnoredClasses) override;
	virtual void AddIgnoredClass_Implementation(const TSoftClassPtr<UObject>& AddIgnoredClass) override;
	virtual void AddIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& AddIgnoredClasses) override;
	virtual void RemoveIgnoredClass_Implementation(const TSoftClassPtr<UObject>& RemoveIgnoredClass) override;
	virtual void RemoveIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& RemoveIgnoredClasses) override;

	virtual void AddInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency) override;
	virtual void RemoveInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency) override;
	virtual TArray<TScriptInterface<IMounteaInteractableInterface>> GetInteractionDependencies_Implementation() const override;
	virtual void ProcessDependencies_Implementation() override;
	virtual void InteractableDependencyStartedCallback_Implementation(const TScriptInterface<IMounteaInteractableInterface>& NewMaster) override;
	virtual void InteractableDependencyStoppedCallback_Implementation(const TScriptInterface<IMounteaInteractableInterface>& FormerMaster) override;

	virtual void ToggleWidgetVisibility_Implementation(const bool IsVisible) override;
	virtual void ToggleDebug_Implementation() override;
	virtual FDebugSettings GetDebugSettings_Implementation() const override;
	virtual FString ToString_Implementation() const override;
	virtual void OnInputDeviceChanged_Implementation(const ECommonInputType DeviceType, const FName& DeviceName) override;

#pragma endregion

#pragma region DelegateAccessors

public:

	virtual FInteractorFound&               GetOnInteractorFoundHandle()                   override { return OnInteractorFound; }
	virtual FInteractorLost&                GetOnInteractorLostHandle()                    override { return OnInteractorLost; }
	virtual FOnInteractorSlotsChanged&      GetOnInteractorSlotsChangedHandle()            override { return OnInteractorSlotsChanged; }
	virtual FOnInteractorRejected&          GetOnInteractorRejectedHandle()                override { return OnInteractorRejected; }
	virtual FInteractionStarted&            GetOnInteractionStartedHandle()                override { return OnInteractionStarted; }
	virtual FInteractionStopped&            GetOnInteractionStoppedHandle()                override { return OnInteractionStopped; }
	virtual FInteractionCompleted&          GetOnInteractionCompletedHandle()              override { return OnInteractionCompleted; }
	virtual FInteractionCycleCompleted&     GetOnInteractionCycleCompletedHandle()         override { return OnInteractionCycleCompleted; }
	virtual FInteractionCanceled&           GetOnInteractionCanceledHandle()               override { return OnInteractionCanceled; }
	virtual FLifecycleCompleted&            GetOnLifecycleCompletedHandle()                override { return OnLifecycleCompleted; }
	virtual FCooldownCompleted&             GetOnCooldownCompletedHandle()                 override { return OnCooldownCompleted; }
	virtual FInteractableStateChanged&      GetInteractableStateChangedHandle()            override { return OnInteractableStateChanged; }
	virtual FInteractableDependencyChanged& GetInteractableDependencyChangedHandle()       override { return OnInteractableDependencyChanged; }
	virtual FInteractableDependencyStarted& GetInteractableDependencyStartedHandle()       override { return OnInteractableDependencyStarted; }
	virtual FInteractableDependencyStopped& GetInteractableDependencyStoppedHandle()       override { return OnInteractableDependencyStopped; }
	virtual FHighlightTypeChanged&          GetHighlightTypeChangedHandle()                override { return OnHighlightTypeChanged; }
	virtual FHighlightMaterialChanged&      GetHighlightMaterialChangedHandle()            override { return OnHighlightMaterialChanged; }
	virtual FInteractableWidgetVisibilityChanged& GetInteractableWidgetVisibilityChangedHandle() override { return OnInteractableWidgetVisibilityChanged; }
	virtual FInteractionDeviceChanged&      GetInteractionDeviceChangedHandle()            override { return OnInteractionDeviceChanged; }

#pragma endregion

#pragma region Networking

protected:

	/** Server-authoritative slot join. Validates tags and capacity before adding to ActiveInteractorSlots. */
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractorJoin_Server(const TScriptInterface<IMounteaInteractorInterface>& RequestingInteractor);

	/** Server-authoritative slot leave. Removes the interactor from ActiveInteractorSlots. */
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractorLeave_Server(const TScriptInterface<IMounteaInteractorInterface>& LeavingInteractor);

	/** Server-authoritative state change. Authority validates the transition. */
	UFUNCTION(Server, Reliable, WithValidation)
	void SetState_Server(const EInteractableStateV3 NewState);

	/** Called on all clients when ActiveInteractorSlots replicates. Updates UI and fires OnInteractorSlotsChanged. */
	UFUNCTION()
	void OnRep_ActiveInteractorSlots();

	/** Called on all clients when InteractionStartServerTime replicates. Clients compute progress from server delta. */
	UFUNCTION()
	void OnRep_InteractionStartServerTime();

	/** Called on all clients when InteractableState replicates. */
	UFUNCTION()
	void OnRep_InteractableState();

	UFUNCTION(Client, Unreliable) void StartHighlight_Client();
	UFUNCTION(Client, Unreliable) void StopHighlight_Client();

	/** ANDs bWidgetStateEligible with bWidgetDistanceEligible and applies the result via
	 *  SetVisibility() - the single place actual widget visibility gets decided. */
	void RefreshWidgetVisibility();

#pragma endregion

#pragma region InternalFunctions

protected:

	virtual void ProcessToggleActive(const bool bIsEnabled);
	virtual void ProcessStartHighlight();
	virtual void ProcessStopHighlight();
	virtual void ProcessShowWidget();
	virtual void ProcessHideWidget();

	/** True while any OTHER IMounteaInteractableInterface component on the same actor is
	 *  EIS_Active. Gates RequestInteractorJoin only - siblings stay visible/selectable in Tier 2
	 *  action lists, they're just rejected (like a full slot) if actually started. */
	bool IsBlockedBySiblingInteractable() const;

	/** Force-releases every currently joined interactor's lock (without re-entrant callbacks into
	 *  this interactable) and clears all slots. Shared by EndPlay (mid-teardown) and
	 *  CompleteInteractable (a multi-occupant interaction naturally concluding) - both need "kick
	 *  everyone out cleanly" with the same semantics, just at different moments. */
	void ReleaseAllInteractorSlots();

	UFUNCTION()
	void OnInputModeChanged(ECommonInputType CommonInput);

	TSubclassOf<UUserWidget> GetInteractableWidgetClass() const { return WidgetClass; }

#if WITH_EDITOR
public:
	UFUNCTION(BlueprintCallable, Category="MounteaInteraction")
	void SetDefaultValues();
#endif

#pragma endregion

#pragma region OverlapCallbacks

protected:

	UFUNCTION()
	virtual void OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnInteractableStopOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnInteractableTraced(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

#pragma endregion

#pragma region Delegates

public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea Interaction|Interactable",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FInteractionStarted OnInteractionStarted;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea Interaction|Interactable",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FInteractionStopped OnInteractionStopped;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea Interaction|Interactable",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FInteractionCompleted OnInteractionCompleted;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea Interaction|Interactable",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FInteractionCycleCompleted OnInteractionCycleCompleted;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FInteractionCanceled OnInteractionCanceled;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FLifecycleCompleted OnLifecycleCompleted;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FCooldownCompleted OnCooldownCompleted;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea Interaction|Interactable",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FInteractorFound OnInteractorFound;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea Interaction|Interactable",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FInteractorLost OnInteractorLost;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea Interaction|Interactable",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FOnInteractorSlotsChanged OnInteractorSlotsChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea Interaction|Interactable",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FOnInteractorRejected OnInteractorRejected;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FInteractableStateChanged OnInteractableStateChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FInteractableWeightChanged OnInteractableWeightChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FInteractableCollisionChannelChanged OnInteractableCollisionChannelChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FLifecycleModeChanged OnLifecycleModeChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FLifecycleCountChanged OnLifecycleCountChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FCooldownPeriodChanged OnCooldownPeriodChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FHighlightTypeChanged OnHighlightTypeChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FHighlightMaterialChanged OnHighlightMaterialChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FInteractableWidgetVisibilityChanged OnInteractableWidgetVisibilityChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FInteractionDeviceChanged OnInteractionDeviceChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea Interaction|Interactable",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FOnWidgetUpdated OnWidgetUpdated;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactable")
	FInteractableDependencyChanged OnInteractableDependencyChanged;

	FInteractableDependencyStarted OnInteractableDependencyStarted;
	FInteractableDependencyStopped OnInteractableDependencyStopped;

#pragma endregion

#pragma region Properties_Debug

protected:

	UPROPERTY(EditAnywhere, Category="Mountea Interaction|Debug", meta=(ShowOnlyInnerProperties))
	FDebugSettings DebugSettings;

#if WITH_EDITORONLY_DATA
	TObjectPtr<UBillboardComponent> InteractableSpriteComponent = nullptr;
#endif

#pragma endregion

#pragma region Properties_Required

protected:

	/** Interaction duration (-1 = instant, 0 = 0.1s). */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Required", meta=(UIMin=-1, ClampMin=-1, Units="seconds", NoResetToDefault))
	float InteractionPeriod = 3.f;

	/** State on BeginPlay. Cannot be Active, Cooldown, or Finished. */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Required", meta=(NoResetToDefault))
	EInteractableStateV3 DefaultInteractableState = EInteractableStateV3::EIS_Awake;

	/** Auto-setup mode for collision/highlight components. */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Required", meta=(NoResetToDefault))
	ESetupType SetupType = ESetupType::EST_Quick;

	/** Collision channel for overlap/trace detection. */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Required", meta=(NoResetToDefault))
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_Camera;

	/** Cooldown duration (seconds) after Cycled lifecycle completes. */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Required",
		meta=(NoResetToDefault, EditCondition="LifecycleMode==EInteractableLifecycle::EIL_Cycled", UIMin=0.1, ClampMin=0.1, Units="Seconds"))
	float CooldownPeriod = 3.f;

	/** Lifecycle mode: Cycled (repeatable) or Once (single use). */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Required", meta=(NoResetToDefault))
	EInteractableLifecycle LifecycleMode = EInteractableLifecycle::EIL_Cycled;

	/** How many times this can be triggered (-1 = infinite, min 2 when Cycled). */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Required",
		meta=(NoResetToDefault, EditCondition="LifecycleMode==EInteractableLifecycle::EIL_Cycled", UIMin=-1, ClampMin=-1, Units="times"))
	int32 LifecycleCount = -1;

	/** Priority weight for Tier 1 focus selection. Higher = preferred. */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Required", meta=(UIMin=0, ClampMin=0, NoResetToDefault))
	int32 InteractionWeight = 1;

	/** Maximum concurrent interactors. Slot count is validated server-side. */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Required", meta=(UIMin=1, ClampMin=1))
	int32 MaxInteractors = 1;

	/** If true, this component forces immediate selection - no action cycling UI. */
	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Required")
	bool bStandaloneInteraction = false;

	/** Attention and Interaction range thresholds. */
	UPROPERTY(Replicated, EditAnywhere, Category="Mountea Interaction|Required")
	FInteractableWidgetRanges WidgetRanges;

#pragma endregion

#pragma region Properties_Optional

protected:

	/** Tags an interactor must match (any) to join. Empty = no filter. */
	UPROPERTY(Replicated, EditAnywhere, Category="Mountea Interaction|Optional")
	FGameplayTagContainer RequiredInteractorTags;

	/** Tags that disqualify an interactor (any match) from joining, checked alongside
	 *  RequiredInteractorTags. Empty = exclude none. */
	UPROPERTY(Replicated, EditAnywhere, Category="Mountea Interaction|Optional")
	FGameplayTagContainer ExcludedInteractorTags;

	/** Tags this interactable exposes to interactors for filtering and slot matching. */
	UPROPERTY(Replicated, EditAnywhere, Category="Mountea Interaction|Optional")
	FGameplayTagContainer InteractableCompatibleTags;

	/** Classes whose interactors are silently ignored for overlap/trace events. */
	UPROPERTY(SaveGame, EditAnywhere, Category="Mountea Interaction|Optional",
		meta=(NoResetToDefault, AllowAbstract=false, MustImplement="/Script/MounteaInteractionSystem.MounteaInteractorInterface", BlueprintBaseOnly))
	TArray<TSoftClassPtr<UObject>> IgnoredClasses;

	/** Component tag filter for auto-setup collision shapes. */
	UPROPERTY(EditAnywhere, Category="Mountea Interaction|Optional")
	TArray<FName> CollisionOverrides;

	/** Component tag filter for auto-setup highlightable meshes. */
	UPROPERTY(EditAnywhere, Category="Mountea Interaction|Optional")
	TArray<FName> HighlightableOverrides;

	UPROPERTY(Replicated, SaveGame, EditAnywhere, BlueprintReadOnly, Category="Mountea Interaction|Optional")
	EHighlightType HighlightType = EHighlightType::EHT_OverlayMaterial;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Mountea Interaction|Optional")
	uint8 bInteractionHighlight : 1;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Mountea Interaction|Optional",
		meta=(EditCondition="bInteractionHighlight==true", UIMin=0, ClampMin=0, UIMax=255, ClampMax=255))
	int32 StencilID = 133;

	UPROPERTY(Replicated, SaveGame, EditAnywhere, BlueprintReadOnly, Category="Mountea Interaction|Optional",
		meta=(EditCondition="bInteractionHighlight==true"))
	TObjectPtr<UMaterialInterface> HighlightMaterial = nullptr;

	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Optional")
	FDataTableRowHandle InteractableData;

	UPROPERTY(Replicated, SaveGame, EditAnywhere, Category="Mountea Interaction|Optional")
	FText InteractableName = NSLOCTEXT("InteractableComponentBase", "DefaultInteractable", "Default");

	UPROPERTY(SaveGame, EditAnywhere, Category="Mountea Interaction|Optional")
	ETimingComparison ComparisonMethod = ETimingComparison::ECM_None;

	UPROPERTY(SaveGame, EditAnywhere, Category="Mountea Interaction|Optional",
		meta=(UIMin=0.001, ClampMin=0.001, Units="seconds", EditCondition="ComparisonMethod!=ETimingComparison::ECM_None"))
	float TimeToStart = 0.5f;

#pragma endregion

#pragma region Properties_ReadOnly

protected:

	/** Current state - drives the state machine. */
	UPROPERTY(ReplicatedUsing=OnRep_InteractableState, SaveGame, VisibleAnywhere, Category="Mountea Interaction|Read Only")
	EInteractableStateV3 InteractableState = EInteractableStateV3::EIS_Disabled;

	/** Remaining lifecycle count. -1 = infinite. */
	UPROPERTY(Replicated, SaveGame, VisibleAnywhere, Category="Mountea Interaction|Read Only")
	int32 RemainingLifecycleCount = -1;

	/**
	 * Server world time when the current interaction started.
	 * Clients derive progress as: (ServerNow - InteractionStartServerTime) / InteractionPeriod.
	 * Replaces FTimerHandle replication for multiplayer accuracy.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_InteractionStartServerTime, VisibleAnywhere, Category="Mountea Interaction|Read Only")
	float InteractionStartServerTime = -1.f;

	/** Active interactor slots (N-interactor support). */
	UPROPERTY(ReplicatedUsing=OnRep_ActiveInteractorSlots, SaveGame, VisibleAnywhere, Category="Mountea Interaction|Read Only")
	TArray<FInteractorSlot> ActiveInteractorSlots;

	UPROPERTY(SaveGame, VisibleAnywhere, Category="Mountea Interaction|Read Only")
	TArray<TScriptInterface<IMounteaInteractableInterface>> InteractionDependencies;

	UPROPERTY(SaveGame, VisibleAnywhere, Category="Mountea Interaction|Read Only")
	mutable TMap<TObjectPtr<UPrimitiveComponent>, FCollisionShapeCache> CachedCollisionShapesSettings;

	UPROPERTY(SaveGame, VisibleAnywhere, Category="Mountea Interaction|Read Only")
	TArray<TObjectPtr<UMeshComponent>> HighlightableComponents;

	UPROPERTY(SaveGame, VisibleAnywhere, Category="Mountea Interaction|Read Only")
	TArray<TObjectPtr<UPrimitiveComponent>> CollisionComponents;

	UPROPERTY(VisibleAnywhere, Category="Mountea Interaction|Read Only")
	int32 CachedInteractionWeight = 1;

	UPROPERTY()
	FTimerHandle Timer_Cooldown;

	UPROPERTY(VisibleAnywhere)
	uint8 bInteractableInitialized : 1;

	/** Coarse widget gate: true when InteractableState alone permits the widget to show
	 *  (Active/Awake). Set by OnRep_InteractableState(). ANDed with bWidgetDistanceEligible in
	 *  RefreshWidgetVisibility() - state eligible is necessary but not sufficient; a viewing
	 *  interactor must also confirm range. */
	UPROPERTY()
	bool bWidgetStateEligible = false;

	/** Fine-grained widget gate: true once a viewing interactor's own distance check (see
	 *  RefreshFocus()) has confirmed this interactable is within WidgetRanges.InteractionRange.
	 *  Set via ToggleWidgetVisibility(). Local to each client's own component instance - never
	 *  replicated, since each client decides this independently for its own camera/pawn position. */
	UPROPERTY()
	bool bWidgetDistanceEligible = false;

#pragma endregion

#pragma region Editor

#if WITH_EDITOR
protected:

	/** Static delegate fired in PostEditChangeProperty. Editor subsystem (Phase 8) binds to this. */
	static FSimpleMulticastDelegate OnInteractableComponentChanged;

	DECLARE_EVENT(UMounteaInteractableComponentBase, FRequestEditorDefaults)
	FRequestEditorDefaults RequestEditorDefaults;

	virtual void ResetDefaults();
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;

public:
	/** Returns the editor-change delegate so the EditorSubsystem can bind to it. */
	static FSimpleMulticastDelegate& GetOnInteractableComponentChangedDelegate() { return OnInteractableComponentChanged; }
#endif

#if (!UE_BUILD_SHIPPING || WITH_EDITOR)
protected:
	virtual void DrawDebug();
#endif

#pragma endregion
};
