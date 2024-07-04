// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.


#include "Helpers/MounteaInteractionSystemBFL.h"

#include "Kismet/KismetSystemLibrary.h"

UMeshComponent* UMounteaInteractionSystemBFL::FindMeshByTag(const FName Tag, const AActor* Source)
{
	if (!Source) return nullptr;

	TArray<UMeshComponent*> MeshComponents;
	Source->GetComponents(MeshComponents);

	for (const auto& Itr : MeshComponents)
	{
		if (Itr && Itr->ComponentHasTag(Tag))
		{
			return Itr;
		}
	}

	return nullptr;
}

UMeshComponent* UMounteaInteractionSystemBFL::FindMeshByName(const FName Name, const AActor* Source)
{
	if (!Source) return nullptr;

	TArray<UMeshComponent*> MeshComponents;
	Source->GetComponents(MeshComponents);

	for (const auto& Itr : MeshComponents)
	{
		if (Itr && Itr->GetName().Equals(Name.ToString()))
		{
			return Itr;
		}
	}

	return nullptr;
}

UPrimitiveComponent* UMounteaInteractionSystemBFL::FindPrimitiveByTag(const FName Tag, const AActor* Source)
{
	if (!Source) return nullptr;

	TArray<UPrimitiveComponent*> MeshComponents;
	Source->GetComponents(MeshComponents);

	for (const auto& Itr : MeshComponents)
	{
		if (Itr && Itr->ComponentHasTag(Tag))
		{
			return Itr;
		}
	}

	return nullptr;
}

UPrimitiveComponent* UMounteaInteractionSystemBFL::FindPrimitiveByName(const FName Name, const AActor* Source)
{
	if (!Source) return nullptr;

	TArray<UPrimitiveComponent*> MeshComponents;
	Source->GetComponents(MeshComponents);

	for (const auto& Itr : MeshComponents)
	{
		if (Itr && Itr->GetName().Equals(Name.ToString()))
		{
			return Itr;
		}
	}

	return nullptr;
}

UActorInteractionPluginSettings* UMounteaInteractionSystemBFL::GetInteractionSystemSettings()
{
	return GetMutableDefault<UActorInteractionPluginSettings>();
}

bool UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(const UWorld* WorldContext)
{
	return !UKismetSystemLibrary::IsDedicatedServer(WorldContext);
}

FText UMounteaInteractionSystemBFL::ReplaceRegexInText(const FText& SourceText, const TMap<FString, FText>& Replacements)
{
	FString ResultString = SourceText.ToString();

	for (const auto& Replacement : Replacements)
	{
		FString Placeholder = Replacement.Key;
		const FString& ReplacementString = Replacement.Value.ToString();

		// Check if the placeholder is already in the ${action} format
		if (!Placeholder.StartsWith(TEXT("${")) && !Placeholder.EndsWith(TEXT("}")))
		{
			Placeholder = FString::Printf(TEXT("\\$\\{%s\\}"), *Placeholder);
		}
		else
		{
			Placeholder = FString::Printf(TEXT("\\$\\{%s\\}"), *Placeholder.Mid(2, Placeholder.Len() - 3));
		}

		FRegexPattern Pattern(Placeholder);
		FRegexMatcher Matcher(Pattern, ResultString);

		FString TempResultString;
		int32 LastPosition = 0;

		while (Matcher.FindNext())
		{
			int32 MatchBeginning = Matcher.GetMatchBeginning();
			int32 MatchEnding = Matcher.GetMatchEnding();
			
			TempResultString.Append(ResultString.Mid(LastPosition, MatchBeginning - LastPosition));
			TempResultString.Append(ReplacementString);

			LastPosition = MatchEnding;
		}
		
		TempResultString.Append(ResultString.Mid(LastPosition));
		ResultString = TempResultString;
	}

	return FText::FromString(ResultString);
}