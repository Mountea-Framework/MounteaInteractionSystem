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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceTypeChanged, const ETraceType&, NewTraceType);

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
	
	UFUNCTION(BlueprintCallable, Category="Interaction|Tracing")
	virtual void DisableTracing();
	UFUNCTION(BlueprintCallable, Category="Interaction|Tracing")
	virtual void EnableTracing();
	UFUNCTION(BlueprintCallable, Category="Interaction|Tracing")
	virtual void PauseTracing();
	UFUNCTION()
	virtual void ResumeTracing();

	UFUNCTION()
	virtual void ProcessTrace();
	virtual void ProcessTrace_Precise(FInteractionTraceDataV2& InteractionTraceData);
	virtual void ProcessTrace_Loose(FInteractionTraceDataV2& InteractionTraceData);

	virtual bool CanTrace() const;

protected:
	
	virtual bool CanInteract() const override;
	virtual void SetState(const EInteractorStateV2 NewState) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Interaction")
	void OnTraceTypeChangedEvent(const ETraceType& NewType);

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
	 * Timer Handle.
	 * Won't display any values in Blueprints.
	 */
	UPROPERTY(VisibleAnywhere, Category="Interaction|Read Only")
	FTimerHandle Timer_Ticking;

protected:
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FTraceTypeChanged OnTraceTypeChanged;

#if WITH_EDITOR

protected:

	virtual void DrawTracingDebugStart(FInteractionTraceDataV2& InteractionTraceData) const;
	virtual void DrawTracingDebugEnd(FInteractionTraceDataV2& InteractionTraceData) const;
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;

#endif
};