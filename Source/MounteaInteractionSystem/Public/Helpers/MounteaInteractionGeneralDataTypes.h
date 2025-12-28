// All rights reserved Dominik Pavlicek 2023

#pragma once

#include "CoreMinimal.h"
#include "MounteaInteractionGeneralDataTypes.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EMounteaInteractionLoggingVerbosity : uint8
{
	None		= 0			UMETA(hidden),
	// Toggle Info On/Off. Info level provides most basic information. Color is green.
	Info		= 1 << 0,
	// Toggle Warning On/Off. Warning level provides information about issues that might affect interaction, but are not blockers.
	Warning		= 1 << 1,
	// Toggle Error On/Off. Error level provides information about issues that will block interaction.
	Error		= 1 << 2
};
ENUM_CLASS_FLAGS(EMounteaInteractionLoggingVerbosity)
