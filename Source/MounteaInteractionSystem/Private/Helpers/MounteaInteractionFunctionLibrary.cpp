// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Helpers/MounteaInteractionFunctionLibrary.h"

#include "Helpers/MounteaInteractableStatics.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionFunctionLibrary"

// Deprecated - every function below just redirects to its UMounteaInteractableStatics
// replacement. Kept only so existing customer Blueprints/C++ keep compiling; see the class
// doc comment in the header and Config/DefaultMounteaInteractionSystem.ini's CoreRedirects.

UMounteaInteractionSystemSettings* UMounteaInteractionFunctionLibrary::GetInteractionSettings()
{
	return UMounteaInteractableStatics::GetInteractionSystemSettings();
}

float UMounteaInteractionFunctionLibrary::GetDefaultWidgetUpdateFrequency()
{
	return UMounteaInteractableStatics::GetDefaultWidgetUpdateFrequency();
}

UDataTable* UMounteaInteractionFunctionLibrary::GetInteractableDefaultDataTable()
{
	return UMounteaInteractableStatics::GetInteractableDefaultDataTable();
}

TSubclassOf<UUserWidget> UMounteaInteractionFunctionLibrary::GetInteractableDefaultWidgetClass()
{
	return UMounteaInteractableStatics::GetInteractableDefaultWidgetClass();
}

bool UMounteaInteractionFunctionLibrary::IsEditorDebugEnabled()
{
	return UMounteaInteractableStatics::IsEditorDebugEnabled();
}

#undef LOCTEXT_NAMESPACE
