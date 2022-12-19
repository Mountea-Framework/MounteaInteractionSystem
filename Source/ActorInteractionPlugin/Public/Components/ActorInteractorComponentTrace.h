// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractorComponentBase.h"
#include "CollisionQueryParams.h"
#include "ActorInteractorComponentTrace.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETraceType : uint8
{
	ETT_Precise		UMETA(DisplayName = "Precise", Tooltip = "Raycast/Line Trace."),
	ETT_Loose		UMETA(DisplayName = "Loose", Tooltip = "Cubecast/Cube Trace."),

	Default
};

/**
 * Interaction Tracing Data used easier data management.
 */
struct FInteractionTraceDataV2
{
	FVector StartLocation;
	FVector EndLocation;
	FRotator TraceRotation;
	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;
	ECollisionChannel CollisionChannel;

	// Default zero constructor
	FInteractionTraceDataV2()
	{
		StartLocation = FVector();
		EndLocation = FVector();
		TraceRotation = FRotator();
		CollisionParams = FCollisionQueryParams();
		CollisionChannel = ECC_Visibility;
	};
	
	FInteractionTraceDataV2(const FVector& Start, const FVector& End, const FRotator Rotation, const FCollisionQueryParams& Params, const ECollisionChannel& Channel)
	{
		StartLocation = Start;
		EndLocation = End;
		TraceRotation = Rotation;
		CollisionParams = Params;
		CollisionChannel = Channel;
	};
};

#pragma region TracingData
USTRUCT(BlueprintType)
struct FTracingData
{
	GENERATED_BODY()

