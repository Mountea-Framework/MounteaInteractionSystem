// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInteractorComponentBase.h"
#include "MounteaInteractorComponentMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMouseTraced);

/**
 * v5.0 Actor Interactor Mouse Component
 *
 * Detection method: world-space ray cast from the current mouse cursor position.
 * Uses APlayerController::DeprojectMousePositionToWorld to build a trace ray,
 * then line-traces against the collision channel to find interactables under the cursor.
 *
 * Intended for top-down and isometric games where interaction is mouse-driven rather
 * than camera-forward. The base's RefreshFocus() handles Tier 1/2 selection.
 *
 * Owner must be (or be possessed by) a Pawn with an associated APlayerController.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactor-Component-Mouse
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName="Interactor Component Mouse"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractorComponentMouse : public UMounteaInteractorComponentBase
{
	GENERATED_BODY()

public:

	UMounteaInteractorComponentMouse();

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ProcessStateChanged() override;
	virtual FString ToString_Implementation() const override;

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Mouse")
	void EnableMouseTracing();
	virtual void EnableMouseTracing_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Mouse")
	void DisableMouseTracing();
	virtual void DisableMouseTracing_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Mouse")
	bool CanMouseTrace() const;
	virtual bool CanMouseTrace_Implementation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|Mouse")
	float GetMouseTraceRange() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Mouse")
	void SetMouseTraceRange(float NewRange);
	virtual void SetMouseTraceRange_Implementation(float NewRange);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Interactor|Mouse")
	float GetMouseTraceInterval() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Interaction|Interactor|Mouse")
	void SetMouseTraceInterval(float NewInterval);
	virtual void SetMouseTraceInterval_Implementation(float NewInterval);

protected:

	UFUNCTION()
	void ProcessMouseTrace();

	/** Called after each trace to diff hits against DetectedSet. */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Interaction|Interactor|Mouse")
	void PostMouseTraced();
	virtual void PostMouseTraced_Implementation();

	/** Returns the PlayerController owning this interactor (via Pawn possession). */
	APlayerController* GetOwnerPlayerController() const;

protected:

	UFUNCTION(Server, Unreliable)
	void SetMouseTraceRange_Server(float NewRange);

	UFUNCTION(Server, Unreliable)
	void SetMouseTraceInterval_Server(float NewInterval);

	/** Reports the local trace's hit actors so the server's own copy of this component can build
	 *  a matching DetectedSet - the server has no independent way to derive "what's under this
	 *  client's mouse cursor" the way it can re-simulate Overlap/Trace detection identically on
	 *  both sides. Unreliable: fires at ~MouseTraceInterval Hz; a dropped report is superseded by
	 *  the next trace tick moments later. */
	UFUNCTION(Server, Unreliable)
	void ReportHitActors_Server(const TArray<AActor*>& HitActors);

public:

	UPROPERTY(BlueprintAssignable, Category="Mountea Interaction|Interactor")
	FOnMouseTraced OnMouseTraced;

protected:

	/** Maximum world distance for the mouse ray cast. */
	UPROPERTY(Replicated, EditAnywhere, Category="Mountea Interaction|Required",
		meta=(Units="cm", UIMin=1, ClampMin=1, DisplayName="Mouse Trace Range (cm)"))
	float MouseTraceRange = 5000.f;

	/** How frequently (seconds) the mouse position is re-evaluated. */
	UPROPERTY(Replicated, EditAnywhere, Category="Mountea Interaction|Required",
		meta=(Units="s", UIMin=0.01, ClampMin=0.01, DisplayName="Mouse Poll Interval (sec)"))
	float MouseTraceInterval = 0.05f;

	UPROPERTY(VisibleAnywhere, Category="Mountea Interaction|Read Only")
	FTimerHandle Timer_MouseTicking;

	/** Interactables detected in the most recent mouse trace (diffed against DetectedSet). */
	TSet<TScriptInterface<IMounteaInteractableInterface>> LastMouseTraceHits;
};
