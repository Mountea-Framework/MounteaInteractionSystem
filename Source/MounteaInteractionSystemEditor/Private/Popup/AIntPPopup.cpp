// All rights reserved Dominik Morse (Pavlicek) 2024

#include "AIntPPopup.h"

#include "AIntPPopupConfig.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
#include "Internationalization/Regex.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Slate/MounteaInteractionWebBrowserWrapper.h"
#include "Statics/MounteaInteractionWebPageStatics.h"
#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionSystemPopup"

FMounteaInteractionPluginVersion AIntPPopup::GetPluginVersion()
{
	const FName pluginName = TEXT("MounteaInteractionSystem");
	const TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin(pluginName.ToString());
	if (!plugin.IsValid())
		return FMounteaInteractionPluginVersion("1", "0.0.0.1");

	const FString pluginFilePath = plugin->GetBaseDir() / FString::Printf(TEXT("%s.uplugin"), *pluginName.ToString());

	FString fileContents;
	if (!FFileHelper::LoadFileToString(fileContents, *pluginFilePath))
		return FMounteaInteractionPluginVersion("1", "0.0.0.1");

	TSharedPtr<FJsonObject> jsonObject;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(fileContents);
	if (!FJsonSerializer::Deserialize(reader, jsonObject) || !jsonObject.IsValid())
		return FMounteaInteractionPluginVersion("1", "0.0.0.1");

	const int32 versionNumber = jsonObject->GetIntegerField(TEXT("Version"));
	const FString versionName = jsonObject->GetStringField(TEXT("VersionName"));
	const FString version = FString::Printf(TEXT("%d"), versionNumber);

	return FMounteaInteractionPluginVersion(version, versionName);
}

bool AIntPPopup::IsVersionGreater(const FString& NewVersion, const FString& OldVersion)
{
	TArray<FString> newParts, oldParts;
	NewVersion.ParseIntoArray(newParts, TEXT("."), true);
	OldVersion.ParseIntoArray(oldParts, TEXT("."), true);

	while (newParts.Num() < 4) newParts.Add(TEXT("0"));
	while (oldParts.Num() < 4) oldParts.Add(TEXT("0"));

	for (int32 index = 0; index < 4; ++index)
	{
		const int32 newValue = FCString::Atoi(*newParts[index]);
		const int32 oldValue = FCString::Atoi(*oldParts[index]);
		if (newValue > oldValue) return true;
		if (newValue < oldValue) return false;
	}

	return false;
}

bool AIntPPopup::ExtractVersionFromMarkdown(const FString& ChangelogMarkdown, FString& OutVersion)
{
	OutVersion.Empty();
	if (ChangelogMarkdown.IsEmpty())
		return false;

	const FRegexPattern versionPattern(TEXT("([0-9]+\\.[0-9]+\\.[0-9]+(?:\\.[0-9]+)?)"));

	TArray<FString> lines;
	ChangelogMarkdown.ParseIntoArrayLines(lines, true);

	for (const FString& line : lines)
	{
		const FString trimmedLine = line.TrimStartAndEnd();
		if (!trimmedLine.StartsWith(TEXT("#")))
			continue;

		FRegexMatcher matcher(versionPattern, trimmedLine);
		if (!matcher.FindNext())
			continue;

		OutVersion = matcher.GetCaptureGroup(1);
		return !OutVersion.IsEmpty();
	}

	FRegexMatcher fallbackMatcher(versionPattern, ChangelogMarkdown);
	if (!fallbackMatcher.FindNext())
		return false;

	OutVersion = fallbackMatcher.GetCaptureGroup(1);
	return !OutVersion.IsEmpty();
}

