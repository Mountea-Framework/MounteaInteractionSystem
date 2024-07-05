// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "AIntPCommands.h"

#define LOCTEXT_NAMESPACE "ActorInteractionPluginEditorModule"

void FAIntPCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Support", "Opens Mountea Framework Support channel", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control|EModifierKey::Shift|EModifierKey::Alt, EKeys::X));
}

#undef LOCTEXT_NAMESPACE