// All rights reserved Dominik Pavlicek 2022.


#include "Helpers/ActorInteractionFunctionLibrary.h"

#include "Helpers/ActorInteractionPluginLog.h"
#include "Helpers/ActorInteractionPluginSettings.h"

#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"


float UActorInteractionFunctionLibrary::GetDefaultWidgetUpdateFrequency()
{
	if (const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>())
	{
		return Settings->GetWidgetUpdateFrequency();
	}

	AIntP_LOG(Error, TEXT("[GetDefaultWidgetUpdateFrequency] Cannot load ActorInteractionPluginSettings! Using hardcoded value."))
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

	AIntP_LOG(Error, TEXT("[GetInteractableDefaultDataTable] Cannot load ActorInteractionPluginSettings! Using null value."))
	return nullptr;
}

TSubclassOf<UUserWidget> UActorInteractionFunctionLibrary::GetInteractableDefaultWidgetClass()
{
	if (const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>())
	{
		const TSubclassOf<UUserWidget> WidgetClass = Settings->GetInteractableDefaultWidgetClass().LoadSynchronous();
		return WidgetClass;
	}
		
	AIntP_LOG(Error, TEXT("[GetInteractableDefaultWidgetClass] Cannot load ActorInteractionPluginSettings! Using null value."))
	return nullptr;
}

bool UActorInteractionFunctionLibrary::IsEditorDebugEnabled()
{
	if (const UActorInteractionPluginSettings* Settings = GetMutableDefault<UActorInteractionPluginSettings>())
	{
		return Settings->IsEditorDebugEnabled();
	}
		
	AIntP_LOG(Error, TEXT("[GetInteractableDefaultWidgetClass] Cannot load ActorInteractionPluginSettings! Using null value."))
	return false;
}
