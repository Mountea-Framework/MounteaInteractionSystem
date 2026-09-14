// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInteractorComponentBase.h"
#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"
#include "MounteaInteractorComponentTrace.generated.h"

UENUM(BlueprintType)
enum class EMounteaTraceType : uint8
{
	ETT_Precise	UMETA(DisplayName="Precise", Tooltip="Line Trace - requires precision, better for small objects."),
	ETT_Loose	UMETA(DisplayName="Loose",   Tooltip="Box Trace - forgiving, better for large objects."),
	Default		UMETA(Hidden)
};

/** Transient trace data bundled per trace tick. */
struct FInteractionTraceDataV2
{
	FVector StartLocation;
	FVector EndLocation;
	FRotator TraceRotation;
	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;
	ECollisionChannel CollisionChannel;

	FInteractionTraceDataV2()
		: StartLocation(FVector::ZeroVector), EndLocation(FVector::ZeroVector)
		, TraceRotation(FRotator::ZeroRotator), CollisionChannel(ECC_Visibility) {}

	FInteractionTraceDataV2(const FVector& Start, const FVector& End, const FRotator Rotation,
		const FCollisionQueryParams& Params, const ECollisionChannel& Channel)
		: StartLocation(Start), EndLocation(End), TraceRotation(Rotation)
		, CollisionParams(Params), CollisionChannel(Channel) {}
};

/** Replicated tracing configuration. */
USTRUCT(BlueprintType)
struct FTracingData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Mountea Interaction|FTracingData")
	EMounteaTraceType TracingType = EMounteaTraceType::ETT_Loose;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Mountea Interaction|FTracingData")
	float TracingInterval = 0.05f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Mountea Interaction|FTracingData")
	float TracingRange = 250.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Mountea Interaction|FTracingData")
	float TracingShapeHalfSize = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Mountea Interaction|FTracingData")
	uint8 bUsingCustomStartTransform : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Mountea Interaction|FTracingData")
	FTransform CustomTracingTransform;

	FTracingData() : bUsingCustomStartTransform(0) {}

	bool operator==(const FTracingData& Other) const
	{
		return TracingType == Other.TracingType
			&& FMath::IsNearlyEqual(TracingInterval, Other.TracingInterval)
			&& FMath::IsNearlyEqual(TracingRange, Other.TracingRange)
			&& FMath::IsNearlyEqual(TracingShapeHalfSize, Other.TracingShapeHalfSize)
			&& bUsingCustomStartTransform == Other.bUsingCustomStartTransform
			&& (!bUsingCustomStartTransform || CustomTracingTransform.Equals(Other.CustomTracingTransform));
	}
	bool operator!=(const FTracingData& Other) const { return !(*this == Other); }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTracingDataChanged, const FTracingData&, NewTracingData, const FTracingData&, OldTracingData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTraced);

