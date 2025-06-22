// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInteractionSystemBFL.generated.h"

class UActorInteractionPluginSettings;
enum class ECommonInputType : uint8;

/**
 * 
 */
UCLASS()
class ACTORINTERACTIONPLUGIN_API UMounteaInteractionSystemBFL : public UBlueprintFunctionLibrary
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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static UMeshComponent* FindMeshByTag(const FName Tag, const AActor* Source);

	/**
	 * Finds a mesh component by name within the specified actor.
	 *
	 * @param Name					The name to search for.
	 * @param Source				The actor to search within.
	 * @return							The first mesh component found with the specified name, or nullptr if none is found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static UMeshComponent* FindMeshByName(const FName Name, const AActor* Source);

	/**
	 * Finds a primitive component by tag within the specified actor.
	 *
	 * @param Tag					The tag to search for.
	 * @param Source				The actor to search within.
	 * @return							The first primitive component found with the specified tag, or nullptr if none is found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static UPrimitiveComponent* FindPrimitiveByTag(const FName Tag, const AActor* Source);

	/**
	 * Finds a primitive component by name within the specified actor.
	 *
	 * @param Name					The name to search for.
	 * @param Source				The actor to search within.
	 * @return							The first primitive component found with the specified name, or nullptr if none is found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static UPrimitiveComponent* FindPrimitiveByName(const FName Name, const AActor* Source);

	/**
	 * Retrieves the interaction system settings.
	 *
	 * @return							The interaction system settings object.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static UActorInteractionPluginSettings* GetInteractionSystemSettings();

	/**
	 * Determines if cosmetic events can be executed in the specified world context.
	 *
	 * @param WorldContext	The world context to check.
	 * @return							True if cosmetic events can be executed, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static bool CanExecuteCosmeticEvents(const UWorld* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static FText ReplaceRegexInText(const FText& SourceText, const TMap<FString, FText>& Replacements);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static ULocalPlayer* FindLocalPlayer(AActor* ForActor);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static bool IsGamePadConnected();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static ECommonInputType GetActiveInputType(class APlayerController* PlayerController);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static bool IsInputKeyPairSupported(class APlayerController* PlayerController, const FKey& InputKey, const FString& HardwareDeviceID, TSoftObjectPtr<class UTexture2D>& FoundInputTexture);
};
