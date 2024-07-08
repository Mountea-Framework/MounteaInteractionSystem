// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "InteractionHelpers.h"
#include "Engine/DataAsset.h"
#include "InputCoreTypes.h"
#include "MounteaInteractionSettingsConfig.generated.h"

enum class ECommonInputType : uint8;
struct FKey;

/**
 * Represents a pairing of a key texture with a supported device type and platforms.
 */
USTRUCT(BlueprintType)
struct FKeyOnDevicePair
{
	GENERATED_BODY()

	/** The texture to display for the key. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input")
	TSoftObjectPtr<class UTexture2D> KeyTexture;

	/** The type of device that supports this key. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input")
	ECommonInputType SupportedDeviceType;

	/** The platforms that support this key. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input")
	TArray<FString> SupportedPlatforms;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input")
	TArray<FString> BlacklistedDeviceIDs;

	FKeyOnDevicePair();

	FKeyOnDevicePair(const TSoftObjectPtr<UTexture2D>& InKeyTexture, ECommonInputType InSupportedDeviceType, const TArray<FString>& InSupportedPlatforms)
		: KeyTexture(InKeyTexture), SupportedDeviceType(InSupportedDeviceType), SupportedPlatforms(InSupportedPlatforms)
	{}

	bool operator==(const FKeyOnDevicePair& Other) const
	{
		return
				SupportedDeviceType == Other.SupportedDeviceType &&
				SupportedPlatforms == Other.SupportedPlatforms;
	}

	bool operator==(const TPair<ECommonInputType, FString>& InputPair) const
	{
		const ECommonInputType& InputType = InputPair.Key;
		const FString& Platform = InputPair.Value;
		
		return SupportedDeviceType == InputType && SupportedPlatforms.Contains(Platform);
	}

	bool operator!=(const FKeyOnDevicePair& Other) const
	{
		return !(*this == Other);
	}
};

/**
 * Represents a collection of key-device pairs.
 */
USTRUCT(BlueprintType)
struct FKeyOnDevice
{
	GENERATED_BODY()

	/** The list of key-device pairs. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input", meta = (ForceInlineRow, TitleProperty="SupportedDeviceType"))
	TArray<FKeyOnDevicePair> KeyPairs;

	FKeyOnDevice() {}

	FKeyOnDevice(const TArray<FKeyOnDevicePair>& InKeyPairs)
		: KeyPairs(InKeyPairs)
	{}

	bool operator==(const FKeyOnDevice& Other) const
	{
		return KeyPairs == Other.KeyPairs;
	}

	bool operator!=(const FKeyOnDevice& Other) const
	{
		return !(*this == Other);
	}
};

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), EditInlineNew, meta = (DisplayName = "Mountea Interaction System Settings Configuration"), autoexpandcategories=("Interactor","Interactable","Input"))
class ACTORINTERACTIONPLUGIN_API UMounteaInteractionSettingsConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Interactor")
	FInteractorBaseSettings													InteractorDefaultSettings;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Interactable")
	FInteractableBaseSettings												InteractableBaseSettings;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input", meta=(ForceInlineRow))
	TMap<FKey, FKeyOnDevice>											MappingKeys;
};
