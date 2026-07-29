// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Helpers/MounteaInteractionFunctionLibrary.h"

#include "Helpers/MounteaInteractionSystemLog.h"
#include "Helpers/MounteaInteractionSystemSettings.h"
#include "Settings/MounteaInteractableUIConfig.h"

#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionFunctionLibrary"

UMounteaInteractionSystemSettings* UMounteaInteractionFunctionLibrary::GetInteractionSettings()
{
	return GetMutableDefault<UMounteaInteractionSystemSettings>();
}

float UMounteaInteractionFunctionLibrary::GetDefaultWidgetUpdateFrequency()
{
	if (const UMounteaInteractionSystemSettings* Settings = GetMutableDefault<UMounteaInteractionSystemSettings>())
	{
		if (const UMounteaInteractableUIConfig* UIConfig = Settings->GetInteractableUIConfig())
		{
			return UIConfig->WidgetUpdateFrequency;
		}
	}

	LOG_ERROR(TEXT("[GetDefaultWidgetUpdateFrequency] Cannot load MounteaInteractionSystemSettings or InteractableUIConfig! Using hardcoded value."))
	return 0.1f;
}

UDataTable* UMounteaInteractionFunctionLibrary::GetInteractableDefaultDataTable()
{
	if (const UMounteaInteractionSystemSettings* Settings = GetMutableDefault<UMounteaInteractionSystemSettings>())
	{
		if (const UMounteaInteractableUIConfig* UIConfig = Settings->GetInteractableUIConfig())
		{
			if (UDataTable* Table = UIConfig->DefaultInteractableDataTable.LoadSynchronous())
			{
				return Table;
			}
		}
	}

	LOG_ERROR(TEXT("[GetInteractableDefaultDataTable] Cannot load InteractableUIConfig! Using null value."))
	return nullptr;
}

TSubclassOf<UUserWidget> UMounteaInteractionFunctionLibrary::GetInteractableDefaultWidgetClass()
{
	if (const UMounteaInteractionSystemSettings* Settings = GetMutableDefault<UMounteaInteractionSystemSettings>())
	{
		if (const UMounteaInteractableUIConfig* UIConfig = Settings->GetInteractableUIConfig())
		{
			return UIConfig->DefaultInteractionWidgetClass.LoadSynchronous();
		}
	}

	LOG_ERROR(TEXT("[GetInteractableDefaultWidgetClass] Cannot load InteractableUIConfig! Using null value."))
	return nullptr;
}

bool UMounteaInteractionFunctionLibrary::IsEditorDebugEnabled()
{
	if (const UMounteaInteractionSystemSettings* Settings = GetMutableDefault<UMounteaInteractionSystemSettings>())
	{
		return Settings->IsEditorDebugEnabled();
	}

	LOG_ERROR(TEXT("[IsEditorDebugEnabled] Cannot load MounteaInteractionSystemSettings! Using default value."))
	return false;
}

#undef LOCTEXT_NAMESPACE
