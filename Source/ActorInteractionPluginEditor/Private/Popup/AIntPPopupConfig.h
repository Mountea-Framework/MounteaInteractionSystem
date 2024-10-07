// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "Engine/DeveloperSettings.h"
#include "AIntPPopupConfig.generated.h"

UCLASS(config = EditorPerProjectUserSettings)
class UAIntPPopupConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UAIntPPopupConfig()
	{
	}

	UPROPERTY(config)
	FString PluginVersionUpdate = "";
	
};
