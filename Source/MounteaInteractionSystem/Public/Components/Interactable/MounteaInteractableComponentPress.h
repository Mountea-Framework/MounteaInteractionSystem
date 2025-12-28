// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInteractableComponentBase.h"
#include "MounteaInteractableComponentPress.generated.h"

/**
 * Actor Interactable Press Component
 *
 * Child class of Actor Interactable Base Component.
 * This component requires to press interaction key. Interaction period is ignored.
 * 
 * Implements MounteaInteractableInterface.
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/Actor-Interactable-Component-Press
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component Press"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableComponentPress : public UMounteaInteractableComponentBase
{
	GENERATED_BODY()

public:

	UMounteaInteractableComponentPress();

protected:

	virtual void BeginPlay() override;

	virtual void InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor) override;
	virtual void SetDefaults_Implementation() override;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};	