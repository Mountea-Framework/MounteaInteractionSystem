// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#pragma once

#include "CoreMinimal.h"

namespace MounteaInteractionBaseTypes
{
	inline const FText InteractorBaseClassesMenuEntry =	FText::FromString(TEXT("1. Base Classes (Interactor)"));
	inline const FText InteractableBaseClassesMenuEntry =	FText::FromString(TEXT("2. Base Classes (Interactable)"));
	inline const FText ConfigurationMenuEntry =			FText::FromString(TEXT("3. Configuration"));
}

namespace MounteaInteractionToolbar
{
	static const FName MounteaSharedMenuName("MounteaFramework.SharedMenu");
	static const FName MounteaSharedSectionName("MounteaFramework");
	static const FName MounteaSharedButtonName("MounteaFrameworkButton");
}