// All rights reserved Dominik Pavlicek 2022.

#include "ActorInteractionPluginEditor.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FActorInteractionPluginEditor"

void FActorInteractionPluginEditor::StartupModule()
{
	{
		InteractorComponentSet = MakeShareable(new FSlateStyleSet("InteractorComponent Style"));
		InteractableComponentSet = MakeShareable(new FSlateStyleSet("InteractableComponent Style"));

		const TSharedPtr<IPlugin> PluginPtr = IPluginManager::Get().FindPlugin("ActorInteractionPlugin");

		if (PluginPtr.IsValid())
		{
			const FString ContentDir = IPluginManager::Get().FindPlugin("ActorInteractionPlugin")->GetBaseDir();
        	
			// Interactor Component
			{
				InteractorComponentSet->SetContentRoot(ContentDir);
        		
				FSlateImageBrush* InteractorComponentClassThumb = new FSlateImageBrush(InteractorComponentSet->RootToContentDir(TEXT("Resources/InteractorIcon_128"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* InteractorComponentClassIcon = new FSlateImageBrush(InteractorComponentSet->RootToContentDir(TEXT("Resources/InteractorIcon_16"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (InteractorComponentClassThumb && InteractorComponentClassIcon)
				{
					InteractorComponentSet->Set("ClassThumbnail.ActorInteractorComponentBase", InteractorComponentClassThumb);
					InteractorComponentSet->Set("ClassIcon.ActorInteractorComponentBase", InteractorComponentClassIcon);
     
					//Register the created style
					FSlateStyleRegistry::RegisterSlateStyle(*InteractorComponentSet.Get());
				}
			}

			// Interactable Component
			{
				InteractableComponentSet->SetContentRoot(ContentDir);
        		
				FSlateImageBrush* InteractableComponentClassThumb = new FSlateImageBrush(InteractableComponentSet->RootToContentDir(TEXT("Resources/InteractableIcon_128"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* InteractableComponentClassIcon = new FSlateImageBrush(InteractableComponentSet->RootToContentDir(TEXT("Resources/InteractableIcon_16"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (InteractableComponentClassThumb && InteractableComponentClassIcon)
				{
					InteractableComponentSet->Set("ClassThumbnail.ActorInteractableComponentBase", InteractableComponentClassThumb);
					InteractableComponentSet->Set("ClassIcon.ActorInteractableComponentBase", InteractableComponentClassIcon);
     
					//Register the created style
					FSlateStyleRegistry::RegisterSlateStyle(*InteractableComponentSet.Get());
				}
			}
		}
	}
}

void FActorInteractionPluginEditor::ShutdownModule()
{
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(InteractorComponentSet->GetStyleSetName());
		FSlateStyleRegistry::UnRegisterSlateStyle(InteractableComponentSet->GetStyleSetName());
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FActorInteractionPluginEditor, ActorInteractionPluginEditor);