// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Log category definition
MOUNTEAINTERACTIONSYSTEMEDITOR_API DECLARE_LOG_CATEGORY_EXTERN(LogMounteaInteractionEditor, Display, All);

// Forward declaration of the logging function
void PrintInteractionEditorLog(const FString& Message, const FLinearColor Color, const float Duration);

// Logging macro definitions
#define EDITOR_LOG_INFO(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
UE_LOG(LogMounteaInteractionEditor, Log, TEXT("%s"), *FormattedMessage); \
PrintInteractionEditorLog(FormattedMessage, FLinearColor(0.0f, 1.0f, 0.0f), 5.0f); \
}

#define EDITOR_LOG_WARNING(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
UE_LOG(LogMounteaInteractionEditor, Warning, TEXT("%s"), *FormattedMessage); \
PrintInteractionEditorLog(FormattedMessage, FLinearColor(1.0f, 1.0f, 0.0f), 10.0f); \
}

#define EDITOR_LOG_ERROR(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
UE_LOG(LogMounteaInteractionEditor, Error, TEXT("%s"), *FormattedMessage); \
PrintInteractionEditorLog(FormattedMessage, FLinearColor(1.0f, 0.0f, 0.0f), 15.0f); \
}