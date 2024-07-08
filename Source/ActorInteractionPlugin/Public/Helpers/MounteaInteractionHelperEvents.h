// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MounteaInteractionHelperEvents.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputActionConsumed, UInputAction*, ConsumedInput);

UCLASS()
class UMounteaInteractionHelperEvents : public UObject
{
	GENERATED_BODY()

public:
	
};
