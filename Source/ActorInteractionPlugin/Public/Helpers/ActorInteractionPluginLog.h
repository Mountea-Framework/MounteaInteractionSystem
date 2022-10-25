// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Log category definition
ACTORINTERACTIONPLUGIN_API DECLARE_LOG_CATEGORY_EXTERN(LogActorInteraction, Display, All);

#define AIP_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogActorInteraction, Verbosity, Format, ##__VA_ARGS__); \
}

#define AIntP_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogActorInteraction, Verbosity, Format, ##__VA_ARGS__); \
}