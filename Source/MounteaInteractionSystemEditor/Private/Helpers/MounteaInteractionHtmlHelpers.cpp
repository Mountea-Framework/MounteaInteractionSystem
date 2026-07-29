// All rights reserved Dominik Morse (Pavlicek) 2024

#include "Helpers/MounteaInteractionHtmlHelpers.h"

#include "Internationalization/Regex.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionHtmlHelpers"

namespace
{
	FString EscapeHtml(const FString& text)
	{
		FString out = text;
		out = out.Replace(TEXT("&"),  TEXT("&amp;"),  ESearchCase::CaseSensitive);
		out = out.Replace(TEXT("<"),  TEXT("&lt;"),   ESearchCase::CaseSensitive);
		out = out.Replace(TEXT(">"),  TEXT("&gt;"),   ESearchCase::CaseSensitive);
		out = out.Replace(TEXT("\""), TEXT("&quot;"), ESearchCase::CaseSensitive);
		return out;
	}

	FString FormatInline(const FString& rawText)
	{
		FString text = rawText;

		// [text](url) → <a href="url">text</a>
		{
			const FRegexPattern linkPattern(TEXT("\\[([^\\]]+)\\]\\(([^)]+)\\)"));
			FRegexMatcher matcher(linkPattern, text);
			FString rebuilt;
			int32 lastEnd = 0;
			while (matcher.FindNext())
			{
				rebuilt += text.Mid(lastEnd, matcher.GetMatchBeginning() - lastEnd);
				rebuilt += FString::Printf(TEXT("<a href=\"%s\">%s</a>"),
					*matcher.GetCaptureGroup(2), *matcher.GetCaptureGroup(1));
				lastEnd = matcher.GetMatchEnding();
			}
			rebuilt += text.Mid(lastEnd);
			text = rebuilt;
		}

		// `code` → <code>code</code>
		{
			const FRegexPattern codePattern(TEXT("`([^`]+)`"));
			FRegexMatcher matcher(codePattern, text);
			FString rebuilt;
			int32 lastEnd = 0;
			while (matcher.FindNext())
			{
				rebuilt += text.Mid(lastEnd, matcher.GetMatchBeginning() - lastEnd);
				rebuilt += FString::Printf(TEXT("<code>%s</code>"), *matcher.GetCaptureGroup(1));
				lastEnd = matcher.GetMatchEnding();
			}
			rebuilt += text.Mid(lastEnd);
			text = rebuilt;
		}

		// ***bold-italic*** → <strong><em>...</em></strong>
		{
			const FRegexPattern boldItalicPattern(TEXT("\\*\\*\\*([^*]+)\\*\\*\\*"));
			FRegexMatcher matcher(boldItalicPattern, text);
			FString rebuilt;
			int32 lastEnd = 0;
			while (matcher.FindNext())
			{
				rebuilt += text.Mid(lastEnd, matcher.GetMatchBeginning() - lastEnd);
				rebuilt += FString::Printf(TEXT("<strong><em>%s</em></strong>"), *matcher.GetCaptureGroup(1));
				lastEnd = matcher.GetMatchEnding();
			}
			rebuilt += text.Mid(lastEnd);
			text = rebuilt;
		}

		// **bold** → <strong>...</strong>
		{
			const FRegexPattern boldPattern(TEXT("\\*\\*([^*]+)\\*\\*"));
			FRegexMatcher matcher(boldPattern, text);
			FString rebuilt;
			int32 lastEnd = 0;
			while (matcher.FindNext())
			{
				rebuilt += text.Mid(lastEnd, matcher.GetMatchBeginning() - lastEnd);
				rebuilt += FString::Printf(TEXT("<strong>%s</strong>"), *matcher.GetCaptureGroup(1));
				lastEnd = matcher.GetMatchEnding();
			}
			rebuilt += text.Mid(lastEnd);
			text = rebuilt;
		}

		// *italic* → <em>...</em>
		{
			const FRegexPattern italicPattern(TEXT("\\*([^*]+)\\*"));
			FRegexMatcher matcher(italicPattern, text);
			FString rebuilt;
			int32 lastEnd = 0;
			while (matcher.FindNext())
			{
				rebuilt += text.Mid(lastEnd, matcher.GetMatchBeginning() - lastEnd);
				rebuilt += FString::Printf(TEXT("<em>%s</em>"), *matcher.GetCaptureGroup(1));
				lastEnd = matcher.GetMatchEnding();
			}
			rebuilt += text.Mid(lastEnd);
			text = rebuilt;
		}

		// ~~strikethrough~~ → <del>...</del>
		{
			const FRegexPattern strikePattern(TEXT("~~([^~]+)~~"));
			FRegexMatcher matcher(strikePattern, text);
			FString rebuilt;
			int32 lastEnd = 0;
			while (matcher.FindNext())
			{
				rebuilt += text.Mid(lastEnd, matcher.GetMatchBeginning() - lastEnd);
				rebuilt += FString::Printf(TEXT("<del>%s</del>"), *matcher.GetCaptureGroup(1));
				lastEnd = matcher.GetMatchEnding();
			}
			rebuilt += text.Mid(lastEnd);
			text = rebuilt;
		}

		return text;
	}
}

