// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "AIntPHelpStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Brushes/SlateImageBrush.h"

TSharedPtr< FSlateStyleSet > FAIntPHelpStyle::StyleInstance = nullptr;

void FAIntPHelpStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FAIntPHelpStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FAIntPHelpStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FAIntPHelpStyle::Get()
{
	return *StyleInstance;
}

FName FAIntPHelpStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("AIntPHelpStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define SVG_BRUSH( RelativePath, ... ) FSlateVectorImageBrush( Style->RootToContentDir( RelativePath, TEXT(".svg") ), __VA_ARGS__ )
#define SVG_BRUSH_TINT( RelativePath, Size, Tint ) FSlateVectorImageBrush( Style->RootToContentDir( RelativePath, TEXT(".svg") ), Size, Tint )
#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)

TSharedRef<FSlateStyleSet> FAIntPHelpStyle::Create()
{
	const FVector2D Icon12x12(12.0f, 12.0f);
	const FVector2D Icon14x14(14.0f, 14.0f);
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon24x24(24.0f, 24.0f);
	const FVector2D Icon32x32(32.0f, 32.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);
	const FVector2D Icon128x128(128.f, 128.f);
	const FVector2D Icon200x70(200.f, 70.f);
	
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("AIntPHelpStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MounteaInteractionSystem")->GetBaseDir() / TEXT("Resources"));

	Style->Set("AIntPStyleSet.MounteaLogo", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon40x40));
	Style->Set("AIntPStyleSet.Interaction", new IMAGE_BRUSH(TEXT("InteractorIcon"), Icon40x40));
	Style->Set("AIntPStyleSet.Launcher", new IMAGE_BRUSH(TEXT("MPLIcon"), Icon40x40));

	Style->Set("AIntPStyleSet.PluginAction", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon40x40));

	Style->Set("AIntPStyleSet.Help", new SVG_BRUSH_TINT(TEXT("WebIcons/message-circle-question-mark"), Icon40x40, FLinearColor::White));

	Style->Set("AIntPStyleSet.Dialoguer", new IMAGE_BRUSH(TEXT("Dialoguer_Icon"), Icon40x40));

	Style->Set("AIntPStyleSet.Wiki", new SVG_BRUSH_TINT(TEXT("WebIcons/book-open-text"), Icon40x40, FLinearColor::White));

	Style->Set("AIntPStyleSet.Settings", new SVG_BRUSH_TINT(TEXT("WebIcons/settings"), Icon40x40, FLinearColor::White));

	Style->Set("AIntPStyleSet.Youtube", new SVG_BRUSH_TINT(TEXT("WebIcons/brand-youtube"), Icon40x40, FLinearColor::White));

	Style->Set("AIntPStyleSet.Icon.Close", new SVG_BRUSH_TINT(TEXT("WebIcons/x"), Icon16x16, FLinearColor::White));
	Style->Set("AIntPStyleSet.Icon.SupportDiscord", new SVG_BRUSH_TINT(TEXT("WebIcons/Discord-Symbol-White"), Icon16x16, FLinearColor::White));
	Style->Set("AIntPStyleSet.Icon.HeartIcon", new SVG_BRUSH_TINT(TEXT("WebIcons/heart"), Icon16x16, FLinearColor::White));
	Style->Set("AIntPStyleSet.Icon.UBIcon", new IMAGE_BRUSH(TEXT("UnrealBucketIcon"), Icon16x16));
	Style->Set("AIntPStyleSet.Icon.MoneyIcon", new SVG_BRUSH_TINT(TEXT("WebIcons/hand-coins"), Icon16x16, FLinearColor::White));

	Style->Set("AIntPStyleSet.Tutorial", new SVG_BRUSH_TINT(TEXT("WebIcons/graduation-cap"), Icon40x40, FLinearColor::White));

	Style->Set("AIntPStyleSet.Level", new SVG_BRUSH_TINT(TEXT("WebIcons/mountain-snow"), Icon40x40, FLinearColor::White));
	Style->Set("AIntPStyleSet.Folder", new SVG_BRUSH_TINT(TEXT("WebIcons/folder-open"), Icon40x40, FLinearColor::White));
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef SVG_BRUSH
#undef SVG_BRUSH_TINT
#undef DEFAULT_FONT
#undef TTF_FONT
#undef OTF_FONT
