// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#include "Helpers/ActorInteractionPluginLog.h"

#include "ActorInteractionPluginSettings.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

// Log category definition
DEFINE_LOG_CATEGORY(LogActorInteraction);

void PrintInteractionLog(const FString& Message, const FLinearColor Color, const float Duration)
{
	auto interactionSettings = GetMutableDefault<UActorInteractionPluginSettings>();
	if (interactionSettings && !interactionSettings->bEditorDebugEnabled)
		return;
	
	if (GWorld)
	{
		UKismetSystemLibrary::PrintString(GWorld, Message, true, true, Color, Duration);
	}
}