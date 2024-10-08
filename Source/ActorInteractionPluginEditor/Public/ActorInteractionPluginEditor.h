// All rights reserved Dominik Morse (Pavlicek) 2021

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Modules/ModuleManager.h"

class FSlateStyleSet;
class FHttpModule;

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

protected:
		
	bool DoesHaveValidTags() const;
	void RefreshGameplayTags();
	void UpdateTagsConfig(const FString& NewContent);
	void CreateTagsConfig(const FString& NewContent);
	
private:

	TSharedPtr<FSlateStyleSet> InteractionSet;	

	TSharedPtr<class FInteractorComponentAssetActions> InteractorComponentAssetActions;
	TSharedPtr<class FInteractableComponentAssetActions> InteractableComponentAssetActions;
	TSharedPtr<class FInteractionSettingsConfigAssetActions> InteractionConfigSettingsAssetAction;
	
public:
	
	/** This function will be bound to Command. */
	void PluginButtonClicked() const;
	void WikiButtonClicked() const;
	void DialoguerButtonClicked() const;

private:

	void RegisterMenus();
	TSharedRef<SWidget> MakeMounteaMenuWidget() const;

	void OnGetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	UFUNCTION() void SendHTTPGet();
	void OnGetResponse_Tags(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	UFUNCTION() void SendHTTPGet_Tags();

private:
	
	TSharedPtr<class FUICommandList> PluginCommands;
	TArray<FName> RegisteredCustomClassLayouts;
	FHttpModule* Http;
};
