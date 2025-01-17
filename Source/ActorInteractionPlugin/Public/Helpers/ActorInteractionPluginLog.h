// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Log category definition
ACTORINTERACTIONPLUGIN_API DECLARE_LOG_CATEGORY_EXTERN(LogActorInteraction, Display, All);

// Forward declaration of the logging function
void PrintLog(const ELogVerbosity::Type Verbosity, const FString& Message, FLinearColor Color, float Duration);

// Logging macro definitions
#define LOG_INFO(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
PrintLog(ELogVerbosity::Log, FormattedMessage, FLinearColor(0.0f, 1.0f, 0.0f), 5.0f); \
}

#define LOG_WARNING(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
PrintLog(ELogVerbosity::Warning, FormattedMessage, FLinearColor(1.0f, 1.0f, 0.0f), 10.0f); \
}

#define LOG_ERROR(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
PrintLog(ELogVerbosity::Error, FormattedMessage, FLinearColor(1.0f, 0.0f, 0.0f), 15.0f); \
}