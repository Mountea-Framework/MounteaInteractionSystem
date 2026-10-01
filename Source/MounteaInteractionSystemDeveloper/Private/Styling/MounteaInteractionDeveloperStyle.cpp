// All rights reserved Dominik Morse (Pavlicek) 2024

#include "MounteaInteractionDeveloperStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define SVG_BRUSH_TINT(RelativePath, Size, Tint) FSlateVectorImageBrush(StyleRef->RootToContentDir(RelativePath, TEXT(".svg")), Size, Tint)

TSharedPtr<FSlateStyleSet> FMounteaInteractionDeveloperStyle::StyleInstance = nullptr;

TSharedRef<FSlateStyleSet> FMounteaInteractionDeveloperStyle::Create()
{
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);

	TSharedRef<FSlateStyleSet> StyleRef = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	StyleRef->SetContentRoot(IPluginManager::Get().FindPlugin("MounteaInteractionSystem")->GetBaseDir() / TEXT("Resources"));

	StyleRef->Set("MISStyleSet.K2Node_ValidateIcon.small",        new SVG_BRUSH_TINT(TEXT("WebIcons/shield-check"),      Icon20x20, FLinearColor::White));
	StyleRef->Set("MISStyleSet.K2Node_ValidateIcon",              new SVG_BRUSH_TINT(TEXT("WebIcons/shield-check"),      Icon40x40, FLinearColor::White));
	StyleRef->Set("MISStyleSet.K2Node_SetterIcon.small",          new SVG_BRUSH_TINT(TEXT("WebIcons/sunrise"),           Icon20x20, FLinearColor::White));
	StyleRef->Set("MISStyleSet.K2Node_SetterIcon",                new SVG_BRUSH_TINT(TEXT("WebIcons/sunrise"),           Icon40x40, FLinearColor::White));
	StyleRef->Set("MISStyleSet.K2Node_GetterIcon.small",          new SVG_BRUSH_TINT(TEXT("WebIcons/sunset"),            Icon20x20, FLinearColor::White));
	StyleRef->Set("MISStyleSet.K2Node_GetterIcon",                new SVG_BRUSH_TINT(TEXT("WebIcons/sunset"),            Icon40x40, FLinearColor::White));
	StyleRef->Set("MISStyleSet.K2Node_BindingIcon.small",         new SVG_BRUSH_TINT(TEXT("WebIcons/connection"),        Icon20x20, FLinearColor::White));
	StyleRef->Set("MISStyleSet.K2Node_BindingIcon",               new SVG_BRUSH_TINT(TEXT("WebIcons/connection"),        Icon40x40, FLinearColor::White));
	StyleRef->Set("MISStyleSet.K2Node_CommandIcon.small",         new SVG_BRUSH_TINT(TEXT("WebIcons/circuit-board"),     Icon20x20, FLinearColor::White));
	StyleRef->Set("MISStyleSet.K2Node_CommandIcon",               new SVG_BRUSH_TINT(TEXT("WebIcons/circuit-board"),     Icon40x40, FLinearColor::White));

	return StyleRef;
}

void FMounteaInteractionDeveloperStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FMounteaInteractionDeveloperStyle::Shutdown()
{
	if (!StyleInstance.IsValid())
		return;

	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

const ISlateStyle& FMounteaInteractionDeveloperStyle::Get()
{
	return *StyleInstance;
}

FName FMounteaInteractionDeveloperStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MounteaInteractionDeveloperStyle"));
	return StyleSetName;
}

#undef SVG_BRUSH_TINT
