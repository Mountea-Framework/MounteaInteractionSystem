// All rights reserved Dominik Morse (Pavlicek) 2021

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Components/Interactable/ActorInteractableComponentBase.h"

class FInteractableComponentAssetActions : public FAssetTypeActions_Base
{
public:
	virtual UClass* GetSupportedClass() const override { return UActorInteractableComponentBase::StaticClass(); };
	virtual FText GetName() const override { return INVTEXT("Interactable Component"); };
	virtual FColor GetTypeColor() const override { return FColor::Magenta; };
	virtual uint32 GetCategories() override
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaInteraction"));
		}
	
		return  EAssetTypeCategories::Misc;
	};
};