/**
 * v5.0 Actor Interactor Trace Component
 *
 * Detection method: periodic line/box trace from owner's eye location (or custom transform).
 * On each trace tick, diffs the hit set against DetectedSet: adds new hits, removes stale ones.
 * The base's RefreshFocus() handles Tier 1/2 selection.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactor-Component-Trace
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName="Interactor Component Trace"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractorComponentTrace : public UMounteaInteractorComponentBase
{
	GENERATED_BODY()

public:

	UMounteaInteractorComponentTrace();

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ProcessStateChanged() override;
	virtual FString ToString_Implementation() const override;

public:

	/** Stops the periodic trace timer, releases all detected interactables, and clears trace hits. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Disable Tracing"))
	void DisableTracing();
	virtual void DisableTracing_Implementation();

	/** Starts the periodic trace timer, ticking at TraceInterval while active. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Enable Tracing"))
	void EnableTracing();
	virtual void EnableTracing_Implementation();

	/** Pauses the periodic trace timer without clearing any currently detected interactables. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Pause Tracing"))
	void PauseTracing();
	virtual void PauseTracing_Implementation();

	/** Resumes a previously paused trace timer. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Resume Tracing"))
	void ResumeTracing();
	virtual void ResumeTracing_Implementation();

	/** Returns true if this interactor is currently allowed to trace. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Can Trace"))
	bool CanTrace() const;
	virtual bool CanTrace_Implementation() const;

	/** Returns whether this interactor uses a precise line trace or a loose box trace. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(DisplayName="Get Trace Type"))
	EMounteaTraceType GetTraceType() const;

	/** Sets whether this interactor uses a precise line trace or a loose box trace. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Set Trace Type"))
	void SetTraceType(const EMounteaTraceType& NewTraceType);
	virtual void SetTraceType_Implementation(const EMounteaTraceType& NewTraceType);

	/** Returns how often, in seconds, the trace re-evaluates. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(DisplayName="Get Trace Interval"))
	float GetTraceInterval() const;

	/** Sets how often, in seconds, the trace re-evaluates. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Set Trace Interval"))
	void SetTraceInterval(const float NewInterval);
	virtual void SetTraceInterval_Implementation(const float NewInterval);

	/** Returns the maximum distance, in centimeters, the trace will reach. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(DisplayName="Get Trace Range"))
	float GetTraceRange() const;

	/** Sets the maximum distance, in centimeters, the trace will reach. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Set Trace Range"))
	void SetTraceRange(const float NewRange);
	virtual void SetTraceRange_Implementation(const float NewRange);

	/** Returns the half-size, in centimeters, of the box shape used for loose tracing. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(DisplayName="Get Trace Shape Half Size"))
	float GetTraceShapeHalfSize() const;

	/** Sets the half-size, in centimeters, of the box shape used for loose tracing. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Set Trace Shape Half Size"))
	void SetTraceShapeHalfSize(const float NewTraceShapeHalfSize);
	virtual void SetTraceShapeHalfSize_Implementation(const float NewTraceShapeHalfSize);

	/** Returns whether the trace starts from a custom transform instead of the owner's eyes view point. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(DisplayName="Get Use Custom Start Transform"))
	bool GetUseCustomStartTransform() const;

	/** Sets whether the trace starts from a custom transform instead of the owner's eyes view point. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Set Use Custom Start Transform"))
	void SetUseCustomStartTransform(const bool bUse);
	virtual void SetUseCustomStartTransform_Implementation(const bool bUse);

	/** Returns a snapshot of this interactor's current tracing configuration. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(DisplayName="Get Last Tracing Data"))
	FTracingData GetLastTracingData() const;

	/** Sets the custom trace start transform used when GetUseCustomStartTransform is true. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Set Custom Trace Start"))
	void SetCustomTraceStart(const FTransform& TraceStart);
	virtual void SetCustomTraceStart_Implementation(const FTransform& TraceStart);

	/** Returns the custom trace start transform used when GetUseCustomStartTransform is true. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor",
		meta=(DisplayName="Get Custom Trace Start"))
	FTransform GetCustomTraceStart() const;

protected:

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Process Trace"))
	void ProcessTrace();
	virtual void ProcessTrace_Implementation();

	virtual void ProcessTrace_Precise(FInteractionTraceDataV2& TraceData);
	virtual void ProcessTrace_Loose(FInteractionTraceDataV2& TraceData);

	/** Called after each trace completes. Diffs hits against DetectedSet. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactor|Tracing",
		meta=(DisplayName="Post Traced"))
	void PostTraced();
	virtual void PostTraced_Implementation();

protected:

	UFUNCTION(Server, Reliable)
	void DisableTracing_Server();
	UFUNCTION(Server, Reliable)
	void EnableTracing_Server();
	UFUNCTION(Server, Reliable)
	void PauseTracing_Server();
	UFUNCTION(Server, Reliable)
	void ResumeTracing_Server();
	UFUNCTION(Server, Reliable)
	void ProcessTrace_Server();

	UFUNCTION(Server, Unreliable)
	void SetTraceType_Server(const EMounteaTraceType& NewTraceType);
	UFUNCTION(Server, Unreliable)
	void SetTraceInterval_Server(float NewInterval);
	UFUNCTION(Server, Unreliable)
	void SetTraceRange_Server(float NewRange);
	UFUNCTION(Server, Unreliable)
	void SetTraceShapeHalfSize_Server(float NewTraceShapeHalfSize);
	UFUNCTION(Server, Unreliable)
	void SetUseCustomStartTransform_Server(bool bUse);
	UFUNCTION(Server, Unreliable)
	void SetCustomTraceStart_Server(const FTransform& TraceStart);

#if WITH_EDITOR
protected:
	virtual void DrawTracingDebugStart(FInteractionTraceDataV2& TraceData) const;
	virtual void DrawTracingDebugEnd(FInteractionTraceDataV2& TraceData) const;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

public:

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactor")
	FTracingDataChanged OnTraceDataChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactor")
	FOnTraced OnTraced;

protected:

	UPROPERTY(Replicated, EditAnywhere, Category="Mountea Interaction|Required")
	EMounteaTraceType TraceType = EMounteaTraceType::ETT_Loose;

	UPROPERTY(Replicated, EditAnywhere, Category="Mountea Interaction|Required",
		meta=(Units="s", UIMin=0.01, ClampMin=0.01, DisplayName="Tick Interval (sec)"))
	float TraceInterval = 0.05f;

	UPROPERTY(Replicated, EditAnywhere, Category="Mountea Interaction|Required",
		meta=(Units="cm", UIMin=1, ClampMin=1, DisplayName="Interaction Range (cm)"))
	float TraceRange = 250.f;

	UPROPERTY(Replicated, EditAnywhere, Category="Mountea Interaction|Required",
		meta=(Units="cm", UIMin=0.1, ClampMin=0.1))
	float TraceShapeHalfSize = 5.f;

	UPROPERTY(Replicated, EditAnywhere, Category="Mountea Interaction|Required")
	uint8 bUseCustomStartTransform : 1;

	UPROPERTY(Replicated, VisibleAnywhere, Category="Mountea Interaction|Read Only", AdvancedDisplay)
	FTransform CustomTraceTransform;

	UPROPERTY(Transient, VisibleAnywhere, Category="Mountea Interaction|Read Only")
	FTracingData LastTracingData;

	UPROPERTY(VisibleAnywhere, Category="Mountea Interaction|Read Only")
	FTimerHandle Timer_Ticking;

	/** Interactables detected in the most recent trace (used to diff against DetectedSet). */
	TSet<TScriptInterface<IMounteaInteractableInterface>> LastTraceHits;
};
