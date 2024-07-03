// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#include "Helpers/ActorInteractionPluginLog.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

// Log category definition
DEFINE_LOG_CATEGORY(LogActorInteraction);

void PrintLog(const FString& Message, FLinearColor Color, float Duration)
{
	// Ensure GWorld is available
	if (GWorld)
	{
		UKismetSystemLibrary::PrintString(GWorld, Message, true, true, Color, Duration);
	}
}