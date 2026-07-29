// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Statics/MounteaInteractionWebPageStatics.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Settings/MounteaInteractionEditorSettings.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionWebPageStatics"

FString MounteaInteractionWebPageStatics::InjectSharedAssets(const FString& HtmlContent)
{
	const UMounteaInteractionEditorSettings* editorSettings = GetDefault<UMounteaInteractionEditorSettings>();
	if (!editorSettings)
		return HtmlContent;

	FString result = HtmlContent;

	const FString cssPath = editorSettings->GetSharedStylesheetPath();
	if (!cssPath.IsEmpty())
	{
		FString cssContent;
		if (FFileHelper::LoadFileToString(cssContent, *cssPath))
		{
			const FString styleTag = FString::Printf(TEXT("<style>%s</style>"), *cssContent);
			if (!result.Contains(styleTag))
			{
				const int32 headEndPos = result.Find(TEXT("</head>"), ESearchCase::IgnoreCase);
				if (headEndPos != INDEX_NONE)
					result.InsertAt(headEndPos, styleTag);
				else
					result = styleTag + result;
			}
		}
	}

	const FString scriptPath = editorSettings->GetSharedScriptPath();
	if (!scriptPath.IsEmpty())
	{
		FString scriptContent;
		if (FFileHelper::LoadFileToString(scriptContent, *scriptPath))
		{
			const FString scriptTag = FString::Printf(TEXT("<script>%s</script>"), *scriptContent);
			if (!result.Contains(scriptTag))
			{
				const int32 bodyEndPos = result.Find(TEXT("</body>"), ESearchCase::IgnoreCase);
				if (bodyEndPos != INDEX_NONE)
					result.InsertAt(bodyEndPos, scriptTag);
				else
					result += scriptTag;
			}
		}
	}

	return result;
}

FString MounteaInteractionWebPageStatics::MakeFileBaseUrl(const FString& FilePath)
{
	if (FilePath.IsEmpty())
		return TEXT("about:blank");

	return FString::Printf(TEXT("file:///%s/"), *FPaths::GetPath(FilePath).Replace(TEXT("\\"), TEXT("/")));
}

#undef LOCTEXT_NAMESPACE
