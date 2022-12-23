// Fill out your copyright notice in the Description page of Project Settings.

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
