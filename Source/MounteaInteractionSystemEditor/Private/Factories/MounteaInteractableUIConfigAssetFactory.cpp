// All rights reserved Dominik Morse (Pavlicek) 2021

#include "MounteaInteractableUIConfigAssetFactory.h"

#include "AssetToolsModule.h"
#include "Utilities/MounteaInteractionEditorUtilities.h"

#include "Consts/MounteaInteractionEditorConsts.h"
#include "Settings/MounteaInteractableUIConfig.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableUIConfigAssetFactory"

UMounteaInteractableUIConfigAssetFactory::UMounteaInteractableUIConfigAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInteractableUIConfig::StaticClass();

	// Default class
	ParentClass = SupportedClass;
}

bool UMounteaInteractableUIConfigAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Interactable UI Config"));

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInteractionEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaInteractableUIConfig::StaticClass());

	if (bPressedOk)
		ParentClass = ChosenClass;

	return bPressedOk;
}

UObject* UMounteaInteractableUIConfigAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaInteractableUIConfig>(InParent, ParentClass, Name, Flags, Context);
}

uint32 UMounteaInteractableUIConfigAssetFactory::GetMenuCategories() const
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaInteraction"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& UMounteaInteractableUIConfigAssetFactory::GetMenuCategorySubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaInteractionBaseTypes::ConfigurationMenuEntry
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
