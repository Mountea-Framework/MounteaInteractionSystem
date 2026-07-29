// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"

class FMounteaInteractionPinFactory : public FGraphPanelPinFactory
{
public:

	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override;
};
