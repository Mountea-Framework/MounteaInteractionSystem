// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Components/ActorInteractableComponentBase.h"

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
			return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("Interaction"));
		}
	
		return  EAssetTypeCategories::Misc;
	};
};
