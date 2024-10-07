// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FAIntPHelpStyle : public FAppStyle
{
public:

	static void Initialize();

	static void Shutdown();

	/** reloads textures used by slate renderer */
	static void ReloadTextures();

	/** @return The Slate style set for the Actor Interaction Plugin Help Button */
	static const ISlateStyle& Get();

	static const FSlateBrush* GetBrush(FName PropertyName)
	{
		const auto& Style = Get();
		return Style.GetBrush(PropertyName);
	}

	static FName GetStyleSetName();

private:

	static TSharedRef< class FSlateStyleSet > Create();

private:

	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};
