// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Helpers/MounteaInteractionSystemSettings.h"
#include "Settings/MounteaInteractorRuntimeConfig.h"
#include "Settings/MounteaInteractorUIConfig.h"
#include "Settings/MounteaInteractableRuntimeConfig.h"
#include "Settings/MounteaInteractableUIConfig.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionSystemSettings"

UMounteaInteractionSystemSettings::UMounteaInteractionSystemSettings()
	: bEditorDebugEnabled(true)
	, LogVerbosity(14)
{
	CategoryName = TEXT("Mountea Framework");
	SectionName  = TEXT("Mountea Interaction System");
}

EMounteaInteractionLoggingVerbosity UMounteaInteractionSystemSettings::GetAllowedLoggVerbosity() const
{
	return static_cast<EMounteaInteractionLoggingVerbosity>(LogVerbosity);
}

const UMounteaInteractorRuntimeConfig* UMounteaInteractionSystemSettings::GetInteractorConfig() const
{
	return InteractorConfig.LoadSynchronous();
}

const UMounteaInteractorUIConfig* UMounteaInteractionSystemSettings::GetInteractorUIConfig() const
{
	return InteractorUIConfig.LoadSynchronous();
}

const UMounteaInteractableRuntimeConfig* UMounteaInteractionSystemSettings::GetInteractableConfig() const
{
	return InteractableConfig.LoadSynchronous();
}

const UMounteaInteractableUIConfig* UMounteaInteractionSystemSettings::GetInteractableUIConfig() const
{
	return InteractableUIConfig.LoadSynchronous();
}

#undef LOCTEXT_NAMESPACE
