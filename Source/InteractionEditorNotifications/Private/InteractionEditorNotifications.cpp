#include "InteractionEditorNotifications.h"

DEFINE_LOG_CATEGORY(InteractionEditorNotifications);

#define LOCTEXT_NAMESPACE "FInteractionEditorNotifications"

void FInteractionEditorNotifications::StartupModule()
{
	UE_LOG(InteractionEditorNotifications, Warning, TEXT("InteractionEditorNotifications module has been loaded"));
}

void FInteractionEditorNotifications::ShutdownModule()
{
	UE_LOG(InteractionEditorNotifications, Warning, TEXT("InteractionEditorNotifications module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FInteractionEditorNotifications, InteractionEditorNotifications)