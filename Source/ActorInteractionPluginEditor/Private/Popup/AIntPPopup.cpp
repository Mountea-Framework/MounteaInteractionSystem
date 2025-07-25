// All rights reserved Dominik Morse (Pavlicek) 2024

#include "AIntPPopup.h"
#include "AIntPPopupConfig.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "HelpButton/AIntPHelpStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Widgets/Layout/SScaleBox.h"

void AIntPPopup::OnBrowserLinkClicked(const FSlateHyperlinkRun::FMetadata& Metadata)
{
	const FString* URL = Metadata.Find(TEXT("href"));

	if (URL)
	{
		FPlatformProcess::LaunchURL(**URL, nullptr, nullptr);
	}
}

void AIntPPopup::FormatChangelog(FString& InChangelog)
{
	FString partyEmoji = TEXT("🎉");
	FString miracleEmoji = TEXT("✨");

	FString HeaderMessage = partyEmoji + FString(R"(<LargeText>Welcome, and thank you for choosing the Mountea Interaction System!</>)") + TEXT(" ") + partyEmoji;
	FString WelcomeMessage = FString(R"(

We're delighted to have you on board. If you're enjoying your experience, we'd greatly appreciate it if you could take a moment to leave <a id="browser" href="https://www.unrealengine.com/marketplace/en-US/product/ca842c00ec0d4db0b3aad17701e1637b">a quick review on our marketplace page</>. Your feedback means the world to us!

Additionally, I'd like to invite you to check out the <a id="browser" href="https://www.unrealengine.com/marketplace/en-US/product/fbe5d74e46b846f0aeb8ca759e64b71d">Modular Sword Pack</>. This versatile tool allows you to create thousands of unique swords with ease, and now it includes the Modular Scabbard System at no extra cost.

)");

	FString DialoguerMessage = miracleEmoji + FString(R"(<LargeText>We're thrilled to announce a game-changing addition to our toolkit: the new standalone dialogue builder!</>)") + TEXT(" ") + miracleEmoji; 
	FString RestOfMessage =  FString(R"(

You can now easily <a id="browser" href="https://mountea-framework.github.io/MounteaDialoguer/">check it out here</>, a powerful tool that allows you to create, manage, and refine dialogues like never before. This tool isn't just built for Unreal Engine - it's designed to export dialogues for any engine and media format. Seamlessly import your dialogues into Unreal and export them back for other platforms, making your creative process more flexible and efficient than ever!

Now, let's get straight to the exciting new features and bug fixes in this version!
)");
	
	if (InChangelog.IsEmpty())
	{
		const FString InvalidChangelog = FString(R"(
We're sorry, but there was an error retrieving the online Changelog. We apologize for the inconvenience.

You can still access the complete changelog publicly <a id="browser" href="https://github.com/Mountea-Framework/MounteaInteractionSystem/blob/5.4/CHANGELOG.md">on our GitHub</>.

Thank you for your understanding!
)");

		InChangelog = InChangelog.Append(WelcomeMessage).Append(DialoguerMessage).Append(RestOfMessage).Append(InvalidChangelog);
		return;
	}
	
	InChangelog = InChangelog.RightChop(109);
	InChangelog = InChangelog.Replace(TEXT("### Added"),		TEXT("<RichTextBlock.BoldHighlight>Added</>"));
	InChangelog = InChangelog.Replace(TEXT("### Fixed"),		TEXT("<RichTextBlock.BoldHighlight>Fixed</>"));
	InChangelog = InChangelog.Replace(TEXT("### Changed"),		TEXT("<RichTextBlock.BoldHighlight>Changed</>"));
	
	InChangelog = InChangelog.Replace(TEXT("> - - -"),			TEXT("     •"));
	InChangelog = InChangelog.Replace(TEXT("> - -"),			TEXT("   ○"));
	InChangelog = InChangelog.Replace(TEXT("> -"),				TEXT("●"));
	InChangelog = InChangelog.Replace(TEXT("> -"),				TEXT("●"));
	InChangelog = InChangelog.Replace(TEXT(">   -"),			TEXT("   ○"));
	InChangelog = InChangelog.Replace(TEXT(">     -"),			TEXT("     •"));

	FormatTextWithTags(InChangelog, TEXT("***"), TEXT("***"),TEXT("<RichTextBlock.Italic>"),	TEXT("</>"));	
	FormatTextWithTags(InChangelog, TEXT("**"), TEXT("**"),TEXT("<LargeText>"),					TEXT("</>"));
	FormatTextWithTags(InChangelog, TEXT("`"), TEXT("`"),	TEXT("<RichTextBlock.TextHighlight>"),TEXT("</>"));
	FormatTextWithTags(InChangelog, TEXT("*"), TEXT("*"),	TEXT("<RichTextBlock.TextHighlight>"),TEXT("</>"));
	
	const FString TempString = InChangelog;

	InChangelog.Empty();

	InChangelog = InChangelog.Append(HeaderMessage).Append(WelcomeMessage).Append(DialoguerMessage).Append(RestOfMessage).Append(TempString);
}

