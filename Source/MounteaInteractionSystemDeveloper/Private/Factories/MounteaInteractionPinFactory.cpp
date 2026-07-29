// All rights reserved Dominik Morse (Pavlicek) 2024

#include "MounteaInteractionPinFactory.h"

TSharedPtr<SGraphPin> FMounteaInteractionPinFactory::CreatePin(UEdGraphPin* Pin) const
{
	// Custom pin selectors will be registered here as K2Nodes are implemented (Phase 7)
	return nullptr;
}
