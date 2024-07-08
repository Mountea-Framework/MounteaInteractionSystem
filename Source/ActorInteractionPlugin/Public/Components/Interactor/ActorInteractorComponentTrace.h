// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractorComponentBase.h"
#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"
#include "ActorInteractorComponentTrace.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETraceType : uint8
{
	ETT_Precise		UMETA(DisplayName = "Precise", Tooltip = "Raycast/Line Trace."),
	ETT_Loose			UMETA(DisplayName = "Loose", Tooltip = "Cubecast/Cube Trace."),

	Default					 UMETA(hidden)
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

	UPROPERTY(Category="MounteaInteraction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
	ETraceType TracingType;
	UPROPERTY(Category="MounteaInteraction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
	float TracingInterval;
	UPROPERTY(Category="MounteaInteraction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
	float TracingRange;
	UPROPERTY(Category="MounteaInteraction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
	float TracingShapeHalfSize;
	UPROPERTY(Category="MounteaInteraction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
	uint8 bUsingCustomStartTransform : 1;
	UPROPERTY(Category="MounteaInteraction|FTracingData", VisibleAnywhere, BlueprintReadWrite)
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTraced);

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName = "Interactor Component Trace"))
class ACTORINTERACTIONPLUGIN_API UActorInteractorComponentTrace : public UActorInteractorComponentBase
{
	GENERATED_BODY()

public:

	UActorInteractorComponentTrace();

protected:

	virtual void BeginPlay() override;

public:

	/**
	 * Disables Tracing. Can be Enabled again. Clears all timers.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void DisableTracing();
	virtual void DisableTracing_Implementation();
	
	/**
	 * Tries to enable Tracing. Could fail if non valid state.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void EnableTracing();
	virtual void EnableTracing_Implementation();
	
	/**
	 * Pauses Tracing if already active.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void PauseTracing();
	virtual void PauseTracing_Implementation();

	/**
	 * 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void ResumeTracing();
	virtual void ResumeTracing_Implementation();
	
	/**
	 * Returns whether Tracing is allowed or not.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	bool CanTrace() const;
	virtual bool CanTrace_Implementation() const;

	/**
	 * Returns Trace Type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	virtual ETraceType GetTraceType() const;
	
	/**
	 * Sets Trace Type.
	 *
	 * @param NewTraceType	Value to be set.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetTraceType(const ETraceType& NewTraceType);
	virtual void SetTraceType_Implementation(const ETraceType& NewTraceType);

	/**
	 * Returns Trace Interval in seconds.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	virtual float GetTraceInterval() const;
	
	/**
	 * Sets Trace Interval in seconds.
	 * Clamped to be at least 0.01s.
	 *
	 * @param NewInterval	Value to be set
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetTraceInterval(const float NewInterval);
	virtual void SetTraceInterval_Implementation(const float NewInterval);

	/**
	 * Returns Trace Range in cm.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	virtual float GetTraceRange() const;
	
	/**
	 * Sets Trace Range in cm.
	 * Clamped to be at least 1cm.
	 *
	 * @param NewRange	Value to be set
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetTraceRange(const float NewRange);
	virtual void SetTraceRange_Implementation(const float NewRange);

	/**
	 * Returns Trace Shape Half Size in cm.
	 * Defines how precise tracing is when using Loose tracing type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	virtual float GetTraceShapeHalfSize() const;
	
	/**
	 * Sets Trace Shape Half Size in cm.
	 * Clamped to be at least 0.1cm.
	 *
	 * @param NewTraceShapeHalfSize	Value to be set
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetTraceShapeHalfSize(const float NewTraceShapeHalfSize);
	virtual void SetTraceShapeHalfSize_Implementation(const float NewTraceShapeHalfSize);

	/**
	 * Returns whether using Custom Trace Transform.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	virtual bool GetUseCustomStartTransform() const;
	
	/**
	 * Sets Using Custom Trace Start Transform.
	 *
	 * @param bUse	Value to be set
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetUseCustomStartTransform(const bool bUse);
	virtual void SetUseCustomStartTransform_Implementation(const bool bUse);

	/**
	 * Returns transient Tracing Data.
	 * Structure of all Tracing Data at one place.
	 * Updated every time any value is changed.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	virtual FTracingData GetLastTracingData() const;

	/**
	 * Sets Trace Start to specified location.
	 *
	 * @param TraceStart	Value to be used as Custom Trace Start.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void SetCustomTraceStart(const FTransform& TraceStart);
	virtual void SetCustomTraceStart_Implementation(const FTransform& TraceStart);
	
	/**
	 * Returns current Custom Trace Start value.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor")
	virtual FTransform GetCustomTraceStart() const;

protected:
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
	void ProcessTrace();
	virtual void ProcessTrace_Implementation();
	virtual void ProcessTrace_Precise(FInteractionTraceDataV2& InteractionTraceData);
	virtual void ProcessTrace_Loose(FInteractionTraceDataV2& InteractionTraceData);
	
	/**
	 * Function called after Trace has finished.
	 * Could be called Twice:
	 * * On Client
	 * * On Server
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="MounteaInteraction|Tracing")
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
	void SetTraceType_Server(const ETraceType& NewTraceType);

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

	UFUNCTION(Client, Unreliable)
	void PostTraced_Client();
	
protected:

	virtual void ProcessStateChanged() override;

	virtual FString ToString_Implementation() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Variables
	
protected:
	
	/**
	 * Defines how precise the interaction is.
	 * - Loose Tracing is using BoxTrace and does not require precision.
	 * - Precise Tracing is using LineTrace and requires higher precision. Useful with smaller objects.
	 */
	UPROPERTY(Replicated,  EditAnywhere, Category="MounteaInteraction|Required")
	ETraceType																		TraceType;
	
	/**
	 * Optimization feature.
	 * The frequency in seconds at which the Interaction function will be executed.
	 *
	 * Min value is 0.01 (1e-2)
	 * Higher the value, less frequent tracing is and less performance is required.
	 * Lower the value, more frequent tracing is and more performance is required.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required", meta=(Units = "s", UIMin=0.01f, ClampMin=0.01f, DisplayName="Tick Interval (sec)"))
	float																					TraceInterval;

	/**
	 * Defines the length of interaction tracing.
	 * Higher the value, further items can be reached.
	 * Min value: 0.01cm
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required", meta=(Units = "cm", UIMin=1, ClampMin=1, DisplayName="Interaction Range (cm)"))
	float																					TraceRange;

	/**
	 * Defines half extend of Loose Tracing.
	 * - Higher the value, less precise interaction is.
	 * - Lower the value, more precise interaction is.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required", meta=(Units = "cm", UIMin=0.1f, ClampMin=0.1f))
	float																					TraceShapeHalfSize = 5.0f;
	
	/**
	 * Defines whether Tracing starts at ActorEyesViewPoint (default) or at a given Location.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="MounteaInteraction|Required")
	uint8																				bUseCustomStartTransform : 1;
	
	/**
	 * Transform in World Space.
	 * Suggested use:
	 * - Update from Code/Blueprint to follow Socket, like Weapon Barrel
	 * 
	 * Defines where does the Tracing start and what direction it follows.
	 * Will be ignored if bUseCustomStartTransform is false.
	 */
	UPROPERTY(Replicated, VisibleAnywhere, Category="MounteaInteraction|Read Only", AdvancedDisplay, meta=(DisplayName="Trace Start (World Space Transform)"))
	FTransform																		CustomTraceTransform;

	/**
	 * Structure of all Tracing Data at one place.
	 * Updated every time any value is changed.
	 */
	UPROPERTY(Transient, VisibleAnywhere, Category="MounteaInteraction|Read Only")
	FTracingData																	LastTracingData;

	/**
	 * Timer Handle.
	 * Won't display any values in Blueprints.
	 */
	UPROPERTY(VisibleAnywhere, Category="MounteaInteraction|Read Only")
	FTimerHandle																	Timer_Ticking;

#pragma endregion

#pragma region Events
	
protected:

	/**
	 * Event called every time any Trace value has changed.
	 * Will provide information about Old data and New data. Useful when debugging.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FTracingDataChanged													OnTraceDataChanged;

	/**
	 * 
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Interaction|Interactor")
	FOnTraced																		OnTraced;

#pragma endregion
	
#if WITH_EDITOR

protected:

	virtual void DrawTracingDebugStart(FInteractionTraceDataV2& InteractionTraceData) const;
	virtual void DrawTracingDebugEnd(FInteractionTraceDataV2& InteractionTraceData) const;
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;

#endif
};