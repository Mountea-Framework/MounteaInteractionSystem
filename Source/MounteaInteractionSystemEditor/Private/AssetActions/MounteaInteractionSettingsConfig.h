// All rights reserved Dominik Morse (Pavlicek) 2021

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Helpers/MounteaInteractionSettingsConfig.h"

class FInteractionSettingsConfigAssetActions : public FAssetTypeActions_Base
{
public:
	virtual UClass* GetSupportedClass() const override { return UMounteaInteractionSettingsConfig::StaticClass(); };
	virtual TArray<FText>& GetSubMenus() const override;
	virtual FText GetName() const override { return INVTEXT("Interaction Settings Config"); };
	virtual FColor GetTypeColor() const override { return FColor::Cyan; };
	virtual uint32 GetCategories() override
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaInteraction"));
		}
	
		return  EAssetTypeCategories::Misc;
	};
};

inline TArray<FText>& FInteractionSettingsConfigAssetActions::GetSubMenus() const
{
	static TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("Configuration")
	};
	return AssetTypeActionSubMenu;
}
