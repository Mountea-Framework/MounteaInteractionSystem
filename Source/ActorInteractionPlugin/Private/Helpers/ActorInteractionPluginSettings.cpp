// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#include "Helpers/ActorInteractionPluginSettings.h"

#include "InputMappingContext.h"
#include "Helpers/ActorInteractionPluginLog.h"
#include "Helpers/MounteaInteractionSettingsConfig.h"
#include "Materials/MaterialInterface.h"

UActorInteractionPluginSettings::UActorInteractionPluginSettings() :
	bEditorDebugEnabled(true),
	WidgetUpdateFrequency(0.1f)
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Interaction System");
}

TSoftClassPtr<UUserWidget> UActorInteractionPluginSettings::GetInteractableDefaultWidgetClass() const
{
	{ return InteractableDefaultWidgetClass; };
}

UMaterialInterface* UActorInteractionPluginSettings::GetDefaultHighlightMaterial() const
{
	if (DefaultInteractionSystemConfig.LoadSynchronous())
	{
		return DefaultInteractionSystemConfig.LoadSynchronous()->InteractableBaseSettings.DefaultHighlightSetup.HighlightMaterial;
	}
	return nullptr;
}

UInputMappingContext* UActorInteractionPluginSettings::GetDefaultInputMappingContext() const
{
	return InteractionInputMapping.LoadSynchronous();
};
