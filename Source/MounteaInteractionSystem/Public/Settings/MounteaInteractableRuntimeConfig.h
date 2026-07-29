// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "MounteaInteractableRuntimeConfig.generated.h"

/**
 * Runtime configuration for Interactable components.
 * Referenced as a soft pointer from UMounteaInteractionSystemSettings.
 */
UCLASS(BlueprintType, meta=(DisplayName="Mountea Interactable Runtime Config"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableRuntimeConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Interaction duration in seconds. -1 = instant (Press behavior). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable|Timing", meta=(Units="s", UIMin=-1, ClampMin=-1))
	float DefaultInteractionPeriod = 3.f;

	/** State interactables boot into. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable|State")
	EInteractableStateV3 DefaultInteractableState = EInteractableStateV3::EIS_Awake;

	/** How mesh/collision components are automatically discovered for highlighting and collision. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable|Setup")
	ESetupType DefaultSetupType = ESetupType::EST_Quick;

	/** Collision channel interactable primitives respond ECR_Overlap on.
	 *  Must match OverlapCollisionChannelName in the interactor runtime config. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable|Collision",
		meta=(NoResetToDefault))
	FName OverlapCollisionChannelName = "InteractionOverlap";

	/** Whether interactables highlight their mesh components when focused. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable|Highlight")
	bool bDefaultHighlightEnabled = true;

	/** Highlight setup defaults (type + material or stencil). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable|Highlight")
	FInteractionHighlightSetup DefaultHighlightSetup;

	/** Whether the interactable can be re-triggered (Cycled) or fires exactly once (Once). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable|Lifecycle")
	EInteractableLifecycle DefaultLifecycleMode = EInteractableLifecycle::EIL_Cycled;

	/** Cooldown period (seconds) between cycles when DefaultLifecycleMode is Cycled. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable|Lifecycle",
		meta=(Units="s", UIMin=0.1, ClampMin=0.1, EditCondition="DefaultLifecycleMode==EInteractableLifecycle::EIL_Cycled"))
	float DefaultCooldownPeriod = 3.f;

	/** Priority weight for Tier 1 actor focus selection. Higher weight = preferred focus candidate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable|Priority", meta=(UIMin=-1, ClampMin=-1))
	int32 DefaultInteractableWeight = 1;

	/** Maximum number of interactors that can occupy slots simultaneously (N-interactor multiplayer). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable|Multiplayer", meta=(UIMin=1, ClampMin=1))
	int32 DefaultMaxInteractors = 1;

	/** Attention and Interaction range thresholds used for widget mode transitions and focus scoring. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable|Ranges")
	FInteractableWidgetRanges DefaultWidgetRanges;
};
