// All rights reserved Dominik Morse (Pavlicek) 2021

#include "MounteaInteractorRuntimeConfigAssetFactory.h"

#include "AssetToolsModule.h"
#include "Utilities/MounteaInteractionEditorUtilities.h"

#include "Consts/MounteaInteractionEditorConsts.h"
#include "Settings/MounteaInteractorRuntimeConfig.h"

#define LOCTEXT_NAMESPACE "MounteaInteractorRuntimeConfigAssetFactory"

UMounteaInteractorRuntimeConfigAssetFactory::UMounteaInteractorRuntimeConfigAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInteractorRuntimeConfig::StaticClass();

	// Default class
	ParentClass = SupportedClass;
}

bool UMounteaInteractorRuntimeConfigAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Interactor Runtime Config"));

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInteractionEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaInteractorRuntimeConfig::StaticClass());

	if (bPressedOk)
		ParentClass = ChosenClass;

	return bPressedOk;
}

UObject* UMounteaInteractorRuntimeConfigAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaInteractorRuntimeConfig>(InParent, ParentClass, Name, Flags, Context);
}

uint32 UMounteaInteractorRuntimeConfigAssetFactory::GetMenuCategories() const
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaInteraction"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& UMounteaInteractorRuntimeConfigAssetFactory::GetMenuCategorySubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaInteractionBaseTypes::ConfigurationMenuEntry
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
