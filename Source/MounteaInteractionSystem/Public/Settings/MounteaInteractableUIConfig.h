// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputCoreTypes.h"
#include "MounteaInteractableUIConfig.generated.h"

class UUserWidget;
class UDataTable;
enum class ECommonInputType : uint8;

/**
 * Pairing of a key texture with a supported device type and platforms.
 * Used in TMap<FKey, FKeyOnDevice> for input prompt display.
 */
USTRUCT(BlueprintType)
struct MOUNTEAINTERACTIONSYSTEM_API FKeyOnDevicePair
{
	GENERATED_BODY()

	/** Texture to display for this key on the interaction widget. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Input")
	TSoftObjectPtr<class UTexture2D> KeyTexture;

	/** Device type that this pair applies to (Keyboard, Gamepad, Touch, etc.). */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Input")
	ECommonInputType SupportedDeviceType;

	/** Platform identifiers this pair is valid for (empty = all platforms). */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Input")
	TArray<FString> SupportedPlatforms;

	/** Device IDs that explicitly do NOT display this texture. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Input")
	TArray<FString> BlacklistedDeviceIDs;

	FKeyOnDevicePair();

	FKeyOnDevicePair(const TSoftObjectPtr<UTexture2D>& InKeyTexture, ECommonInputType InSupportedDeviceType, const TArray<FString>& InSupportedPlatforms)
		: KeyTexture(InKeyTexture), SupportedDeviceType(InSupportedDeviceType), SupportedPlatforms(InSupportedPlatforms)
	{}

	bool operator==(const FKeyOnDevicePair& Other) const
	{
		return SupportedDeviceType == Other.SupportedDeviceType && SupportedPlatforms == Other.SupportedPlatforms;
	}

	bool operator==(const TPair<ECommonInputType, FString>& InputPair) const
	{
		return SupportedDeviceType == InputPair.Key && SupportedPlatforms.Contains(InputPair.Value);
	}

	bool operator!=(const FKeyOnDevicePair& Other) const { return !(*this == Other); }
};

/**
 * Collection of device-specific key texture pairs for a single input key.
 */
USTRUCT(BlueprintType)
struct MOUNTEAINTERACTIONSYSTEM_API FKeyOnDevice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Input", meta=(ForceInlineRow, TitleProperty="SupportedDeviceType"))
	TArray<FKeyOnDevicePair> KeyPairs;

	FKeyOnDevice() {}
	FKeyOnDevice(const TArray<FKeyOnDevicePair>& InKeyPairs) : KeyPairs(InKeyPairs) {}

	bool operator==(const FKeyOnDevice& Other) const { return KeyPairs == Other.KeyPairs; }
	bool operator!=(const FKeyOnDevice& Other) const { return !(*this == Other); }
};

/**
 * UI configuration for Interactable components.
 * Controls widget classes, key-to-texture mappings, and update frequency.
 */
UCLASS(BlueprintType, meta=(DisplayName="Mountea Interactable UI Config"))
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractableUIConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Default interaction widget class shown in InteractionRange when this component is the selected action. Must implement IActorInteractionWidget. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget", meta=(MustImplement="/Script/MounteaInteractionSystem.ActorInteractionWidget"))
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;

	/** DataTable supplying display data to interactable widgets (row type defined per project). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget")
	TSoftObjectPtr<UDataTable> DefaultInteractableDataTable;

	/** Per-key texture mapping for input prompt display on the interaction widget. Key = input key; Value = per-device texture set. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ForceInlineRow))
	TMap<FKey, FKeyOnDevice> MappingKeys;

	/** How often (in seconds) the interaction widget polls for data updates. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget", meta=(Units="s", UIMin=0.001, ClampMin=0.001))
	float WidgetUpdateFrequency = 0.05f;
};
