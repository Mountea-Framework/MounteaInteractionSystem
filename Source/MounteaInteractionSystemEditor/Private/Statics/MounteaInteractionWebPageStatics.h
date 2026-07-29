// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"

namespace MounteaInteractionWebPageStatics
{
	FString InjectSharedAssets(const FString& HtmlContent);
	FString MakeFileBaseUrl(const FString& FilePath);
}
