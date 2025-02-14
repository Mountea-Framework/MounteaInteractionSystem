// All rights reserved Dominik Morse 2024

#pragma once

#include "Widgets/SCompoundWidget.h"

class SInteractionSystemTutorialPage : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInteractionSystemTutorialPage) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FText GetTutorialText() const;
};
