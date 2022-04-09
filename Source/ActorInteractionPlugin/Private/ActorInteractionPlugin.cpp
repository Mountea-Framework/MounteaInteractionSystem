// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "ActorInteractionPlugin.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FActorInteractionPluginModule"

void FActorInteractionPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Components Thumbnails and Icons 
    {
        InteractorSet = MakeShareable(new FSlateStyleSet("Interactor Style"));
        InteractableSet = MakeShareable(new FSlateStyleSet("Interactable Style"));
    
        const TSharedPtr<IPlugin> PluginPtr = IPluginManager::Get().FindPlugin("ActorInteractionPlugin");
    
        if (PluginPtr.IsValid())
        {
        	const FString ContentDir = IPluginManager::Get().FindPlugin("ActorInteractionPlugin")->GetBaseDir();
        	
        	// Interactor
        	{
        		InteractorSet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InteractorClassThumbnail = new FSlateImageBrush(InteractorSet->RootToContentDir(TEXT("Resources/Interactor_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InteractorClassIcon = new FSlateImageBrush(InteractorSet->RootToContentDir(TEXT("Resources/Interactor_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InteractorClassThumbnail && InteractorClassIcon)
        		{
        			InteractorSet->Set("ClassThumbnail.ActorInteractorComponent", InteractorClassThumbnail);
        			InteractorSet->Set("ClassIcon.ActorInteractorComponent", InteractorClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InteractorSet.Get());
        		}
        	}
    
        	// Interactable
        	{
        		InteractableSet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InteractableClassThumbnail = new FSlateImageBrush(InteractableSet->RootToContentDir(TEXT("Resources/Interactable_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InteractableClassIcon = new FSlateImageBrush(InteractableSet->RootToContentDir(TEXT("Resources/Interactable_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InteractableClassThumbnail && InteractableClassIcon)
        		{
        			InteractableSet->Set("ClassThumbnail.ActorInteractableComponent", InteractableClassThumbnail);
        			InteractableSet->Set("ClassIcon.ActorInteractableComponent", InteractableClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InteractableSet.Get());
        		}
        	}
        }
    }
}

void FActorInteractionPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Components Thumbnails and Icons
	{
		//Unregister styles
		FSlateStyleRegistry::UnRegisterSlateStyle(InteractorSet->GetStyleSetName());
		FSlateStyleRegistry::UnRegisterSlateStyle(InteractableSet->GetStyleSetName());
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FActorInteractionPluginModule, ActorInteractionPlugin)