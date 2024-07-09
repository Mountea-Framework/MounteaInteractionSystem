// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Log category definition
ACTORINTERACTIONPLUGIN_API DECLARE_LOG_CATEGORY_EXTERN(LogActorInteraction, Display, All);

// Forward declaration of the logging function
void PrintInteractionLog(const FString& Message, const FLinearColor Color, const float Duration);

// Logging macro definitions
#define LOG_INFO(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
UE_LOG(LogActorInteraction, Log, TEXT("%s"), *FormattedMessage); \
PrintInteractionLog(FormattedMessage, FLinearColor(0.0f, 1.0f, 0.0f), 5.0f); \
}

#define LOG_WARNING(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
UE_LOG(LogActorInteraction, Warning, TEXT("%s"), *FormattedMessage); \
PrintInteractionLog(FormattedMessage, FLinearColor(1.0f, 1.0f, 0.0f), 10.0f); \
}

#define LOG_ERROR(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
UE_LOG(LogActorInteraction, Error, TEXT("%s"), *FormattedMessage); \
PrintInteractionLog(FormattedMessage, FLinearColor(1.0f, 0.0f, 0.0f), 15.0f); \
}