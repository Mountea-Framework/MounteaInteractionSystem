// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

struct FMounteaInteractionPluginVersion
{
	FString PluginVersion;
	FString PluginVersionName;

	FMounteaInteractionPluginVersion(const FString& A, const FString& B) : PluginVersion(A), PluginVersionName(B) {}
};

class AIntPPopup
{
public:
	static void Register(const FString& ChangelogMarkdown, const FString& ChangelogHtml, const FString& ChangelogHtmlPath);
	static void Open(const FString& ChangelogHtml, const FString& ChangelogHtmlPath);

	static FMounteaInteractionPluginVersion GetPluginVersion();
	static bool IsVersionGreater(const FString& NewVersion, const FString& OldVersion);

private:
	static bool ExtractVersionFromMarkdown(const FString& ChangelogMarkdown, FString& OutVersion);
};
