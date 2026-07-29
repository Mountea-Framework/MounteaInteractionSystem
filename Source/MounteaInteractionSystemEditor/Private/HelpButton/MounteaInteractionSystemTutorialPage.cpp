// All rights reserved Dominik Morse (Pavlicek) 2024

#include "MounteaInteractionSystemTutorialPage.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "ISettingsModule.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"

#include "Settings/MounteaInteractionEditorSettings.h"
#include "Slate/MounteaInteractionWebBrowserWrapper.h"
#include "Statics/MounteaInteractionWebPageStatics.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionSystemTutorialPage"

void SInteractionSystemTutorialPage::Construct(const FArguments& InArgs)
{
	TSharedRef<SVerticalBox> navigationBox = SNew(SVerticalBox);

	if (const UMounteaInteractionEditorSettings* editorSettings = GetDefault<UMounteaInteractionEditorSettings>())
	{
		TArray<int32> pageIds;
		editorSettings->GetEditorTemplatePages().GetKeys(pageIds);
		pageIds.Sort();

		for (const int32 pageId : pageIds)
		{
			if (const FInteractionEditorPageConfig* pageConfig = editorSettings->GetEditorTemplatePages().Find(pageId))
			{
				navigationBox->AddSlot()
				.AutoHeight()
				[
					CreateNavigationButton(pageConfig->PageTitle, pageId)
				];
			}
		}
	}

	ChildSlot
	[
		SNew(SBorder)
		.BorderBackgroundColor(FLinearColor::Transparent)
		.Padding(0)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(0.25f)
			.Padding(0)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Brushes.Background"))
				.Padding(25)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					[
						navigationBox
					]
				]
			]

			+ SHorizontalBox::Slot()
			.FillWidth(0.75f)
			[
				SNew(SOverlay)

				+ SOverlay::Slot()
				[
					SAssignNew(WebBrowser, SMounteaInteractionWebBrowserWrapper)
					.InitialURL(TEXT("about:blank"))
					.OnConsoleMessage_Raw(this, &SInteractionSystemTutorialPage::HandleConsoleMessage)
					.OnLoadStarted_Lambda([this]()
					{
						bBlockInput = true;
					})
					.OnLoadCompleted_Lambda([this]()
					{
						if (!WebBrowser.IsValid())
							return;

						WebBrowser->ExecuteJavascript(TEXT(
							"(function() {"
							"  var start = window.pageYOffset || document.documentElement.scrollTop;"
							"  var startTime = null;"
							"  var duration = 100;"
							"  function animation(currentTime) {"
							"    if (startTime === null) startTime = currentTime;"
							"    var timeElapsed = currentTime - startTime;"
							"    var progress = Math.min(timeElapsed / duration, 1);"
							"    var ease = 1 - Math.pow(1 - progress, 3);"
							"    window.scrollTo(0, start * (1 - ease));"
							"    if (timeElapsed < duration) requestAnimationFrame(animation);"
							"  }"
							"  requestAnimationFrame(animation);"
							"})();"
						));

						RegisterActiveTimer(0.2f, FWidgetActiveTimerDelegate::CreateLambda(
							[this](double, float) -> EActiveTimerReturnType
							{
								bBlockInput = false;
								return EActiveTimerReturnType::Stop;
							}
						));
					})
				]

				+ SOverlay::Slot()
				[
					SNew(SBox)
					.Visibility_Lambda([this]() { return bBlockInput ? EVisibility::Visible : EVisibility::Collapsed; })
					[
						SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("NoBorder"))
						.Padding(0)
					]
				]
			]
		]
	];

	SwitchToPage(0);
}

TSharedRef<SWidget> SInteractionSystemTutorialPage::CreateNavigationButton(const FText& Label, const int32 PageId)
{
	TSharedPtr<SBorder> borderWidget;

	TSharedRef<SButton> button = SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "NoBorder")
		.OnClicked_Lambda([this, PageId]()
		{
			SwitchToPage(PageId);
			return FReply::Handled();
		})
		.ContentPadding(4)
		[
			SAssignNew(borderWidget, SBorder)
			.Padding(FMargin(10, 8))
			[
				SNew(STextBlock)
				.Text(Label)
				.Font_Lambda([this, PageId]()
				{
					return CurrentPageId == PageId
						? FCoreStyle::GetDefaultFontStyle("Bold", 12)
						: FCoreStyle::GetDefaultFontStyle("Regular", 10);
				})
			]
		];

	borderWidget->SetBorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"));
	borderWidget->SetBorderBackgroundColor(TAttribute<FSlateColor>::CreateLambda([this, PageId, WeakButton = TWeakPtr<SButton>(button)]() -> FSlateColor
	{
		const bool bSelected = (CurrentPageId == PageId);
		const bool bHovered = WeakButton.IsValid() && WeakButton.Pin()->IsHovered();

		if (bSelected && bHovered) return FLinearColor(0.12f, 0.52f, 1.0f, 0.60f);
		if (bSelected)             return FLinearColor(0.10f, 0.42f, 0.90f, 0.45f);
		if (bHovered)              return FLinearColor(1.0f, 1.0f, 1.0f, 0.10f);
		return FLinearColor::Transparent;
	}));

	return button;
}

void SInteractionSystemTutorialPage::SwitchToPage(const int32 PageId)
{
	if (!WebBrowser.IsValid())
		return;

	CurrentPageId = PageId;

	FString htmlContent;
	const FString filePath = GetHtmlPath(PageId);

	if (FFileHelper::LoadFileToString(htmlContent, *filePath))
	{
		const FString htmlWithAssets = MounteaInteractionWebPageStatics::InjectSharedAssets(htmlContent);
		const FString baseUrl = MounteaInteractionWebPageStatics::MakeFileBaseUrl(filePath);
		WebBrowser->LoadString(htmlWithAssets, baseUrl);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("[MounteaInteractionTutorial] Failed to load page %d from: %s"), PageId, *filePath);
}

FString SInteractionSystemTutorialPage::GetHtmlPath(const int32 PageId)
{
	if (const UMounteaInteractionEditorSettings* editorSettings = GetDefault<UMounteaInteractionEditorSettings>())
		return editorSettings->GetEditorTemplatePagePath(PageId);

	return FString();
}

void SInteractionSystemTutorialPage::HandleConsoleMessage(const FString& Message, const FString& Source, int32 Line, EWebBrowserConsoleLogSeverity Severity)
{
	if (!Message.StartsWith(TEXT("MIAIS_LINK:")))
		return;

	const FString data = Message.RightChop(11);

	FString dataType, value;
	if (!data.Split(TEXT(":"), &dataType, &value))
		return;

	if (dataType.Equals(TEXT("external")))
	{
		FPlatformProcess::LaunchURL(*value, nullptr, nullptr);
	}
	else if (dataType.Equals(TEXT("page")))
	{
		SwitchToPage(FCString::Atoi(*value));
	}
	else if (dataType.Equals(TEXT("settings")))
	{
		ISettingsModule& settingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
		if (value.Equals(TEXT("Mountea")))
			settingsModule.ShowViewer(TEXT("Project"), TEXT("Mountea Framework"), TEXT("Mountea Interaction System"));
		else if (value.Equals(TEXT("Collision")))
			settingsModule.ShowViewer(TEXT("Project"), TEXT("Engine"), TEXT("Collision"));
	}
	else if (dataType.Equals(TEXT("folder")))
	{
		FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		contentBrowserModule.Get().SyncBrowserToFolders({ value });
	}
}

#undef LOCTEXT_NAMESPACE
