// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "Widgets/SCompoundWidget.h"

class SMounteaInteractionWebBrowserWrapper;
enum class EWebBrowserConsoleLogSeverity;

class SInteractionSystemTutorialPage : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInteractionSystemTutorialPage) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget> CreateNavigationButton(const FText& Label, int32 PageId);
	void SwitchToPage(int32 PageId);
	static FString GetHtmlPath(int32 PageId);
	void HandleConsoleMessage(const FString& Message, const FString& Source, int32 Line, EWebBrowserConsoleLogSeverity Severity);

	TSharedPtr<SMounteaInteractionWebBrowserWrapper> WebBrowser;
	bool bBlockInput = false;
	int32 CurrentPageId = 0;
};