void AIntPPopup::FormatTextWithTags(FString& SourceText, const FString& StartMarker, const FString& EndMarker, const FString& StartTag, const FString& EndTag)
{
	int32 StartIndex = 0;
	while ((StartIndex = SourceText.Find(StartMarker, ESearchCase::CaseSensitive, ESearchDir::FromStart, StartIndex)) != INDEX_NONE)
	{
		int32 EndIndex = SourceText.Find(StartMarker, ESearchCase::CaseSensitive, ESearchDir::FromStart, StartIndex + StartMarker.Len());
		if (EndIndex != INDEX_NONE)
		{
			FString TextToFormat = SourceText.Mid(StartIndex + StartMarker.Len(), EndIndex - StartIndex - StartMarker.Len());

			FString FormattedText = FString::Printf(TEXT("%s%s%s"), *StartTag, *TextToFormat, *EndTag);
			SourceText = SourceText.Left(StartIndex) + FormattedText + SourceText.Mid(EndIndex + StartMarker.Len());

			StartIndex = StartIndex + FormattedText.Len();
		}
		else
		{
			break;
		}
	}
}

bool AIntPPopup::IsVersionGreater(const FString& NewVersion, const FString& OldVersion)
{
	TArray<FString> NewParts, OldParts;
	NewVersion.ParseIntoArray(NewParts, TEXT("."), true);
	OldVersion.ParseIntoArray(OldParts, TEXT("."), true);
	while (NewParts.Num() < 4) NewParts.Add(TEXT("0"));
	while (OldParts.Num() < 4) OldParts.Add(TEXT("0"));
	for (int32 i = 0; i < 4; ++i)
	{
		int32 NewNum = FCString::Atoi(*NewParts[i]);
		int32 OldNum = FCString::Atoi(*OldParts[i]);
		if (NewNum > OldNum) return true;
		if (NewNum < OldNum) return false;
	}
	return false;
}

MounteaInteractionSystemPopup::FPluginVersion AIntPPopup::GetPluginVersion()
{
	const FName PluginName = TEXT("ActorInteractionPlugin");
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(PluginName.ToString());
	if (!Plugin.IsValid())
	{
		return MounteaInteractionSystemPopup::FPluginVersion("1", "0.0.0.1");
	}

	const FString PluginFilePath = Plugin->GetBaseDir() / FString::Printf(TEXT("%s.uplugin"), *PluginName.ToString());

	FString FileContents;
	if (!FFileHelper::LoadFileToString(FileContents, *PluginFilePath))
	{
		return MounteaInteractionSystemPopup::FPluginVersion("1", "0.0.0.1");
	}
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContents);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return MounteaInteractionSystemPopup::FPluginVersion("1", "0.0.0.1");
	}

	const int32 VersionNumber = JsonObject->GetIntegerField(TEXT("Version"));
	const FString VersionName = JsonObject->GetStringField(TEXT("VersionName"));

	const FString Version = FString::Printf(TEXT("%d"), VersionNumber);

	return MounteaInteractionSystemPopup::FPluginVersion(Version, VersionName);
}

void AIntPPopup::Register(const FString& Changelog)
{
	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("ActorInteractionPlugin"))->GetBaseDir();
	const FString UpdatedConfigFile = PluginDirectory + "/Config/UpdateConfig.ini";
	FString CurrentPluginVersion = GetPluginVersion().PluginVersionName;	
	FString NormalizedConfigFilePath = FConfigCacheIni::NormalizeConfigIniPath(UpdatedConfigFile);

	UAIntPPopupConfig* AIntPPopupConfig = GetMutableDefault<UAIntPPopupConfig>();

	if (FPaths::FileExists(NormalizedConfigFilePath))
	{
		AIntPPopupConfig->LoadConfig(nullptr, *NormalizedConfigFilePath);
	}
	else
	{
		AIntPPopupConfig->SaveConfig(CPF_Config, *NormalizedConfigFilePath);
	}

	// Override Plugin Version from GitHub
	if (!Changelog.IsEmpty())
	{
		FString ChangelogVersion = Changelog.Left(24);
		ChangelogVersion = ChangelogVersion.Right(7);
		
		CurrentPluginVersion = ChangelogVersion;
	}

	if (IsVersionGreater(CurrentPluginVersion, AIntPPopupConfig->PluginVersionUpdate))
	{
		AIntPPopupConfig->PluginVersionUpdate = CurrentPluginVersion;
		AIntPPopupConfig->SaveConfig(CPF_Config, *NormalizedConfigFilePath);

		Open(Changelog);
	}
}

