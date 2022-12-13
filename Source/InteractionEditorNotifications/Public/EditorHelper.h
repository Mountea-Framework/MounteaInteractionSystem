// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Notifications/SNotificationList.h"

#if WITH_EDITOR
struct INTERACTIONEDITORNOTIFICATIONS_API FEditorHelper
{
	/** Displays editor notification. */
	static void DisplayEditorNotification(FText DisplayText, SNotificationItem::ECompletionState State, float ExpireDuration = 5.0f, float FadeOutDuration = 2.0f , const FName ImagePropertyName = TEXT("MessageLog.Warning"));
};
#endif