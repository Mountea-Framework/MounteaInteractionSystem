// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Engine/EngineTypes.h"
#include "InteractionHelpers.generated.h"


/**
 * Type of Interactable Lifecycle.
 *
 * Defined whether Interactable should be disabled once finished or whether should wait defined time to be available again.
 */
UENUM(BlueprintType)
enum class EInteractableLifecycle : uint8
{
  EIL_OnlyOnce		UMETA(DisplayName="Once",					Tooltip="Once is Interactable finished, interaction is no more allowed."),
  EIL_Cycled			UMETA(DisplayName="Cycled",					Tooltip="Once is Interactable finished, interaction is allowed once Cooldown period is finished. Allowed for unlimited time unless Maximum Lifecycles are specified."),

  Default					UMETA(Hidden)
};

/**
 * Type of Interactor Tracing precision.
 *
 * Defines how much is tracing method tolerant.
 * High Precision method will find Interactable only in middle of the camera viewport.
 * Low Precision method will find Interactable even slightly off the middle of the camera viewport.
 */
UENUM(BlueprintType)
enum class EInteractorPrecision : uint8
{
 EIP_High					UMETA(DisplayName = "High Precision",	Tooltip = "High Precision is using less tolerant Line Tracing method to find Interactable Actor."),
 EIP_Low					UMETA(DisplayName = "Low Precision",	Tooltip = "Low Precision is using more tolerant Box Tracing method to find Interactable Actor."),

 Default					UMETA(Hidden)
};

#pragma region State Machine

/**
 * State of Interactable Actor Component.
 * 
 * Machine State of the Interactable Actor Component Base.
 */
UENUM(BlueprintType, meta=(ScriptName="InteractableState"))
enum class EInteractableStateV2 : uint8
{
 EIS_Active				UMETA(DisplayName = "Active",				Tooltip = "Interactable is awaken and being interacted with."),
 EIS_Awake			UMETA(DisplayName = "Awake",				ToolTip = "Interactable is awaken and can be interacted with. Will react to Interactors."),
 EIS_Cooldown		UMETA(DisplayName = "Cooldown",			Tooltip = "Interactable is disabled during cooldown period. Then will be awaken again. Doesn't react to Interactors."),
 EIS_Paused			UMETA(DisplayName = "Paused",				Tooltip = "Interactable is paused during pause period. Does react to Interactors and continues in the progress."),
 EIS_Completed		UMETA(DisplayName = "Finished",				Tooltip = "Interactable is disabled after sucesful interaction. Doesn't react to Interactors. Cannot be activated again."),
 EIS_Disabled			UMETA(DisplayName = "Disabled",				Tooltip = "Interactable is disabled. Can be awaken. Doesn't react to Interactors."),
 EIS_Suppressed	UMETA(DisplayName = "Suppressed",		Tooltip = "Interactable is suppressed and cannot be interacted with. Can be awaken. Doesn't react to Interactors."),

 EIS_Asleep				UMETA(Hidden),
 Default					UMETA(Hidden)
};

UENUM(BlueprintType, meta=(ScriptName="InteractorState"))
enum class EInteractorStateV2 : uint8
{
 EIS_Awake			UMETA(DisplayName = "Awake",				Tooltip = "Interactor is awaken. Interactor is looking for Interactables."),
 EIS_Asleep				UMETA(DisplayName = "Asleep",				Tooltip = "Interactor is asleep, but can be awaken. Default state. Useful for cutscenes or UI interactions."),
 EIS_Suppressed	UMETA(DisplayName = "Suppressed",		Tooltip = "Interactor is suppressed and cannot interact. Default state for secondary Interactors when master is Active."),
 EIS_Active				UMETA(DisplayName = "Interacting",			Tooltip = "Interactor is in use"),
 EIS_Disabled			UMETA(DisplayName = "Disabled",				Tooltip = "Interactor is disabled. Can be awaken."),

 Default					UMETA(Hidden)
};

#pragma endregion

#pragma region CollisionCache

/**
 * Collision Shape Cache data.
 * 
 * Holds data for each Collision Shape before interaction setup has been applied.
 * Used for resetting Collision Shapes to pre-interaction state.
 */
USTRUCT(BlueprintType)
struct FCollisionShapeCache
{
 GENERATED_BODY()
	
 FCollisionShapeCache()
 {
  bGenerateOverlapEvents = false;
  CollisionEnabled = ECollisionEnabled::QueryOnly;
  CollisionResponse = ECR_Overlap;
 };
	
 FCollisionShapeCache(bool GeneratesOverlaps, TEnumAsByte<ECollisionEnabled::Type> collisionEnabled, TEnumAsByte<ECollisionResponse> collisionResponse) :
 bGenerateOverlapEvents(GeneratesOverlaps),
  CollisionEnabled(collisionEnabled),
  CollisionResponse(collisionResponse)
 {};
	
 UPROPERTY(Category="Interaction|Collision Cache", VisibleAnywhere)
 uint8 bGenerateOverlapEvents : 1;
 UPROPERTY(Category="Interaction|Collision Cache", VisibleAnywhere)
 TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled;
 UPROPERTY(Category="Interaction|Collision Cache", VisibleAnywhere)
 TEnumAsByte<ECollisionResponse> CollisionResponse;
	
};

#pragma endregion

#pragma region Debug

#pragma region DebugSettings

/**
 * Helper structure to encapsulate Debug Settings
 */
USTRUCT(BlueprintType)
struct FDebugSettings
{
    GENERATED_BODY()

public:

    FDebugSettings()
    {
     DebugMode = false;
     EditorDebugMode = true;
    };
    FDebugSettings(uint8 NewDebug, uint8 NewEditorDebug) :
    DebugMode(NewDebug),
    EditorDebugMode(NewEditorDebug)
    {};
    FDebugSettings(const FDebugSettings& Other) :
    DebugMode(Other.DebugMode),
    EditorDebugMode(Other.EditorDebugMode)
    {};
    FDebugSettings(const bool bDebug)
    {
        DebugMode = bDebug;
        EditorDebugMode = bDebug;
    }

   /**
    * Enables Debug in Gameplay.
    * Default: Off
    */
    UPROPERTY(Category="Interaction|Debug", BlueprintReadWrite, EditAnywhere)
    uint8 DebugMode : 1;
   /**
    * Enables Warnings in Editor.
    * Default: On
    */
    UPROPERTY(Category="Interaction|Debug", BlueprintReadWrite, EditAnywhere)
    uint8 EditorDebugMode : 1;
};

#pragma endregion 

#pragma endregion 

#pragma region TEMPLATES

/**
 * Template that allows reading Name value from any given UENUM.
 * @param Name: Name of the UENUM (ECollisionChannel, for instance)
 * @param Value: Enum value to be translated (ECC_Visibility, for instance)
 * @return FString of the Value (ECC_Visibility in our example, or invalid of name not specified nor UENUM does not exist)
 */
template<typename TEnum>
static FString GetEnumValueAsString(const FString& Name, TEnum Value)
{
 const UEnum* enumPtr = FindFirstObject<UEnum>(*Name, EFindFirstObjectOptions::None);
 if (!enumPtr) return FString("invalid");
 return enumPtr->GetDisplayNameTextByValue((int64)Value).ToString();
}

#pragma endregion TEMPLATES