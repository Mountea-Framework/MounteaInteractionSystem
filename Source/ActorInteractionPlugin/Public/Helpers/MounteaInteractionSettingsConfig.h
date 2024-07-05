// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "InteractionHelpers.h"
#include "Engine/DataAsset.h"
#include "MounteaInteractionSettingsConfig.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), EditInlineNew, meta = (DisplayName = "Mountea Interaction System Settings Configuration"), autoexpandcategories=("Interactor","Interactable"))
class ACTORINTERACTIONPLUGIN_API UMounteaInteractionSettingsConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Interactor")
	FInteractorBaseSettings													InteractorDefaultSettings;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Interactable")
	FInteractableBaseSettings												InteractableBaseSettings;
};
