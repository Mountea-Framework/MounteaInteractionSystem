// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Components/ActorInteractorComponentBase.h"

class FInteractorComponentAssetActions : public FAssetTypeActions_Base
{
public:
	virtual UClass* GetSupportedClass() const override { return UActorInteractorComponentBase::StaticClass(); };
	virtual FText GetName() const override { return INVTEXT("Interactor Component"); };
	virtual FColor GetTypeColor() const override { return FColor::Cyan; };
	virtual uint32 GetCategories() override
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("Interaction"));
		}
	
		return  EAssetTypeCategories::Misc;
	};
};