void AIntPPopup::Register(const FString& ChangelogMarkdown, const FString& ChangelogHtml, const FString& ChangelogHtmlPath)
{
	const TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin(TEXT("MounteaInteractionSystem"));
	if (!plugin.IsValid())
		return;

	const FString pluginDirectory = plugin->GetBaseDir();
	const FString configFilePath = pluginDirectory + TEXT("/Config/UpdateConfig.ini");
	const FString normalizedConfigFilePath = FConfigCacheIni::NormalizeConfigIniPath(configFilePath);

	FString currentPluginVersion = GetPluginVersion().PluginVersionName;
	UAIntPPopupConfig* popupConfig = GetMutableDefault<UAIntPPopupConfig>();

	if (FPaths::FileExists(normalizedConfigFilePath))
		popupConfig->LoadConfig(nullptr, *normalizedConfigFilePath);
	else
		popupConfig->SaveConfig(CPF_Config, *normalizedConfigFilePath);

	FString changelogVersion;
	if (ExtractVersionFromMarkdown(ChangelogMarkdown, changelogVersion))
		currentPluginVersion = changelogVersion;

	if (!IsVersionGreater(currentPluginVersion, popupConfig->PluginVersionUpdate))
		return;

	popupConfig->PluginVersionUpdate = currentPluginVersion;
	popupConfig->SaveConfig(CPF_Config, *normalizedConfigFilePath);

	Open(ChangelogHtml, ChangelogHtmlPath);
}

void AIntPPopup::Open(const FString& ChangelogHtml, const FString& ChangelogHtmlPath)
{
	if (!FSlateApplication::Get().CanDisplayWindows())
		return;

	const TSharedRef<SBorder> windowContent = SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
		.Padding(FMargin(0.0f));

	TSharedPtr<SWindow> window = SNew(SWindow)
		.AutoCenter(EAutoCenter::PreferredWorkArea)
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::FixedSize)
		.ClientSize(FVector2D(920, 660))
		.Title(FText::FromString("Mountea Interaction System"))
		.IsTopmostWindow(true)
		[
			windowContent
		];

	TSharedPtr<SMounteaInteractionWebBrowserWrapper> htmlView;

	const TSharedRef<SVerticalBox> innerContent = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(0)
		[
			SNew(SBorder)
			.Padding(0)
			.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
			.BorderBackgroundColor(FLinearColor::Transparent)
			[
				SAssignNew(htmlView, SMounteaInteractionWebBrowserWrapper)
				.InitialURL(TEXT("about:blank"))
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(4)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([]()
				{
					FPlatformProcess::LaunchURL(TEXT("https://www.unrealengine.com/marketplace/en-US/product/ca842c00ec0d4db0b3aad17701e1637b"), nullptr, nullptr);
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.Text(FText::FromString("Leave a review"))
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(8, 0)
			[
				SNew(SButton)
				.OnClicked_Lambda([]()
				{
					FPlatformProcess::LaunchURL(TEXT("https://discord.gg/waYT2cn37z"), nullptr, nullptr);
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.Text(FText::FromString("Support Discord"))
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.OnClicked_Lambda([window]()
				{
					window->RequestDestroyWindow();
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.Text(FText::FromString("Close"))
				]
			]
		];

	windowContent->SetContent(innerContent);
	window = FSlateApplication::Get().AddWindow(window.ToSharedRef());

	if (!htmlView.IsValid())
		return;

	// Load from file when available - gives Chromium a proper file:// origin URL so content renders
	if (!ChangelogHtmlPath.IsEmpty() && FPaths::FileExists(ChangelogHtmlPath))
	{
		FString rawHtml;
		if (FFileHelper::LoadFileToString(rawHtml, *ChangelogHtmlPath))
		{
			const FString htmlWithAssets = MounteaInteractionWebPageStatics::InjectSharedAssets(rawHtml);
			const FString baseUrl = MounteaInteractionWebPageStatics::MakeFileBaseUrl(ChangelogHtmlPath);
			htmlView->LoadString(htmlWithAssets, baseUrl);
			return;
		}
	}

	// Fallback path: construct a file:// base URL from the plugin help directory
	const TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin(TEXT("MounteaInteractionSystem"));
	const FString fallbackRef = plugin.IsValid()
		? plugin->GetBaseDir() / TEXT("Resources/Help/popup.html")
		: ChangelogHtmlPath;

	const FString htmlWithAssets = MounteaInteractionWebPageStatics::InjectSharedAssets(ChangelogHtml);
	htmlView->LoadString(htmlWithAssets, MounteaInteractionWebPageStatics::MakeFileBaseUrl(fallbackRef));
}

#undef LOCTEXT_NAMESPACE
