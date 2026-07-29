// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SWebBrowser;
enum class EWebBrowserConsoleLogSeverity;

DECLARE_DELEGATE_FourParams(FOnInteractionConsoleMessageDelegate, const FString&, const FString&, int32, EWebBrowserConsoleLogSeverity);

class SMounteaInteractionWebBrowserWrapper : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMounteaInteractionWebBrowserWrapper)
		: _InitialURL(TEXT("about:blank"))
	{}
		SLATE_ARGUMENT(FString, InitialURL)
		SLATE_EVENT(FSimpleDelegate, OnLoadStarted)
		SLATE_EVENT(FSimpleDelegate, OnLoadCompleted)
		SLATE_EVENT(FOnInteractionConsoleMessageDelegate, OnConsoleMessage)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void LoadString(const FString& Contents, const FString& DummyURL) const;
	void ExecuteJavascript(const FString& ScriptText) const;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
	TSharedPtr<SWebBrowser> WebBrowser;
};
