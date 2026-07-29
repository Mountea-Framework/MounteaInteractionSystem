// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FMounteaInteractionDeveloperStyle
{
public:

	static void Initialize();
	static void Shutdown();

	static const ISlateStyle& Get();
	static FName GetStyleSetName();

	template<typename WidgetStyleType>
	static const WidgetStyleType& GetWidgetStyle(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return StyleInstance->GetWidgetStyle<WidgetStyleType>(PropertyName, Specifier);
	}

	static const FSlateBrush* GetBrush(FName PropertyName)
	{
		return StyleInstance->GetBrush(PropertyName);
	}

private:

	static TSharedPtr<FSlateStyleSet> StyleInstance;

	static TSharedRef<FSlateStyleSet> Create();
};
