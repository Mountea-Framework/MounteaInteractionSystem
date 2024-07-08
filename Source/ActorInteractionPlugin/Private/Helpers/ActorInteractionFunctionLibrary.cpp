// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "Helpers/ActorInteractionFunctionLibrary.h"

#include "Helpers/ActorInteractionPluginLog.h"
#include "Helpers/ActorInteractionPluginSettings.h"

#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"
#include "Helpers/MounteaInteractionSettingsConfig.h"


UActorInteractionPluginSettings* UActorInteractionFunctionLibrary::GetInteractionSettings()
{
	return GetMutableDefault<UActorInteractionPluginSettings>();
}

FInteractorBaseSettings UActorInteractionFunctionLibrary::GetDefaultInteractorSettings()
{
	const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>();
	if (!Settings)
	{
		LOG_ERROR(TEXT("Failed to get interaction settings: Settings returned nullptr."));
		return FInteractorBaseSettings();
	}

	UMounteaInteractionSettingsConfig* DefaultConfig = Settings->DefaultInteractionSystemConfig.LoadSynchronous();
	if (DefaultConfig == nullptr)
	{
		LOG_ERROR(TEXT("Failed to get interaction settings: DefaultInteractionSystemConfig is nullptr."));
		return FInteractorBaseSettings();
	}
	
	if (!DefaultConfig)
	{
		LOG_ERROR(TEXT("Failed to load DefaultInteractionSystemConfig synchronously."));
		return FInteractorBaseSettings();
	}

	return DefaultConfig->InteractorDefaultSettings;
}


FInteractableBaseSettings UActorInteractionFunctionLibrary::GetDefaultInteractableSettings()
{
	const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>();
	if (!Settings)
	{
		LOG_ERROR(TEXT("Failed to get interaction settings: Settings returned nullptr."));
		return FInteractableBaseSettings();
	}

	UMounteaInteractionSettingsConfig* DefaultConfig = Settings->DefaultInteractionSystemConfig.LoadSynchronous();
	if (DefaultConfig == nullptr)
	{
		LOG_ERROR(TEXT("Failed to get interaction settings: DefaultInteractionSystemConfig is nullptr."));
		return FInteractableBaseSettings();
	}
	
	if (!DefaultConfig)
	{
		LOG_ERROR(TEXT("Failed to load DefaultInteractionSystemConfig synchronously."));
		return FInteractableBaseSettings();
	}

	return DefaultConfig->InteractableBaseSettings;
}


float UActorInteractionFunctionLibrary::GetDefaultWidgetUpdateFrequency()
{
	if (const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>())
	{
		return Settings->GetWidgetUpdateFrequency();
	}

	LOG_ERROR(TEXT("[GetDefaultWidgetUpdateFrequency] Cannot load ActorInteractionPluginSettings! Using hardcoded value."))
	return 0.1f;
}

UDataTable* UActorInteractionFunctionLibrary::GetInteractableDefaultDataTable()
{
	if (const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>())
	{
		if (const auto FoundTable = Settings->GetInteractableDefaultDataTable().LoadSynchronous())
		{
			return FoundTable;
		}
	}

	LOG_ERROR(TEXT("[GetInteractableDefaultDataTable] Cannot load ActorInteractionPluginSettings! Using null value."))
	return nullptr;
}

TSubclassOf<UUserWidget> UActorInteractionFunctionLibrary::GetInteractableDefaultWidgetClass()
{
	if (const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>())
	{
		const TSubclassOf<UUserWidget> WidgetClass = Settings->GetInteractableDefaultWidgetClass().LoadSynchronous();
		return WidgetClass;
	}
		
	LOG_ERROR(TEXT("[GetInteractableDefaultWidgetClass] Cannot load ActorInteractionPluginSettings! Using null value."))
	return nullptr;
}

bool UActorInteractionFunctionLibrary::IsEditorDebugEnabled()
{
	if (const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>())
	{
		return Settings->IsEditorDebugEnabled();
	}
		
	LOG_ERROR(TEXT("[GetInteractableDefaultWidgetClass] Cannot load ActorInteractionPluginSettings! Using null value."))
	return false;
}
