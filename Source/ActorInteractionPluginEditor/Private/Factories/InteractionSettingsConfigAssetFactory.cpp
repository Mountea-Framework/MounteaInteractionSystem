// All rights reserved Dominik Morse (Pavlicek) 2021


#include "InteractionSettingsConfigAssetFactory.h"

#include "Utilities/ActorInteractionEditorUtilities.h"

#include "Helpers/MounteaInteractionSettingsConfig.h"
#include "Kismet2/KismetEditorUtilities.h"

#define LOCTEXT_NAMESPACE "ActorInteraction"

UInteractionSettingsConfigAssetFactory::UInteractionSettingsConfigAssetFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;

	// true if the associated editor should be opened after creating a new object.
	bEditAfterNew = false;
	SupportedClass = UMounteaInteractionSettingsConfig::StaticClass();

	// Default class
	ParentClass = SupportedClass;
}

bool UInteractionSettingsConfigAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Item Class for new Interactor Component Object"));
	ParentClass = nullptr;

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FActorInteractionEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, SupportedClass);
	if (bPressedOk)
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* UInteractionSettingsConfigAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (ParentClass == nullptr)
		ParentClass = UMounteaInteractionSettingsConfig::StaticClass();
	
	// Something is not right!
	if (ParentClass == nullptr)
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), ParentClass ? FText::FromString(ParentClass->GetName()) : NSLOCTEXT("UnrealEd", "Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a asset based on the class '{0}'."), Args));
		return nullptr;
	}

	return NewObject<UDataAsset>(InParent, ParentClass);
}

#undef LOCTEXT_NAMESPACE