	UPROPERTY(Category="Interaction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
	ETraceType TracingType;
	UPROPERTY(Category="Interaction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
	float TracingInterval;
	UPROPERTY(Category="Interaction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
	float TracingRange;
	UPROPERTY(Category="Interaction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
	float TracingShapeHalfSize;
	UPROPERTY(Category="Interaction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
	uint8 bUsingCustomStartTransform : 1;
	UPROPERTY(Category="Interaction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
	FTransform CustomTracingTransform;

	FTracingData() :
	TracingType(ETraceType::ETT_Loose),
	TracingInterval(0.01f),
	TracingRange(250.f),
	TracingShapeHalfSize(5.f),
	bUsingCustomStartTransform(false),
	CustomTracingTransform(FTransform())
	{};

	FTracingData
	(
		ETraceType NewType,
		float NewInterval,
		float NewRange,
		float NewShapeHalfSize,
		bool bUse,
		FTransform NewTransform
	) :
	TracingType(NewType), bUsingCustomStartTransform(bUse), CustomTracingTransform(NewTransform)
	{
		TracingInterval = FMath::Max(0.01f, NewInterval);
		TracingRange = FMath::Max(1.f, NewRange);
		TracingShapeHalfSize = FMath::Max(0.1f, NewShapeHalfSize);
	}

	inline bool operator==(const FTracingData& Other) const
	{
		return
		TracingType == Other.TracingType &&
		FMath::IsNearlyEqual(TracingInterval, Other.TracingInterval) &&
		FMath::IsNearlyEqual(TracingRange, Other.TracingRange) &&
		FMath::IsNearlyEqual(TracingShapeHalfSize, Other.TracingShapeHalfSize) &&
		bUsingCustomStartTransform == Other.bUsingCustomStartTransform &&
		(bUsingCustomStartTransform && CustomTracingTransform.Equals(Other.CustomTracingTransform))
		;
	}

	inline bool operator!=(const FTracingData& Other) const
	{
		return !(*this==Other);
	}
};
#pragma endregion 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTracingDataChanged, const FTracingData&, NewTracingData, const FTracingData&, OldTracingData);

/**
 * 
 */
UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation, Rendering), meta=(BlueprintSpawnableComponent, DisplayName = "Interactor Component Trace"))
class ACTORINTERACTIONPLUGIN_API UActorInteractorComponentTrace : public UActorInteractorComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractorComponentTrace();

protected:

	virtual void BeginPlay() override;

protected:

	/**
	 * Disables Tracing. Can be Enabled again. Clears all timers.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Tracing")
	virtual void DisableTracing();
	/**
	 * Tries to enable Tracing. Could fail if non valid state.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Tracing")
	virtual void EnableTracing();
	/**
	 * Pauses Tracing if already active.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Tracing")
	virtual void PauseTracing();
	UFUNCTION()
	virtual void ResumeTracing();

	UFUNCTION()	virtual void ProcessTrace();
	virtual void ProcessTrace_Precise(FInteractionTraceDataV2& InteractionTraceData);
	virtual void ProcessTrace_Loose(FInteractionTraceDataV2& InteractionTraceData);

	/**
	 * Returns whether Tracing is allowed or not.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool CanTrace() const;

	/**
	 * Returns Trace Type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual ETraceType GetTraceType() const;
	/**
	 * Sets Trace Type.
	 *
	 * @param NewTraceType	Value to be set.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetTraceType(const ETraceType& NewTraceType);

	/**
	 * Returns Trace Interval in seconds.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual float GetTraceInterval() const;
	/**
	 * Sets Trace Interval in seconds.
	 * Clamped to be at least 0.01s.
	 *
	 * @param NewInterval	Value to be set
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetTraceInterval(const float NewInterval);

	/**
	 * Returns Trace Range in cm.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual float GetTraceRange() const;
	/**
	 * Sets Trace Range in cm.
	 * Clamped to be at least 1cm.
	 *
	 * @param NewRange	Value to be set
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetTraceRange(const float NewRange);

	/**
	 * Returns Trace Shape Half Size in cm.
	 * Defines how precise tracing is when using Loose tracing type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual float GetTraceShapeHalfSize() const;
	/**
	 * Sets Trace Shape Half Size in cm.
	 * Clamped to be at least 0.1cm.
	 *
	 * @param NewTraceShapeHalfSize	Value to be set
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetTraceShapeHalfSize(const float NewTraceShapeHalfSize);

	/**
	 * Returns whether using Custom Trace Transform.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual bool GetUseCustomStartTransform() const;
	/**
	 * Sets Using Custom Trace Start Transform.
	 *
	 * @param bUse	Value to be set
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetUseCustomStartTransform(const bool bUse);

	/**
	 * Returns transient Tracing Data.
	 * Structure of all Tracing Data at one place.
	 * Updated every time any value is changed.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual FTracingData GetLastTracingData() const;

	/**
	 * Sets Trace Start to specified location.
	 *
	 * @param TraceStart	Value to be used as Custom Trace Start.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetCustomTraceStart(FTransform TraceStart);
	/**
	 * Returns current Custom Trace Start value.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual FTransform GetCustomTraceStart() const;
	
protected:
	
	virtual bool CanInteract() const override;
	virtual void SetState(const EInteractorStateV2 NewState) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnTraceDataChangedEvent(const FTracingData& NewType, const FTracingData& OldType);

protected:
	
	/**
	 * Defines how precise the interaction is.
	 * - Loose Tracing is using BoxTrace and does not require precision.
	 * - Precise Tracing is using LineTrace and requires higher precision. Useful with smaller objects.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required")
	ETraceType TraceType;
	
	/**
	 * Optimization feature.
	 * The frequency in seconds at which the Interaction function will be executed.
	 *
	 * Min value is 0.01 (1e-2)
	 * Higher the value, less frequent tracing is and less performance is required.
	 * Lower the value, more frequent tracing is and more performance is required.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(Units = "s", UIMin=0.01f, ClampMin=0.01f, DisplayName="Tick Interval (sec)"))
	float TraceInterval;

	/**
	 * Defines the length of interaction tracing.
	 * Higher the value, further items can be reached.
	 * Min value: 0.01cm
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(Units = "cm", UIMin=1, ClampMin=1, DisplayName="Interaction Range (cm)"))
	float TraceRange;

	/**
	 * Defines half extend of Loose Tracing.
	 * - Higher the value, less precise interaction is.
	 * - Lower the value, more precise interaction is.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required", meta=(Units = "cm", UIMin=0.1f, ClampMin=0.1f))
	float TraceShapeHalfSize = 5.0f;
	
	/**
	 * Defines whether Tracing starts at ActorEyesViewPoint (default) or at a given Location.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Required")
	uint8 bUseCustomStartTransform : 1;
	
	/**
	 * Transform in World Space.
	 * Suggested use:
	 * - Update from Code/Blueprint to follow Socket, like Weapon Barrel
	 * 
	 * Defines where does the Tracing start and what direction it follows.
	 * Will be ignored if bUseCustomStartTransform is false.
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only", AdvancedDisplay, meta=(DisplayName="Trace Start (World Space Transform)"))
	FTransform CustomTraceTransform;

	/**
	 * Structure of all Tracing Data at one place.
	 * Updated every time any value is changed.
	 */
	UPROPERTY(Transient, VisibleAnywhere, Category="Interaction|Read Only")
	FTracingData LastTracingData;

	/**
	 * Timer Handle.
	 * Won't display any values in Blueprints.
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only")
	FTimerHandle Timer_Ticking;

protected:

	/**
	 * Event called every time any Trace value has changed.
	 * Will provide information about Old data and New data. Useful when debugging.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FTracingDataChanged OnTraceDataChanged;

#if WITH_EDITOR

protected:

	virtual void DrawTracingDebugStart(FInteractionTraceDataV2& InteractionTraceData) const;
	virtual void DrawTracingDebugEnd(FInteractionTraceDataV2& InteractionTraceData) const;
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;

#endif
};