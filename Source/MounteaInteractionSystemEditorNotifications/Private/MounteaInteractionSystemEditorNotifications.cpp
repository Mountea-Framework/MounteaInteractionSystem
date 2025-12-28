#include "MounteaInteractionSystemEditorNotifications.h"

DEFINE_LOG_CATEGORY(MounteaInteractionSystemEditorNotifications);

#define LOCTEXT_NAMESPACE "FMounteaInteractionSystemEditorNotifications"

void FMounteaInteractionSystemEditorNotifications::StartupModule()
{
	UE_LOG(MounteaInteractionSystemEditorNotifications, Warning, TEXT("MounteaInteractionEditorNotifications module has been loaded"));
}

void FMounteaInteractionSystemEditorNotifications::ShutdownModule()
{
	UE_LOG(MounteaInteractionSystemEditorNotifications, Warning, TEXT("MounteaInteractionEditorNotifications module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaInteractionSystemEditorNotifications, MounteaInteractionSystemEditorNotifications)