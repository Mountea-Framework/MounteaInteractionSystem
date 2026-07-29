// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInteractableComponentBase.h"
#include "MounteaInteractableComponentPress.generated.h"

/**
 * v5.0 Actor Interactable Press Component
 *
 * Completes immediately when the interaction key is pressed (InteractionPeriod ignored).
 * InteractionStarted → CompleteInteractable in one frame.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactable-Component-Press
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName="Interactable Component Press"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableComponentPress : public UMounteaInteractableComponentBase
{
	GENERATED_BODY()

public:

	UMounteaInteractableComponentPress();

protected:

	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
