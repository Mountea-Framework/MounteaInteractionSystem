// All rights reserved Dominik Morse 2024

#include "InteractionSystemTutorialPage.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"

void OpenSettingsPage(const FString& SettingsCategory)
{
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");

	if (SettingsCategory == "Mountea")
	{
		SettingsModule.ShowViewer("Project", TEXT("Mountea Framework"), TEXT("Mountea Interaction System"));
	}
	else if (SettingsCategory == "Collision")
	{
		SettingsModule.ShowViewer("Project", TEXT("Engine"), TEXT("Collision"));
	}
}

void OpenContentBrowserFolder(const FString& FolderName)
{
	FString PluginContentPath = TEXT("/ActorInteractionPlugin/") + FolderName;

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().SyncBrowserToFolders({ FolderName });
}

void SInteractionSystemTutorialPage::Construct(const FArguments& InArgs)
{
	FString arrowEmoji = TEXT("➡");
	FString bookEmoji = TEXT("📖");
	FString sparkleEmoji = TEXT("✨");
	FString folderEmoji = TEXT("📂");

	FString TitleMessage = FString(R"(<LargeText>Before You Start</>)");

	FString IntroMessage = FString(R"(
Before you start creating any super complex interactions, there are a few steps that must be done. 
In order to make everything work, you should set up default values and collision channels. 
So let's start with those annoying configurations before so we can focus on interactions later.
)");

	FString CollisionMessage = FString(R"(<RichTextBlock.BoldHighlight>Collision Channels</>

Interaction System can simply work out of the box with collision channels Engine contains by default, but it is much safer to use custom ones.
This way you also have much finer control over your interactable objects.

)") + arrowEmoji + FString(R"( Open <RichTextBlock.Italic>Project Settings</> and open <RichTextBlock.Italic>Collision</> category. 
Here you need to create collision channels for your needs.

)") + sparkleEmoji + FString(R"( You can quickly open the settings here: <a id="settings" href="Collision">Open Collision Settings</>
)") + bookEmoji + FString(R"( Detailed guide here: <a id="browser" href="https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/How-to-Setup-Interaction#collision-channels-">Collision Channels Setup</>
)");

	FString InteractionMessage = FString(R"(<RichTextBlock.BoldHighlight>Interaction Defaults</>

)") + arrowEmoji + FString(R"( Open <RichTextBlock.Italic>Project Settings</> and open <RichTextBlock.Italic>Mountea Interaction System</> category.
<RichTextBlock.BoldHighlight>Default Interaction System Config</> must <RichTextBlock.BoldHighlight>not</> be empty!

)") + sparkleEmoji + FString(R"( You can quickly open the settings here: <a id="settings" href="Mountea">Open Mountea Interaction System Settings</>
)") + bookEmoji + FString(R"( Detailed guide on Wiki page: <a id="browser" href="https://github.com/Mountea-Framework/MounteaInteractionSystem/wiki/How-to-Setup-Interaction#interaction-defaults-">Interaction Defaults Setup</>
)") + folderEmoji + FString(R"( Default Config File is located here: <a id="folder" href="/ActorInteractionPlugin/Config">Open Default Config Folder</>
)");
	
	FString ExampleFolderMessage = FString(R"(
<RichTextBlock.BoldHighlight>Example Content</>

The Mountea Interaction System includes example assets to help you get started.
)") + folderEmoji + FString(R"( Open the example assets folder: <a id="folder" href="/ActorInteractionPlugin/Example">Open Example Folder</>
)");

	ChildSlot
	[
		SNew(SBorder)
		.Padding(10)
		[
			SNew(SScrollBox)

			// Title
			+ SScrollBox::Slot()
			.Padding(10)
			[
				SNew(SRichTextBlock)
				.Text(FText::FromString(TitleMessage))
				.DecoratorStyleSet(&FCoreStyle::Get())
				.AutoWrapText(true)
				+ SRichTextBlock::HyperlinkDecorator(TEXT("browser"), FSlateHyperlinkRun::FOnClick::CreateLambda([](const FSlateHyperlinkRun::FMetadata& Metadata)
				{
					const FString* URL = Metadata.Find(TEXT("href"));
					if (URL) { FPlatformProcess::LaunchURL(**URL, nullptr, nullptr); }
				}))
				+ SRichTextBlock::HyperlinkDecorator(TEXT("settings"), FSlateHyperlinkRun::FOnClick::CreateLambda([](const FSlateHyperlinkRun::FMetadata& Metadata)
				{
					const FString* Category = Metadata.Find(TEXT("href"));
					if (Category) { OpenSettingsPage(*Category); }
				}))
			]

			// Intro
			+ SScrollBox::Slot()
			.Padding(10)
			[
				SNew(SRichTextBlock)
				.Text(FText::FromString(IntroMessage))
				.DecoratorStyleSet(&FCoreStyle::Get())
				.AutoWrapText(true)
			]

			// Image Divider
			+ SScrollBox::Slot()
			.Padding(FMargin(0, 5, 0, 5))
			[
				SNew(SBox)
				.HeightOverride(2)
				[
					SNew(SImage)
					.Image(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.ColorAndOpacity(FLinearColor(1, 1, 1, 0.3))
				]
			]

			// Collision Channels
			+ SScrollBox::Slot()
			.Padding(10)
			[
				SNew(SRichTextBlock)
				.Text(FText::FromString(CollisionMessage))
				.DecoratorStyleSet(&FCoreStyle::Get())
				.AutoWrapText(true)
				+ SRichTextBlock::HyperlinkDecorator(TEXT("browser"), FSlateHyperlinkRun::FOnClick::CreateLambda([](const FSlateHyperlinkRun::FMetadata& Metadata)
				{
					const FString* URL = Metadata.Find(TEXT("href"));
					if (URL) { FPlatformProcess::LaunchURL(**URL, nullptr, nullptr); }
				}))
				+ SRichTextBlock::HyperlinkDecorator(TEXT("settings"), FSlateHyperlinkRun::FOnClick::CreateLambda([](const FSlateHyperlinkRun::FMetadata& Metadata)
				{
					const FString* Category = Metadata.Find(TEXT("href"));
					if (Category) { OpenSettingsPage(*Category); }
				}))
			]

			// Image Divider
			+ SScrollBox::Slot()
			.Padding(FMargin(0, 5, 0, 5))
			[
				SNew(SBox)
				.HeightOverride(2)
				[
					SNew(SImage)
					.Image(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.ColorAndOpacity(FLinearColor(1, 1, 1, 0.3))
				]
			]

			// Interaction Defaults
			+ SScrollBox::Slot()
			.Padding(10)
			[
				SNew(SRichTextBlock)
				.Text(FText::FromString(InteractionMessage))
				.DecoratorStyleSet(&FCoreStyle::Get())
				.AutoWrapText(true)
				+ SRichTextBlock::HyperlinkDecorator(TEXT("browser"), FSlateHyperlinkRun::FOnClick::CreateLambda([](const FSlateHyperlinkRun::FMetadata& Metadata)
				{
					const FString* URL = Metadata.Find(TEXT("href"));
					if (URL) { FPlatformProcess::LaunchURL(**URL, nullptr, nullptr); }
				}))
				+ SRichTextBlock::HyperlinkDecorator(TEXT("settings"), FSlateHyperlinkRun::FOnClick::CreateLambda([](const FSlateHyperlinkRun::FMetadata& Metadata)
				{
					const FString* Category = Metadata.Find(TEXT("href"));
					if (Category) { OpenSettingsPage(*Category); }
				}))
				+ SRichTextBlock::HyperlinkDecorator(TEXT("folder"), FSlateHyperlinkRun::FOnClick::CreateLambda([](const FSlateHyperlinkRun::FMetadata& Metadata)
				{
					const FString* FolderName = Metadata.Find(TEXT("href"));
					if (FolderName)
					{
						OpenContentBrowserFolder(*FolderName);
					}
				}))
			]

			// Image Divider
			+ SScrollBox::Slot()
			.Padding(FMargin(0, 5, 0, 5))
			[
				SNew(SBox)
				.HeightOverride(2)
				[
					SNew(SImage)
					.Image(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.ColorAndOpacity(FLinearColor(1, 1, 1, 0.3))
				]
			]

			// Example Folder
			+ SScrollBox::Slot()
			.Padding(10)
			[
				SNew(SRichTextBlock)
				.Text(FText::FromString(ExampleFolderMessage))
				.DecoratorStyleSet(&FCoreStyle::Get())
				.AutoWrapText(true)
				+ SRichTextBlock::HyperlinkDecorator(TEXT("browser"), FSlateHyperlinkRun::FOnClick::CreateLambda([](const FSlateHyperlinkRun::FMetadata& Metadata)
				{
					const FString* URL = Metadata.Find(TEXT("href"));
					if (URL) { FPlatformProcess::LaunchURL(**URL, nullptr, nullptr); }
				}))
				+ SRichTextBlock::HyperlinkDecorator(TEXT("folder"), FSlateHyperlinkRun::FOnClick::CreateLambda([](const FSlateHyperlinkRun::FMetadata& Metadata)
				{
					const FString* FolderName = Metadata.Find(TEXT("href"));
					if (FolderName)
					{
						OpenContentBrowserFolder(*FolderName);
					}
				}))
			]
		]
	];
}
