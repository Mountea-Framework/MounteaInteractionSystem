// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once
#include "Framework/Text/SlateHyperlinkRun.h"

namespace MounteaInteractionSystemPopup
{
	struct FPluginVersion
	{
		FString PluginVersion;
		FString PluginVersionName;

		FPluginVersion(const FString& A, const FString& B) : PluginVersion(A), PluginVersionName(B) {};
	};
}

class AIntPPopup
{
public:
	static void		Register(const FString& Changelog);
	static void		Open(const FString& Changelog);
	static void 	OnBrowserLinkClicked(const FSlateHyperlinkRun::FMetadata& Metadata);

	static void		FormatChangelog(FString& InChangelog);
	static void		FormatTextWithTags(FString& SourceText, const FString& StartMarker, const FString& EndMarker, const FString& StartTag, const FString& EndTag);
	static bool		IsVersionGreater(const FString& NewVersion, const FString& OldVersion);
	
	static MounteaInteractionSystemPopup::FPluginVersion GetPluginVersion();
};
