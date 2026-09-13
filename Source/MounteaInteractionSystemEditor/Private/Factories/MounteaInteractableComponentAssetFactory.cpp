// All rights reserved Dominik Morse (Pavlicek) 2021


#include "MounteaInteractableComponentAssetFactory.h"

#include "AssetToolsModule.h"
#include "Utilities/MounteaInteractionEditorUtilities.h"

#include "Components/Interactable/MounteaInteractableComponentBase.h"
#include "Consts/MounteaInteractionEditorConsts.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Engine/DataTable.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableComponentAssetFactory"

UMounteaInteractableComponentAssetFactory::UMounteaInteractableComponentAssetFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;

	// true if the associated editor should be opened after creating a new object.
	bEditAfterNew = false;
	SupportedClass = UMounteaInteractableComponentBase::StaticClass();

	// Default class
	ParentClass = SupportedClass;
}

bool UMounteaInteractableComponentAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Item Class for new Interactable Component Object"));
	ParentClass = nullptr;

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInteractionEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, SupportedClass);
	if (bPressedOk)
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* UMounteaInteractableComponentAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// Something is not right!
	if (ParentClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), ParentClass ? FText::FromString(ParentClass->GetName()) : NSLOCTEXT("UnrealEd", "Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."), Args));
		return nullptr;
	}
	
	// Create new Blueprint
	return 
	FKismetEditorUtilities::CreateBlueprint(
		ParentClass,
		InParent,
		Name,
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass(),
		NAME_None
	);
}

uint32 UMounteaInteractableComponentAssetFactory::GetMenuCategories() const
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaInteraction"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& UMounteaInteractableComponentAssetFactory::GetMenuCategorySubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaInteractionBaseTypes::InteractableBaseClassesMenuEntry
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE