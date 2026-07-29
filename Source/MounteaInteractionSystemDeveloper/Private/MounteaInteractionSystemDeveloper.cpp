// All rights reserved Dominik Morse (Pavlicek) 2024

#include "MounteaInteractionSystemDeveloper.h"

#if WITH_EDITOR
#include "Factories/MounteaInteractionPinFactory.h"
#include "EdGraphUtilities.h"
#endif

#include "Styling/MounteaInteractionDeveloperStyle.h"

DEFINE_LOG_CATEGORY(MounteaInteractionSystemDeveloper);

#define LOCTEXT_NAMESPACE "FMounteaInteractionSystemDeveloper"

void FMounteaInteractionSystemDeveloper::StartupModule()
{
	UE_LOG(MounteaInteractionSystemDeveloper, Log, TEXT("MounteaInteractionSystemDeveloper module loaded"));

	FMounteaInteractionDeveloperStyle::Initialize();

#if WITH_EDITOR
	PinFactory = MakeShared<FMounteaInteractionPinFactory>();
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);
#endif
}

void FMounteaInteractionSystemDeveloper::ShutdownModule()
{
	FMounteaInteractionDeveloperStyle::Shutdown();

#if WITH_EDITOR
	FEdGraphUtilities::UnregisterVisualPinFactory(PinFactory);
	PinFactory.Reset();
#endif

	UE_LOG(MounteaInteractionSystemDeveloper, Log, TEXT("MounteaInteractionSystemDeveloper module unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaInteractionSystemDeveloper, MounteaInteractionSystemDeveloper)
