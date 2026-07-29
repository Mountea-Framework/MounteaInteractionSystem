// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "MounteaInteractorRuntimeConfig.generated.h"

class UInputAction;

/**
 * Runtime configuration for Interactor components.
 * Referenced as a soft pointer from UMounteaInteractionSystemSettings.
 */
UCLASS(BlueprintType, meta=(DisplayName="Mountea Interactor Runtime Config"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractorRuntimeConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Default state interactors boot into. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactor|State")
	EInteractorStateV3 DefaultInteractorState = EInteractorStateV3::EIS_Scanning;

	/** Collision channel used for overlap-based interactable detection.
	 *  Must match the channel interactable primitives respond ECR_Overlap on.
	 *  Auto-created on editor startup if not registered in the collision profile. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactor|Detection",
		meta=(NoResetToDefault))
	FName OverlapCollisionChannelName = "InteractionOverlap";

	/** Collision channel used for line-of-sight safety validation.
	 *  World geometry and interactable actors must block this channel.
	 *  Auto-created on editor startup if not registered in the collision profile. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactor|Safety",
		meta=(NoResetToDefault))
	FName TraceCollisionChannelName = "InteractionTrace";

	/** Gameplay tag assigned to interactors by default. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactor|Tags")
	FGameplayTag InteractorTag;

	/** Safety trace settings (line-of-sight validation before interaction). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactor|Safety")
	FSafetyTracingSetup SafetyTracingSetup;

	/** Maximum number of action candidates tracked per focused actor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactor|Detection", meta=(UIMin=1, ClampMin=1))
	int32 MaxCandidatesPerActor = 8;

	/** How often (in seconds) the interactor refreshes its candidate list. Lower = more responsive, higher = better performance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactor|Detection", meta=(Units="s", UIMin=0.016, ClampMin=0.016))
	float CandidateRefreshRate = 0.05f;

	/** Input action used to cycle through the action list (next/previous). Bound by the interactor on BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactor|Input")
	TSoftObjectPtr<UInputAction> CandidateCycleInputAction;
};
