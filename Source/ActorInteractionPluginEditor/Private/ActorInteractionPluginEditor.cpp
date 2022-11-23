// All rights reserved Dominik Pavlicek 2022.

#include "ActorInteractionPluginEditor.h"

#include "Interfaces/IPluginManager.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"

#include "Modules/ModuleManager.h"

#include "AssetActions/InteractorComponentAssetActions.h"
#include "AssetActions/InteractableComponentAssetActions.h"

#include "AssetToolsModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Utilities/ActorInteractionEditorUtilities.h"


DEFINE_LOG_CATEGORY(ActorInteractionPluginEditor);

#define LOCTEXT_NAMESPACE "FActorInteractionPluginEditor"

void FActorInteractionPluginEditor::StartupModule()
{
	UE_LOG(ActorInteractionPluginEditor, Warning, TEXT("ActorInteractionPluginEditor module has been loaded"));

	// Register Category
	{
		FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(FName("Interaction"), FText::FromString("Interaction"));
	}

	// Thumbnails and Icons
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

	// Asset Types
	{
		// Register Interactor Component Base
		{
			InteractorComponentAssetActions = MakeShared<FInteractorComponentAssetActions>();
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InteractorComponentAssetActions.ToSharedRef());
		}

		// Register Interactable Component Base
		{
			InteractableComponentAssetActions = MakeShared<FInteractableComponentAssetActions>();
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InteractableComponentAssetActions.ToSharedRef());
		}
	}

	// Register pre-made Events
	{
		// New Interactable
		FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback
		(
			this,
			UActorInteractableComponentBase::StaticClass(),
			FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw(this, &FActorInteractionPluginEditor::HandleNewInteractableBlueprintCreated)
		);

		// New Interactor
		FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback
		(
			this,
			UActorInteractorComponentBase::StaticClass(),
			FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw(this, &FActorInteractionPluginEditor::HandleNewInteractorBlueprintCreated)
		);
	}
}

void FActorInteractionPluginEditor::ShutdownModule()
{
	// Thumbnails and Icons
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(InteractorComponentSet->GetStyleSetName());
		FSlateStyleRegistry::UnRegisterSlateStyle(InteractableComponentSet->GetStyleSetName());
	}

	// Asset Types Cleanup
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		{
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InteractorComponentAssetActions.ToSharedRef());
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InteractableComponentAssetActions.ToSharedRef());
		}
	}

	UE_LOG(ActorInteractionPluginEditor, Warning, TEXT("ActorInteractionPluginEditor module has been unloaded"));
}

void FActorInteractionPluginEditor::HandleNewInteractorBlueprintCreated(UBlueprint* Blueprint)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return;
	}

	Blueprint->bForceFullEditor = true;
	UEdGraph* FunctionGraph = FActorInteractionEditorUtilities::BlueprintGetOrAddFunction
	(
		Blueprint,
		GET_FUNCTION_NAME_CHECKED(UActorInteractorComponentBase, CanInteractEvent),
		UActorInteractorComponentBase::StaticClass()
	);
	if (FunctionGraph)
	{
		Blueprint->LastEditedDocuments.Add(FunctionGraph);
	}
	
	Blueprint->BlueprintCategory = FString("Interaction");
}

void FActorInteractionPluginEditor::HandleNewInteractableBlueprintCreated(UBlueprint* Blueprint)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return;
	}

	Blueprint->bForceFullEditor = true;
	UEdGraph* FunctionGraph = FActorInteractionEditorUtilities::BlueprintGetOrAddFunction
	(
		Blueprint,
		GET_FUNCTION_NAME_CHECKED(UActorInteractableComponentBase, CanInteractEvent),
		UActorInteractableComponentBase::StaticClass()
	);
	if (FunctionGraph)
	{
		Blueprint->LastEditedDocuments.Add(FunctionGraph);
	}

	Blueprint->BlueprintCategory = FString("Interaction");
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FActorInteractionPluginEditor, ActorInteractionPluginEditor);