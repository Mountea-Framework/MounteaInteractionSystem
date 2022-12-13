// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSlateStyleSet;

DECLARE_LOG_CATEGORY_EXTERN(ActorInteractionPluginEditor, All, All);

class FActorInteractionPluginEditor : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// Handle Blueprint Events
	void HandleNewInteractorBlueprintCreated(UBlueprint* Blueprint);
	void HandleNewInteractableBlueprintCreated(UBlueprint* Blueprint);
	
private:

	TSharedPtr<FSlateStyleSet> InteractorComponentSet;
	TSharedPtr<FSlateStyleSet> InteractableComponentSet;
	

	TSharedPtr<class FInteractorComponentAssetActions> InteractorComponentAssetActions;
	TSharedPtr<class FInteractableComponentAssetActions> InteractableComponentAssetActions;
};
