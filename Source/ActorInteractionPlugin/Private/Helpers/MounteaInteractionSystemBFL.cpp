// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.


#include "Helpers/MounteaInteractionSystemBFL.h"
#include "Helpers/ActorInteractionPluginSettings.h"
#include "Helpers/MounteaInteractionSettingsConfig.h"

#include "CommonInputSubsystem.h"
#include "CommonInputTypeEnum.h"

#include "Framework/Application/SlateApplication.h"

#include "GameFramework/Pawn.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Internationalization/Regex.h"

#include "Components/MeshComponent.h"

#include "Engine/World.h"

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

ULocalPlayer* UMounteaInteractionSystemBFL::FindLocalPlayer(AActor* ForActor)
{
	if (!ForActor)
		return nullptr;

	if (!ForActor->GetWorld())
		return nullptr;

	APlayerController* localPlayerController = Cast<APlayerController>(ForActor);
	if (localPlayerController)
	{
		return localPlayerController->GetLocalPlayer();
	}
	
	APawn* localPawn = Cast<APawn>(ForActor);
	if (localPawn)
	{
		localPlayerController = localPawn->GetController<APlayerController>();
		if (localPlayerController)
		{
			return localPlayerController->GetLocalPlayer();
		}
	}

	localPlayerController = ForActor->GetWorld()->GetFirstPlayerController();
	if (localPlayerController)
		return FindLocalPlayer(localPlayerController);

	return nullptr;
}

bool UMounteaInteractionSystemBFL::IsGamePadConnected()
{
	auto genericApplication = FSlateApplication::Get().GetPlatformApplication();
	if (genericApplication.Get() != nullptr && genericApplication->IsGamepadAttached())
	{
		return true;
	}
	return false;
}

ECommonInputType UMounteaInteractionSystemBFL::GetActiveInputType(APlayerController* PlayerController)
{
	if (!PlayerController)
		return ECommonInputType::MouseAndKeyboard;

	const auto localPlayer = PlayerController->GetLocalPlayer();
	if (!localPlayer)
		return ECommonInputType::MouseAndKeyboard;
	
	UCommonInputSubsystem* commonInputSubsystem = UCommonInputSubsystem::Get(localPlayer);
	if (!commonInputSubsystem)
		return ECommonInputType::MouseAndKeyboard;

	return commonInputSubsystem->GetCurrentInputType();
}

bool UMounteaInteractionSystemBFL::IsInputKeyPairSupported(APlayerController* PlayerController, const FKey& InputKey, const FString& HardwareDeviceID, TSoftObjectPtr<class UTexture2D>& FoundInputTexture)
{
	const UActorInteractionPluginSettings* settings = GetMutableDefault<UActorInteractionPluginSettings>();
	if (!settings)
		return false;

	ULocalPlayer* localPlayer = FindLocalPlayer(PlayerController);
	if (!localPlayer)
		return false;

	const auto interactionConfig = settings->DefaultInteractionSystemConfig;
	if (!interactionConfig)
		return false;

	const auto interactionConfigRef = interactionConfig.LoadSynchronous();
	if (!interactionConfigRef)
		return false;

	if (interactionConfigRef->MappingKeys.Num() == 0)
		return false;

	const FString platformName = UGameplayStatics::GetPlatformName();
	const ECommonInputType activeInputType = GetActiveInputType(PlayerController);

	for (const auto& Itr : interactionConfigRef->MappingKeys)
	{
		if (InputKey != Itr.Key)
			continue;

		TPair<ECommonInputType, FString> testPair(activeInputType, platformName);

		const FKeyOnDevicePair* foundPair = Itr.Value.KeyPairs.FindByPredicate([&testPair, &HardwareDeviceID](const FKeyOnDevicePair& Pair)
		{
			if (Pair.BlacklistedDeviceIDs.IsEmpty())
			{
				return Pair == testPair;
			}
			return Pair == testPair && !Pair.BlacklistedDeviceIDs.Contains(HardwareDeviceID);
		});

		if (foundPair)
		{
			FoundInputTexture = foundPair->KeyTexture;
			return true;
		}
	}

	return false;
}