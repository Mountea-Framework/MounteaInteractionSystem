// Fill out your copyright notice in the Description page of Project Settings.


#include "AIntPCommands.h"

#define LOCTEXT_NAMESPACE "ActorInteractionPluginEditorModule"

void FAIntPCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Support", "Opens Support channel", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control|EModifierKey::Shift|EModifierKey::Alt, EKeys::X));
}

#undef LOCTEXT_NAMESPACE