#pragma once
#include "Framework/Text/SlateHyperlinkRun.h"

class AIntPPopup
{
public:
	static void		Register(const FString& Changelog);
	static void		Open(const FString& Changelog);
	static void 	OnBrowserLinkClicked(const FSlateHyperlinkRun::FMetadata& Metadata);

	static void		FormatChangelog(FString& InChangelog);
	static void		FormatTextWithTags(FString& SourceText, const FString& StartMarker, const FString& EndMarker, const FString& StartTag, const FString& EndTag);
};