bool FMounteaInteractionHtmlHelpers::ConvertMarkdownToHtml(const FString& MarkdownText, FString& OutputHtml)
{
	OutputHtml.Empty();
	if (MarkdownText.IsEmpty())
		return false;

	TArray<FString> lines;
	MarkdownText.ParseIntoArrayLines(lines, false);

	FString result;
	bool bInList = false;
	int32 listDepth = 0;

	auto closeList = [&]()
	{
		while (listDepth > 0)
		{
			result += TEXT("</ul>\n");
			--listDepth;
		}
		bInList = false;
	};

	auto openListDepth = [&](int32 targetDepth)
	{
		while (listDepth < targetDepth)
		{
			result += TEXT("<ul>\n");
			++listDepth;
		}
		while (listDepth > targetDepth)
		{
			result += TEXT("</ul>\n");
			--listDepth;
		}
		bInList = true;
	};

	for (const FString& rawLine : lines)
	{
		const FString line = rawLine.TrimEnd();

		// Headings
		if (line.StartsWith(TEXT("#### ")))
		{
			closeList();
			result += FString::Printf(TEXT("<h4>%s</h4>\n"), *FormatInline(line.RightChop(5)));
			continue;
		}
		if (line.StartsWith(TEXT("### ")))
		{
			closeList();
			result += FString::Printf(TEXT("<h3>%s</h3>\n"), *FormatInline(line.RightChop(4)));
			continue;
		}
		if (line.StartsWith(TEXT("## ")))
		{
			closeList();
			result += FString::Printf(TEXT("<h2>%s</h2>\n"), *FormatInline(line.RightChop(3)));
			continue;
		}
		if (line.StartsWith(TEXT("# ")))
		{
			closeList();
			result += FString::Printf(TEXT("<h1>%s</h1>\n"), *FormatInline(line.RightChop(2)));
			continue;
		}

		// Horizontal rule
		const FString trimmed = line.TrimStartAndEnd();
		if (trimmed == TEXT("---") || trimmed == TEXT("***") || trimmed == TEXT("___"))
		{
			closeList();
			result += TEXT("<hr>\n");
			continue;
		}

		// List items — detect depth by counting leading "  " pairs or leading dashes
		if (line.StartsWith(TEXT("- ")) || line.StartsWith(TEXT("  - ")) || line.StartsWith(TEXT("    - ")))
		{
			int32 depth = 1;
			if (line.StartsWith(TEXT("    - ")))      depth = 3;
			else if (line.StartsWith(TEXT("  - "))) depth = 2;

			const int32 prefixLen = (depth == 1) ? 2 : (depth == 2) ? 4 : 6;

			openListDepth(depth);
			result += FString::Printf(TEXT("<li>%s</li>\n"), *FormatInline(line.RightChop(prefixLen)));
			continue;
		}

		// Empty line
		if (line.IsEmpty())
		{
			closeList();
			continue;
		}

		// Regular paragraph text
		closeList();
		result += FString::Printf(TEXT("<p>%s</p>\n"), *FormatInline(line));
	}

	closeList();
	OutputHtml = result;
	return !OutputHtml.IsEmpty();
}

FString FMounteaInteractionHtmlHelpers::BuildChangelogDocument(const FString& ChangelogBodyHtml)
{
	const FString headerHtml = TEXT(
		"<section class=\"doc-page changelog-page\">"
		"<header>"
		"<h1>What is New</h1>"
		"<p>Thanks for using the Mountea Interaction System. Here are the latest changes.</p>"
		"</header>"
	);

	const FString footerHtml = TEXT("</section>");

	return FString::Printf(
		TEXT("<!DOCTYPE html><html><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body>%s%s%s</body></html>"),
		*headerHtml,
		*ChangelogBodyHtml,
		*footerHtml
	);
}

FString FMounteaInteractionHtmlHelpers::BuildChangelogFallbackMessage()
{
	return TEXT(
		"<p>Unable to download the latest changelog right now. "
		"You can continue working normally — the changelog updates automatically when online.</p>"
		"<p><a href=\"https://github.com/Mountea-Framework/MounteaInteractionSystem/blob/master/CHANGELOG.md\">Open changelog on GitHub</a></p>"
	);
}

#undef LOCTEXT_NAMESPACE
