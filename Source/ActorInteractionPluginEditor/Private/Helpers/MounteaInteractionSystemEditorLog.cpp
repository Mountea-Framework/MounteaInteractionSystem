// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Helpers/MounteaInteractionSystemEditorLog.h"

// Log category definition
DEFINE_LOG_CATEGORY(LogActorInteractionEditor);

void PrintInteractionEditorLog(const FString& Message, const FLinearColor Color, const float Duration)
{
	if (GEditor)
	{
		GEditor->AddOnScreenDebugMessage(0, Duration, Color.ToFColor(true), Message);
	}
}