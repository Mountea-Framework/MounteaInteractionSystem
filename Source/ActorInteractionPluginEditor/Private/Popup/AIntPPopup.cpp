#include "AIntPPopup.h"
#include "AIntPPopupConfig.h"
#include "EditorStyleSet.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "SWebBrowser.h"
#include "Interfaces/IPluginManager.h"

void AIntPPopup::OnBrowserLinkClicked(const FSlateHyperlinkRun::FMetadata& Metadata)
{
	const FString* URL = Metadata.Find(TEXT("href"));

	if (URL)
	{
		FPlatformProcess::LaunchURL(**URL, nullptr, nullptr);
	}
}

void AIntPPopup::Register()
{
	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("ActorInteractionPlugin"))->GetBaseDir();
	const FString UpdatedConfigFile = PluginDirectory + "/Config/UpdateConfig.ini";
	const FString CurrentPluginVersion = "3.0";

	UAIntPPopupConfig* AIntPPopupConfig = GetMutableDefault<UAIntPPopupConfig>();

	if (FPaths::FileExists(UpdatedConfigFile))
	{
		AIntPPopupConfig->LoadConfig(nullptr, *UpdatedConfigFile);
	}
	else
	{
		AIntPPopupConfig->SaveConfig(CPF_Config, *UpdatedConfigFile);
	}

	if (AIntPPopupConfig->PluginVersionUpdate != CurrentPluginVersion)
	{
		AIntPPopupConfig->PluginVersionUpdate = CurrentPluginVersion;
		AIntPPopupConfig->SaveConfig(CPF_Config, *UpdatedConfigFile);

		FCoreDelegates::OnPostEngineInit.AddLambda([]()
		{
			Open();
		});
	}
}

void AIntPPopup::Open()
{
	if (!FSlateApplication::Get().CanDisplayWindows())
	{
		return;
	}

	const TSharedRef<SBorder> WindowContent = SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
			.Padding(FMargin(8.0f, 8.0f));

	TSharedPtr<SWindow> Window = SNew(SWindow)
				.AutoCenter(EAutoCenter::PreferredWorkArea)
				.SupportsMaximize(false)
				.SupportsMinimize(false)
				.SizingRule(ESizingRule::FixedSize)
				.ClientSize(FVector2D(800, 600))
				.Title(FText::FromString("Actor Interaction Plugin"))
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
			.Text(FText::FromString("Actor Interaction Plugin v3.0"))
			.Justification(ETextJustify::Center)
		]
		+ SVerticalBox::Slot()
		  .FillHeight(1.0)
		  .Padding(10)
		[
			SNew(SBorder)
			.Padding(10)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					SNew(SRichTextBlock)
					.Text(FText::FromString(R"(
<LargeText>Hello and thank you for using Actor Interaction Plugin!</>

First thing first, if you've been enjoying using it, it would mean a lot if you could just drop <a id="browser" href="https://bit.ly/AIntP_UE4Marketplace">a small review on the marketplace page</> :).

I also made a paid <a id="browser" href="https://bit.ly/ModularSwordsPack_UE4Marketplace">Modular Sword Pack</>. It's a simple yet powerful tool that allows creating thousands upon thousands of unique swords with a simple click, now with a free upgrade of Modular Scabbard System!
					
But let's keep it short, here are the cool new features (and bugfixes) of version 3.0!

<LargeText>Version 3.0</>

<RichTextBlock.Bold>Features</>

* Add new Interactor Component Base Class implementing <RichTextBlock.Italic>IActorInteractorInterface</>
* Add new child Classes for this new Interactor Base class, replacing existing monolithic <RichTextBlock.Italic>ActorInteractorComponent</> solution
* Add new Interactable Base Component class implementing <RichTextBlock.Italic>IActorInteractableInterface</>
* Add new child Classes for this new Interactable Base class, replacing existing monolithic <RichTextBlock.Italic>ActorInteractableComponent</> solution
* Add new Interactable Widget Interface for easier communication between Widgets and Interactables
* Add Editor billboard so Interactable Components are now easier to spot
* Add new Overlay Materials for 5.1 only

<RichTextBlock.Bold>Bugfixes</>

* Fix missed descriptions
* Add <RichTextBlock.Bold>DEPRECATED</> to old Component Classes
)"))
					.TextStyle(FEditorStyle::Get(), "NormalText")
					.DecoratorStyleSet(&FEditorStyle::Get())
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
				.Text(FText::FromString("Leave a review <3"))
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([]()
				{
					const FString URL = "https://bit.ly/AIntP_UE4Marketplace";
					FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(20, 10))
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.Text(FText::FromString("Support our work"))
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([]()
				{
					const FString URL = "https://bit.ly/AIntP_GitHubDonate";
					FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(20, 10))
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.Text(FText::FromString("Join support Discord"))
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([]()
				{
					const FString URL = "https://discord.gg/2vXWEEN";
					FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(20, 10))
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.Text(FText::FromString("Close window"))
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([Window]()
				{
					Window->RequestDestroyWindow();

					return FReply::Handled();
				})
			]
		];

	WindowContent->SetContent(InnerContent);
	Window = FSlateApplication::Get().AddWindow(Window.ToSharedRef());
}