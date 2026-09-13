// All rights reserved Dominik Morse (Pavlicek) 2021

#include "MounteaInteractableRuntimeConfigAssetFactory.h"

#include "AssetToolsModule.h"
#include "Utilities/MounteaInteractionEditorUtilities.h"

#include "Consts/MounteaInteractionEditorConsts.h"
#include "Settings/MounteaInteractableRuntimeConfig.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableRuntimeConfigAssetFactory"

UMounteaInteractableRuntimeConfigAssetFactory::UMounteaInteractableRuntimeConfigAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInteractableRuntimeConfig::StaticClass();

	// Default class
	ParentClass = SupportedClass;
}

bool UMounteaInteractableRuntimeConfigAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Interactable Runtime Config"));

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInteractionEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaInteractableRuntimeConfig::StaticClass());

	if (bPressedOk)
		ParentClass = ChosenClass;

	return bPressedOk;
}

UObject* UMounteaInteractableRuntimeConfigAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaInteractableRuntimeConfig>(InParent, ParentClass, Name, Flags, Context);
}

uint32 UMounteaInteractableRuntimeConfigAssetFactory::GetMenuCategories() const
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaInteraction"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& UMounteaInteractableRuntimeConfigAssetFactory::GetMenuCategorySubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaInteractionBaseTypes::ConfigurationMenuEntry
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
