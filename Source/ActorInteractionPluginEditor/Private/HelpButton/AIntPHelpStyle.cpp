// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "AIntPHelpStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

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
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon12x12(12.0f, 12.0f);
const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef<FSlateStyleSet> FAIntPHelpStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("AIntPHelpStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ActorInteractionPlugin")->GetBaseDir() / TEXT("Resources"));

	Style->Set("AIntPStyleSet.PluginAction", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon40x40));
	Style->Set("AIntPStyleSet.PluginAction.Small", new IMAGE_BRUSH(TEXT("Help_Icon"), Icon20x20));

	Style->Set("AIntPStyleSet.Help.Small", new IMAGE_BRUSH(TEXT("Help_Icon"), Icon16x16));
	Style->Set("AIntPStyleSet.Help", new IMAGE_BRUSH(TEXT("Help_Icon"), Icon40x40));

	Style->Set("AIntPStyleSet.Dialoguer.Small", new IMAGE_BRUSH(TEXT("Dialoguer_Icon"), Icon16x16));
	Style->Set("AIntPStyleSet.Dialoguer", new IMAGE_BRUSH(TEXT("Dialoguer_Icon"), Icon40x40));

	Style->Set("AIntPStyleSet.Wiki.Small", new IMAGE_BRUSH(TEXT("Wiki_Icon"), Icon16x16));
	Style->Set("AIntPStyleSet.Wiki", new IMAGE_BRUSH(TEXT("Wiki_Icon"), Icon40x40));

	Style->Set("AIntPStyleSet.Icon.Close", new IMAGE_BRUSH(TEXT("CloseIcon"), Icon16x16));
	Style->Set("AIntPStyleSet.Icon.SupportDiscord", new IMAGE_BRUSH(TEXT("Help_Icon"), Icon16x16));
	Style->Set("AIntPStyleSet.Icon.HeartIcon", new IMAGE_BRUSH(TEXT("HeartIcon"), Icon16x16));
	Style->Set("AIntPStyleSet.Icon.UBIcon", new IMAGE_BRUSH(TEXT("UnrealBucketIcon"), Icon16x16));
	Style->Set("AIntPStyleSet.Icon.MoneyIcon", new IMAGE_BRUSH(TEXT("MoneyIcon"), Icon16x16));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT