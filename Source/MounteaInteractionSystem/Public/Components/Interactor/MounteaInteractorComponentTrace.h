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
	ETT_Precise	UMETA(DisplayName="Precise", Tooltip="Line Trace — requires precision, better for small objects."),
	ETT_Loose	UMETA(DisplayName="Loose",   Tooltip="Box Trace — forgiving, better for large objects."),
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MounteaInteraction|FTracingData")
	EMounteaTraceType TracingType = EMounteaTraceType::ETT_Loose;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MounteaInteraction|FTracingData")
	float TracingInterval = 0.05f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MounteaInteraction|FTracingData")
	float TracingRange = 250.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MounteaInteraction|FTracingData")
	float TracingShapeHalfSize = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MounteaInteraction|FTracingData")
	uint8 bUsingCustomStartTransform : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MounteaInteraction|FTracingData")
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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void DisableTracing();
	virtual void DisableTracing_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void EnableTracing();
	virtual void EnableTracing_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void PauseTracing();
	virtual void PauseTracing_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void ResumeTracing();
	virtual void ResumeTracing_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	bool CanTrace() const;
	virtual bool CanTrace_Implementation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	EMounteaTraceType GetTraceType() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetTraceType(const EMounteaTraceType& NewTraceType);
	virtual void SetTraceType_Implementation(const EMounteaTraceType& NewTraceType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	float GetTraceInterval() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetTraceInterval(const float NewInterval);
	virtual void SetTraceInterval_Implementation(const float NewInterval);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	float GetTraceRange() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetTraceRange(const float NewRange);
	virtual void SetTraceRange_Implementation(const float NewRange);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	float GetTraceShapeHalfSize() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetTraceShapeHalfSize(const float NewTraceShapeHalfSize);
	virtual void SetTraceShapeHalfSize_Implementation(const float NewTraceShapeHalfSize);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	bool GetUseCustomStartTransform() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetUseCustomStartTransform(const bool bUse);
	virtual void SetUseCustomStartTransform_Implementation(const bool bUse);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	FTracingData GetLastTracingData() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetCustomTraceStart(const FTransform& TraceStart);
	virtual void SetCustomTraceStart_Implementation(const FTransform& TraceStart);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	FTransform GetCustomTraceStart() const;

protected:

	UFUNCTION(BlueprintNativeEvent, Category="MounteaInteraction|Tracing")
	void ProcessTrace();
	virtual void ProcessTrace_Implementation();

	virtual void ProcessTrace_Precise(FInteractionTraceDataV2& TraceData);
	virtual void ProcessTrace_Loose(FInteractionTraceDataV2& TraceData);

	/** Called after each trace completes. Diffs hits against DetectedSet. */
	UFUNCTION(BlueprintNativeEvent, Category="MounteaInteraction|Tracing")
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

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FTracingDataChanged OnTraceDataChanged;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FOnTraced OnTraced;

protected:

	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required")
	EMounteaTraceType TraceType = EMounteaTraceType::ETT_Loose;

	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required",
		meta=(Units="s", UIMin=0.01, ClampMin=0.01, DisplayName="Tick Interval (sec)"))
	float TraceInterval = 0.05f;

	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required",
		meta=(Units="cm", UIMin=1, ClampMin=1, DisplayName="Interaction Range (cm)"))
	float TraceRange = 250.f;

	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required",
		meta=(Units="cm", UIMin=0.1, ClampMin=0.1))
	float TraceShapeHalfSize = 5.f;

	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required")
	uint8 bUseCustomStartTransform : 1;

	UPROPERTY(Replicated, VisibleAnywhere, Category="MounteaInteraction|Read Only", AdvancedDisplay)
	FTransform CustomTraceTransform;

	UPROPERTY(Transient, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	FTracingData LastTracingData;

	UPROPERTY(VisibleAnywhere, Category="MounteaInteraction|Read Only")
	FTimerHandle Timer_Ticking;

	/** Interactables detected in the most recent trace (used to diff against DetectedSet). */
	TSet<TScriptInterface<IMounteaInteractableInterface>> LastTraceHits;
};
