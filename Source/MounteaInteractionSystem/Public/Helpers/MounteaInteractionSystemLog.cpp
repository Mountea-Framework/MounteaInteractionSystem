// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#include "MounteaInteractionSystemLog.h"

#include "Helpers/MounteaInteractionSystemLog.h"

#include "MounteaInteractionSystemSettings.h"
#include "MounteaInteractionGeneralDataTypes.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

// Log category definition
DEFINE_LOG_CATEGORY(LogActorInteraction);

void PrintInteractionLog(const ELogVerbosity::Type Verbosity, const FString& Message, FLinearColor Color, float Duration)
{
	if (!GWorld) return;

	bool isVerbosityAllowed = false;
	
	if (const UMounteaInteractionSystemSettings* interactionSettings = GetDefault<UMounteaInteractionSystemSettings>())
	{
		const EMounteaInteractionLoggingVerbosity AllowedLogging = interactionSettings->GetAllowedLoggVerbosity();
		
		switch (Verbosity)
		{
		case ELogVerbosity::Fatal:
		case ELogVerbosity::Error:
			isVerbosityAllowed = EnumHasAnyFlags(AllowedLogging, EMounteaInteractionLoggingVerbosity::Error);
			break;
		case ELogVerbosity::Warning:
		case ELogVerbosity::Verbose:
			isVerbosityAllowed = EnumHasAnyFlags(AllowedLogging, EMounteaInteractionLoggingVerbosity::Warning);
			break;
		case ELogVerbosity::Display:
			isVerbosityAllowed = EnumHasAnyFlags(AllowedLogging, EMounteaInteractionLoggingVerbosity::Info);
			break;
		case ELogVerbosity::VeryVerbose:
			isVerbosityAllowed = false;
			break;
		case ELogVerbosity::Log:
			isVerbosityAllowed = false;
			break;
		default:
			isVerbosityAllowed = false;
			break;
		}
	}
	
	if (isVerbosityAllowed)
	{
#if WITH_EDITOR
		FMsg::Logf(__FILE__, __LINE__, LogActorInteraction.GetCategoryName(), Verbosity, TEXT("%s"), *Message);
#endif
		
		UKismetSystemLibrary::PrintString(GWorld, Message, true, true, Color, Duration);
	}
}