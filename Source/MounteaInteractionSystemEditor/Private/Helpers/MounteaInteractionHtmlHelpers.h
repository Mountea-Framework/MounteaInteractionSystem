// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"

class FMounteaInteractionHtmlHelpers
{
public:
	static bool ConvertMarkdownToHtml(const FString& MarkdownText, FString& OutputHtml);
	static FString BuildChangelogDocument(const FString& ChangelogBodyHtml);
	static FString BuildChangelogFallbackMessage();
};
