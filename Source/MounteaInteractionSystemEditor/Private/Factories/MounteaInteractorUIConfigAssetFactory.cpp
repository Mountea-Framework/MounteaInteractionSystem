// All rights reserved Dominik Morse (Pavlicek) 2021

#include "MounteaInteractorUIConfigAssetFactory.h"

#include "AssetToolsModule.h"
#include "Utilities/MounteaInteractionEditorUtilities.h"

#include "Consts/MounteaInteractionEditorConsts.h"
#include "Settings/MounteaInteractorUIConfig.h"

#define LOCTEXT_NAMESPACE "MounteaInteractorUIConfigAssetFactory"

UMounteaInteractorUIConfigAssetFactory::UMounteaInteractorUIConfigAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInteractorUIConfig::StaticClass();

	// Default class
	ParentClass = SupportedClass;
}

bool UMounteaInteractorUIConfigAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Interactor UI Config"));

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInteractionEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaInteractorUIConfig::StaticClass());

	if (bPressedOk)
		ParentClass = ChosenClass;

	return bPressedOk;
}

UObject* UMounteaInteractorUIConfigAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaInteractorUIConfig>(InParent, ParentClass, Name, Flags, Context);
}

uint32 UMounteaInteractorUIConfigAssetFactory::GetMenuCategories() const
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaInteraction"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& UMounteaInteractorUIConfigAssetFactory::GetMenuCategorySubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaInteractionBaseTypes::ConfigurationMenuEntry
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
