// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorInteractableData.generated.h"

#define LOCTEXT_NAMESPACE "InteractableData"

/**
 * 
 */
UCLASS(BlueprintType, Category="Interaction")
class ACTORINTERACTIONPLUGIN_API UActorInteractableData : public UDataAsset
{
	GENERATED_BODY()

public:

	/**
	 * Name of the Interactable.
	 * Should define what Object is being interacted with.
	 * Good examples: 'AK-47', 'Book'
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Interaction")
	FText InteractableName = LOCTEXT("InteractableName", "Object");
	/**
	 * Action to Perform.
	 * Should define what Action is requested to perform the Interaction.
	 * Good examples: 'Press', 'Hold'
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Interaction")
	FText InteractableAction = LOCTEXT("InteractableAction", "Press");
	/**
	 * Helper texture.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Interaction")
	UTexture2D* InteractableTexture = nullptr;
	/**
	 * Should contain textures for Keys.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Interaction")
	TMap<FKey, UTexture2D*> KeyTextures;
};

#undef LOCTEXT_NAMESPACE