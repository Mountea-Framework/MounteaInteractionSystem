// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaInteractorUIConfig.generated.h"

class UUserWidget;

/**
 * UI configuration for Interactor components.
 * Controls the attention widget pool managed by UMounteaInteractionWorldSubsystem.
 */
UCLASS(BlueprintType, meta=(DisplayName="Mountea Interactor UI Config"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractorUIConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Number of attention widget actors pre-spawned in the WorldSubsystem pool. Increase if many actors can be in attention range simultaneously. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attention Widget", meta=(UIMin=1, ClampMin=1))
	int32 AttentionWidgetPoolSize = 5;

	/** Widget class used for the attention indicator (shown when actor is in attention range but not interaction range). Must implement IActorInteractionWidget. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attention Widget", meta=(MustImplement="/Script/MounteaInteractionSystem.ActorInteractionWidget"))
	TSoftClassPtr<UUserWidget> DefaultAttentionWidgetClass;

	/** Time in seconds for the attention widget to fade in when a new actor is focused. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attention Widget", meta=(Units="s", UIMin=0.0, ClampMin=0.0))
	float AttentionWidgetFadeInTime = 0.2f;

	/** Time in seconds for the attention widget to fade out when focus is lost or actor enters interaction range. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attention Widget", meta=(Units="s", UIMin=0.0, ClampMin=0.0))
	float AttentionWidgetFadeOutTime = 0.15f;
};
