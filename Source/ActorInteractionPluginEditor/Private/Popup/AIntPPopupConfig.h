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
