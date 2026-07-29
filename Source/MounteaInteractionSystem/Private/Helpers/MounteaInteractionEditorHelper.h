// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

struct FEditorHelper
{
	static void DisplayEditorNotification(const FText& DisplayText, SNotificationItem::ECompletionState State, float ExpireDuration = 5.0f, float FadeOutDuration = 2.0f, const FName ImagePropertyName = TEXT("MessageLog.Warning"))
	{
		if (DisplayText.IsEmpty() || State == SNotificationItem::CS_None)
				return;

		FNotificationInfo Info(DisplayText);
		Info.bFireAndForget = true;
		Info.ExpireDuration = ExpireDuration;
		Info.FadeOutDuration = FadeOutDuration;
		Info.Image = FCoreStyle::Get().GetBrush(ImagePropertyName);

		FSlateNotificationManager::Get().AddNotification(Info).Get()->SetCompletionState(State);
	}
};
#endif