void AIntPPopup::Open(const FString& Changelog)
{
	if (!FSlateApplication::Get().CanDisplayWindows())
	{
		return;
	}

	FString DisplayText = Changelog;
	FormatChangelog(DisplayText);
		
	const TSharedRef<SBorder> WindowContent = SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
			.Padding(FMargin(8.0f, 8.0f));

	TSharedPtr<SWindow> Window = SNew(SWindow)
				.AutoCenter(EAutoCenter::PreferredWorkArea)
				.SupportsMaximize(false)
				.SupportsMinimize(false)
				.SizingRule(ESizingRule::FixedSize)
				.ClientSize(FVector2D(850, 600))
				.Title(FText::FromString("Mountea Interaction System"))
				.IsTopmostWindow(true)
	[
		WindowContent
	];

	const FSlateFontInfo HeadingFont = FCoreStyle::GetDefaultFontStyle("Regular", 24);
	const FSlateFontInfo ContentFont = FCoreStyle::GetDefaultFontStyle("Regular", 12);

	const TSharedRef<SVerticalBox> InnerContent = SNew(SVerticalBox)
		// Default settings example
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(10)
		[
			SNew(STextBlock)
			.Font(HeadingFont)
			.Text(FText::FromString("Mountea Interaction System"))
			.Justification(ETextJustify::Center)
		]
		+ SVerticalBox::Slot()
		  .FillHeight(1.0)
		  .Padding(10)
		[
			SNew(SBorder)
			.Padding(10)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.DarkGroupBorder"))
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					SNew(SRichTextBlock)
					.Text(FText::FromString(DisplayText))
					.TextStyle(FAppStyle::Get(), "NormalText")
					.DecoratorStyleSet(&FAppStyle::Get())
					.AutoWrapText(true)
					+ SRichTextBlock::HyperlinkDecorator(TEXT("browser"), FSlateHyperlinkRun::FOnClick::CreateStatic(&OnBrowserLinkClicked))
				]
			]
		]
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(10)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([]()
				{
					const FString URL = "https://www.unrealengine.com/marketplace/en-US/product/ca842c00ec0d4db0b3aad17701e1637b";
					FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

					return FReply::Handled();
				})
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Leave a review"))
					]

					+ SHorizontalBox::Slot()
					[
						SNew(SSpacer)
						.Size(FVector2D(5, 0))
					]
			
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[						
						SNew(SScaleBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						[
							SNew(SImage)
							.ColorAndOpacity(FLinearColor::Red)
							.Image(FAIntPHelpStyle::GetBrush("AIntPStyleSet.Icon.HeartIcon"))
						]
					]
				]
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(20, 10))
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([]()
				{
					const FString URL = "https://bit.ly/MounteaFramework_Sponsors";
					FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

					return FReply::Handled();
				})
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Support Us"))
					]

					+ SHorizontalBox::Slot()
					[
						SNew(SSpacer)
						.Size(FVector2D(5, 0))
					]
			
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SScaleBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						[
							SNew(SImage)
							.Image(FAIntPHelpStyle::GetBrush("AIntPStyleSet.Icon.MoneyIcon"))
						]
					]
				]
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(20, 10))
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([]()
				{
					const FString URL = "https://discord.gg/waYT2cn37z"; // Interaction Specific Link
					FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

					return FReply::Handled();
				})
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Support Discord"))
					]

					+ SHorizontalBox::Slot()
					[
						SNew(SSpacer)
						.Size(FVector2D(5, 0))
					]
			
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SScaleBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						[
							SNew(SImage)
							.Image(FAIntPHelpStyle::GetBrush("AIntPStyleSet.Icon.SupportDiscord"))
						]
					]
				]
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(20, 10))
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([Window]()
				{
					const FString URL = "https://www.unrealengine.com/marketplace/en-US/product/3ce48046720d4a66b4f804b0d135a820";
					FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

					return FReply::Handled();
				})
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Unreal Bucket"))
					]

					+ SHorizontalBox::Slot()
					[
						SNew(SSpacer)
						.Size(FVector2D(5, 0))
					]
			
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SScaleBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						[
							SNew(SImage)
							.Image(FAIntPHelpStyle::GetBrush("AIntPStyleSet.Icon.UBIcon"))
						]
					]
				]
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(20, 10))
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([Window]()
				{
					Window->RequestDestroyWindow();

					return FReply::Handled();
				})
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Close window"))
					]

					+ SHorizontalBox::Slot()
					[
						SNew(SSpacer)
						.Size(FVector2D(5, 0))
					]
			
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SScaleBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						[
							SNew(SImage)
							.ColorAndOpacity(FLinearColor::Red)
							.Image(FAIntPHelpStyle::GetBrush("AIntPStyleSet.Icon.Close"))
						]
					]
				]
			]
		];

	WindowContent->SetContent(InnerContent);
	Window = FSlateApplication::Get().AddWindow(Window.ToSharedRef());
}