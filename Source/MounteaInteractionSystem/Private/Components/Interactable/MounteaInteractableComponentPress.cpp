// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Components/Interactable/MounteaInteractableComponentPress.h"
#include "Interfaces/MounteaInteractorInterface.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "MounteaInteractableComponentPress"

UMounteaInteractableComponentPress::UMounteaInteractableComponentPress()
{
	// Press is always standalone - no cycling UI, single immediate action
	bStandaloneInteraction = true;
}

void UMounteaInteractableComponentPress::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	Super::InteractionStarted_Implementation(TimeStarted, CausingInteractor);

	// Press completes in the same frame the key is pressed
	FString err;
	Execute_CompleteInteractable(this, err);
}

#if WITH_EDITOR

EDataValidationResult UMounteaInteractableComponentPress::IsDataValid(FDataValidationContext& Context) const
{
	return Super::IsDataValid(Context);
}

#endif

#undef LOCTEXT_NAMESPACE
