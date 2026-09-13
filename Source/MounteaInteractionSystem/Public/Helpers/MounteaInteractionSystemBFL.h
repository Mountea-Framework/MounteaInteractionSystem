// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInteractionSystemBFL.generated.h"

class UMounteaInteractionSystemSettings;
enum class ECommonInputType : uint8;

/**
 * General-purpose utility library - NOT interaction-domain-specific. Covers input-device
 * detection, local-player lookup, and text helpers used incidentally by interaction UI, but
 * useful independent of this plugin's interaction system.
 *
 * The five component-lookup and settings-accessor functions below are the exception: they were
 * genuinely interaction-domain and have been ported to UMounteaInteractableStatics (same name).
 * They're deprecated here - kept only so existing customer Blueprints keep compiling - while the
 * remaining functions in this class stay permanently, since force-porting general-purpose helpers
 * into an interaction-domain Statics class under a misleading name would be worse than leaving
 * them where they are.
 */
UCLASS()
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractionSystemBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Finds a mesh component by tag within the specified actor.
	 *
	 * @param Tag					The tag to search for.
	 * @param Source				The actor to search within.
	 * @return							The first mesh component found with the specified tag, or nullptr if none is found.
	 */
	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::FindMeshByTag instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers",
		meta=(DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::FindMeshByTag instead."))
	static UMeshComponent* FindMeshByTag(const FName Tag, const AActor* Source);

	/**
	 * Finds a mesh component by name within the specified actor.
	 *
	 * @param Name					The name to search for.
	 * @param Source				The actor to search within.
	 * @return							The first mesh component found with the specified name, or nullptr if none is found.
	 */
	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::FindMeshByName instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers",
		meta=(DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::FindMeshByName instead."))
	static UMeshComponent* FindMeshByName(const FName Name, const AActor* Source);

	/**
	 * Finds a primitive component by tag within the specified actor.
	 *
	 * @param Tag					The tag to search for.
	 * @param Source				The actor to search within.
	 * @return							The first primitive component found with the specified tag, or nullptr if none is found.
	 */
	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::FindPrimitiveByTag instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers",
		meta=(DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::FindPrimitiveByTag instead."))
	static UPrimitiveComponent* FindPrimitiveByTag(const FName Tag, const AActor* Source);

	/**
	 * Finds a primitive component by name within the specified actor.
	 *
	 * @param Name					The name to search for.
	 * @param Source				The actor to search within.
	 * @return							The first primitive component found with the specified name, or nullptr if none is found.
	 */
	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::FindPrimitiveByName instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers",
		meta=(DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::FindPrimitiveByName instead."))
	static UPrimitiveComponent* FindPrimitiveByName(const FName Name, const AActor* Source);

	/**
	 * Retrieves the interaction system settings.
	 *
	 * @return							The interaction system settings object.
	 */
	UE_DEPRECATED(5.0, "Use UMounteaInteractableStatics::GetInteractionSystemSettings instead.")
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers",
		meta=(DeprecatedFunction, DeprecationMessage="Use UMounteaInteractableStatics::GetInteractionSystemSettings instead."))
	static UMounteaInteractionSystemSettings* GetInteractionSystemSettings();

	/**
	 * Determines if cosmetic events can be executed in the specified world context.
	 *
	 * @param WorldContext	The world context to check.
	 * @return							True if cosmetic events can be executed, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static bool CanExecuteCosmeticEvents(const UWorld* WorldContext);

	/** Returns a copy of SourceText with each occurrence of a Replacements key substituted by its matching value. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static FText ReplaceRegexInText(const FText& SourceText, const TMap<FString, FText>& Replacements);

	/** Returns the local player associated with the given actor, if any. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static ULocalPlayer* FindLocalPlayer(AActor* ForActor);

	/** Returns true if a gamepad is currently connected. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static bool IsGamePadConnected();

	/** Returns the active input device type (keyboard/mouse, gamepad, touch, etc.) for the given player controller. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static ECommonInputType GetActiveInputType(class APlayerController* PlayerController);

	/** Returns true if the given input key is supported on the specified hardware device, and outputs its associated icon texture. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static bool IsInputKeyPairSupported(class APlayerController* PlayerController, const FKey& InputKey, const FString& HardwareDeviceID, TSoftObjectPtr<class UTexture2D>& FoundInputTexture);
};
