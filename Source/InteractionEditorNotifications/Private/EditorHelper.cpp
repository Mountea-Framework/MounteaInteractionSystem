// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "EditorHelper.h"
#include "Styling/SlateStyle.h"

#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#endif

void FEditorHelper::DisplayEditorNotification(const FText DisplayText, const SNotificationItem::ECompletionState State, const float ExpireDuration, const float FadeOutDuration, const FName ImagePropertyName)
{
#if WITH_EDITOR
	if (DisplayText.IsEmpty() || State == SNotificationItem::CS_None) return;
	
	
	FNotificationInfo NotificationInfo = FNotificationInfo(DisplayText);
	
	NotificationInfo.bFireAndForget = true;
	NotificationInfo.ExpireDuration = ExpireDuration;
	NotificationInfo.FadeOutDuration = FadeOutDuration;
	NotificationInfo.Image = FCoreStyle::Get().GetBrush(ImagePropertyName);
	
	FSlateNotificationManager::Get().AddNotification(NotificationInfo).Get()->SetCompletionState(State);
#endif
}