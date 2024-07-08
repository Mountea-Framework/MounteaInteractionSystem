// All rights reserved Dominik Morse (Pavlicek) 2021

#pragma once

#include "CoreMinimal.h"

class UK2Node_Event;
/**
 * 
 */
class FActorInteractionEditorUtilities
{
public:

	// Helper function which defined what classes are available to class selector
	static bool PickChildrenOfClass(const FText& TitleText, UClass*& OutChosenClass, UClass* Class);

	// Adding pre-defined Events to BP
	static UK2Node_Event* BlueprintGetOrAddEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature);
	static UK2Node_Event* BlueprintGetEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature);

	// Adding pre-defined Functions to BP
	static UEdGraph* BlueprintGetOrAddFunction(UBlueprint* Blueprint, FName FunctionName, UClass* FunctionClassSignature);
	static UEdGraph* BlueprintGetFunction(UBlueprint* Blueprint, FName FunctionName, UClass* FunctionClassSignature);
};