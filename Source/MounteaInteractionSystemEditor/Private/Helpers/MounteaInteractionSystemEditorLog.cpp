// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Helpers/MounteaInteractionSystemEditorLog.h"

#include "Helpers/MounteaInteractionSystemSettings.h"

// Log category definition
DEFINE_LOG_CATEGORY(LogMounteaInteractionEditor);

void PrintInteractionEditorLog(const FString& Message, const FLinearColor Color, const float Duration)
{
	if (!GEditor)
		return;

	const UMounteaInteractionSystemSettings* settings = GetDefault<UMounteaInteractionSystemSettings>();
	if (settings && settings->IsEditorDebugEnabled())
		GEditor->AddOnScreenDebugMessage(0, Duration, Color.ToFColor(true), Message);
}