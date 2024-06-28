// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInteractionSystemBFL.generated.h"

class UActorInteractionPluginSettings;

/**
 * 
 */
UCLASS()
class ACTORINTERACTIONPLUGIN_API UMounteaInteractionSystemBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/**
	 * 
	 * @param Tag
	 * @param Source 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static UMeshComponent* FindMeshByTag(const FName Tag, const AActor* Source);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static UMeshComponent* FindMeshByName(const FName Name, const AActor* Source);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static UPrimitiveComponent* FindPrimitiveByTag(const FName Tag, const AActor* Source);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static UPrimitiveComponent* FindPrimitiveByName(const FName Name, const AActor* Source);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Helpers")
	static UActorInteractionPluginSettings* GetInteractionSystemSettings();
};
