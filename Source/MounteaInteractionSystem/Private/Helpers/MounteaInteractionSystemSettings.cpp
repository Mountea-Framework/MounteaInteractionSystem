// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#include "Helpers/MounteaInteractionSystemSettings.h"

#include "InputMappingContext.h"
#include "StructUtilsMetadata.h"
#include "Helpers/MounteaInteractionSettingsConfig.h"
#include "Materials/MaterialInterface.h"

UMounteaInteractionSystemSettings::UMounteaInteractionSystemSettings() :
	bEditorDebugEnabled(true),
	LogVerbosity(14),
	WidgetUpdateFrequency(0.1f)
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Interaction System");
}

TSoftClassPtr<UUserWidget> UMounteaInteractionSystemSettings::GetInteractableDefaultWidgetClass() const
{
	{ return InteractableDefaultWidgetClass; };
}

UMaterialInterface* UMounteaInteractionSystemSettings::GetDefaultHighlightMaterial() const
{
	if (DefaultInteractionSystemConfig.LoadSynchronous())
	{
		return DefaultInteractionSystemConfig.LoadSynchronous()->InteractableBaseSettings.DefaultHighlightSetup.HighlightMaterial;
	}
	return nullptr;
}

UInputMappingContext* UMounteaInteractionSystemSettings::GetDefaultInputMappingContext() const
{
	return InteractionInputMapping.LoadSynchronous();
}

EMounteaInteractionLoggingVerbosity UMounteaInteractionSystemSettings::GetAllowedLoggVerbosity() const
{
	return static_cast<EMounteaInteractionLoggingVerbosity>(LogVerbosity);
}