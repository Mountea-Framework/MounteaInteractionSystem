// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Slate/MounteaInteractionWebBrowserWrapper.h"

#include "SWebBrowser.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionWebBrowserWrapper"

void SMounteaInteractionWebBrowserWrapper::Construct(const FArguments& InArgs)
{
	FOnInteractionConsoleMessageDelegate consoleDelegate = InArgs._OnConsoleMessage;

	ChildSlot
	[
		SAssignNew(WebBrowser, SWebBrowser)
		.InitialURL(InArgs._InitialURL)
		.ShowControls(false)
		.ShowAddressBar(false)
		.ShowInitialThrobber(true)
		.SupportsTransparency(true)
		.SupportsThumbMouseButtonNavigation(false)
		.OnLoadStarted(InArgs._OnLoadStarted)
		.OnLoadCompleted(InArgs._OnLoadCompleted)
		.OnConsoleMessage_Lambda([consoleDelegate](const FString& Message, const FString& Source, int32 Line, EWebBrowserConsoleLogSeverity Severity)
		{
			consoleDelegate.ExecuteIfBound(Message, Source, Line, Severity);
		})
	];
}

void SMounteaInteractionWebBrowserWrapper::LoadString(const FString& Contents, const FString& DummyURL) const
{
	if (WebBrowser.IsValid())
		WebBrowser->LoadString(Contents, DummyURL);
}

void SMounteaInteractionWebBrowserWrapper::ExecuteJavascript(const FString& ScriptText) const
{
	if (WebBrowser.IsValid())
		WebBrowser->ExecuteJavascript(ScriptText);
}

FReply SMounteaInteractionWebBrowserWrapper::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton ||
		MouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton2)
	{
		return FReply::Handled();
	}

	return SCompoundWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
}

#undef LOCTEXT_NAMESPACE
