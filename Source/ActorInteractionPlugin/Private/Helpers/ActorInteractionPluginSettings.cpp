// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#include "Helpers/ActorInteractionPluginSettings.h"

#include "InputMappingContext.h"
#include "Materials/MaterialInterface.h"

UActorInteractionPluginSettings::UActorInteractionPluginSettings() :
	bEditorDebugEnabled(true),
	WidgetUpdateFrequency(0.1f),
	InteractableDefaultHighlightSetup(FInteractionHighlightSetup())
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
	return InteractableDefaultHighlightMaterial.LoadSynchronous();
}

UInputMappingContext* UActorInteractionPluginSettings::GetDefaultInputMappingContext() const
{
	return InteractionInputMapping.LoadSynchronous();
};
