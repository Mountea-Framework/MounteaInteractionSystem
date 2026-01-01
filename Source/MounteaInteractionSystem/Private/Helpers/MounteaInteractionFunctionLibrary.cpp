// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "Helpers/MounteaInteractionFunctionLibrary.h"

#include "Helpers/MounteaInteractionSystemLog.h"
#include "Helpers/MounteaInteractionSystemSettings.h"

#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"
#include "Helpers/MounteaInteractionSettingsConfig.h"


UMounteaInteractionSystemSettings* UMounteaInteractionFunctionLibrary::GetInteractionSettings()
{
	return GetMutableDefault<UMounteaInteractionSystemSettings>();
}

FInteractorBaseSettings UMounteaInteractionFunctionLibrary::GetDefaultInteractorSettings()
{
	const UMounteaInteractionSystemSettings* Settings = GetMutableDefault<UMounteaInteractionSystemSettings>();
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


FInteractableBaseSettings UMounteaInteractionFunctionLibrary::GetDefaultInteractableSettings()
{
	const UMounteaInteractionSystemSettings* Settings = GetMutableDefault<UMounteaInteractionSystemSettings>();
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


float UMounteaInteractionFunctionLibrary::GetDefaultWidgetUpdateFrequency()
{
	if (const UMounteaInteractionSystemSettings* Settings = GetMutableDefault<UMounteaInteractionSystemSettings>())
	{
		return Settings->GetWidgetUpdateFrequency();
	}

	LOG_ERROR(TEXT("[GetDefaultWidgetUpdateFrequency] Cannot load MounteaInteractionSystemSettings! Using hardcoded value."))
	return 0.1f;
}

UDataTable* UMounteaInteractionFunctionLibrary::GetInteractableDefaultDataTable()
{
	if (const UMounteaInteractionSystemSettings* Settings = GetMutableDefault<UMounteaInteractionSystemSettings>())
	{
		if (const auto FoundTable = Settings->GetInteractableDefaultDataTable().LoadSynchronous())
		{
			return FoundTable;
		}
	}

	LOG_ERROR(TEXT("[GetInteractableDefaultDataTable] Cannot load MounteaInteractionSystemSettings! Using null value."))
	return nullptr;
}

TSubclassOf<UUserWidget> UMounteaInteractionFunctionLibrary::GetInteractableDefaultWidgetClass()
{
	if (const UMounteaInteractionSystemSettings* Settings = GetMutableDefault<UMounteaInteractionSystemSettings>())
	{
		const TSubclassOf<UUserWidget> WidgetClass = Settings->GetInteractableDefaultWidgetClass().LoadSynchronous();
		return WidgetClass;
	}
		
	LOG_ERROR(TEXT("[GetInteractableDefaultWidgetClass] Cannot load MounteaInteractionSystemSettings! Using null value."))
	return nullptr;
}

bool UMounteaInteractionFunctionLibrary::IsEditorDebugEnabled()
{
	if (const UMounteaInteractionSystemSettings* Settings = GetMutableDefault<UMounteaInteractionSystemSettings>())
	{
		return Settings->IsEditorDebugEnabled();
	}
		
	LOG_ERROR(TEXT("[GetInteractableDefaultWidgetClass] Cannot load MounteaInteractionSystemSettings! Using null value."))
	return false;
}
