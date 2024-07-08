// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "AIntPHelpStyle.h"


class FAIntPCommands : public TCommands<FAIntPCommands>
{
public:

	FAIntPCommands()
	: TCommands<FAIntPCommands>(TEXT("AIntPSupport"), NSLOCTEXT("Contexts", "Support", "ActorInteraction Plugin"), NAME_None, FAIntPHelpStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	
	TSharedPtr< FUICommandInfo > PluginAction;
};
