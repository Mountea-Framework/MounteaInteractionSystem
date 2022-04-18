// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#define INTERACTOR_TAG_NAME   TEXT("Interactor")
#define INTERACTABLE_TAG_NAME TEXT("Interactable")

/**
 * Type of Interactable Actor Component.
 * 
 * Defines how is interaction processed with Interactable Actor.
 */
UENUM(BlueprintType)
enum class EInteractableType : uint8
{
 EIT_Hold    UMETA(DisplayName = "Hold", Tooltip = "Press and hold the button."),
 EIT_Press   UMETA(DisplayName = "Press", Tooltip = "Press the button only once"),
 EIT_Mash    UMETA(DisplayName = "Mash", Tooltip = "Press the button multiple times for a period of time"),
 EIT_Hybrid  UMETA(DisplayName = "Hybrid - Hold & Press", Tooltip = "Press and hold the button, based on time treshold only relevant event will be called."),
 EIT_Auto    UMETA(DisplayName = "Auto", Tooltip = "Interactable will automatically finish if interaction conditions are met (tracing or overlapping)."),

 Default     UMETA(Hidden)
};

/**
 * State of Interactable Actor Component.
 * 
 * Machine States of the Interactable Actor Component.
 */
UENUM(BlueprintType)
enum class EInteractableState : uint8
{
 EIS_Standby  UMETA(DisplayName = "StandBy", ToolTip = "Interactable is enabled"),
 EIS_Active   UMETA(DisplayName = "Active", Tooltip = "Interactable is in use"),
 EIS_Inactive UMETA(DisplayName = "Inactive", Tooltip = "Interactable is inactive, but can be activated"),
 EIS_Cooldown UMETA(DisplayName = "Cooldown", Tooltip = "Interactable is enabled, but inactive until cooldown period"),
 EIS_Finished UMETA(DisplayName = "Finished", Tooltp = "Interactable is disabled, but was sucessfully used"),
 EIS_Disabled UMETA(DisplayName = "Disabled", Tooltip = "Interactable is disabled and cannot be activated unless requsted by function"),

 Default      UMETA(Hidden)
};

/**
 * Type of Interactable Lifecycle.
 *
 * Defined whether Interactable should be disabled once finished or whether should wait defined time to be available again.
 */
UENUM(BlueprintType)
enum class EInteractableLifecycle : uint8
{
  EIL_OnlyOnce UMETA(DisplayName="Once", Tooltip="Once is Interactable finished, interaction is no more allowed."),
  EIL_Cycled   UMETA(DisplayName="Cycled", Tooltip="Once is Interactable finished, interaction is allowed once Cooldown period is finished. Allowed for unlimited time unless Maximum Lifecycles are specified."),

  Default      UMETA(Hidden)
};

/**
 * Type of Interactor Actor Component.
 * 
 * Defines how Interactor Actor Component does process interaction.
 */
UENUM(BlueprintType)
enum class EInteractorType : uint8
{
 EIT_Active  UMETA(DisplayName = "Active", ToolTip = "Interaction is allowed only if the tracing is colliding with collision box of the Interactable Actor Component."),
 EIT_Passive UMETA(DisplayName = "Passive", Tooltip ="Interaction is allowed only if the Owning Actor does overlap collision box of the Interactable Actor Component."),
 EIT_Mixed   UMETA(DisplayName = "Mixed", Tooptip = "Interaction is allowed for both tracing and overlapping with collision box of the Interactable Actor Component."),
 
 Default     UMETA(Hidden)
};

/**
 * State of Interactable Actor Component.
 * 
 * Machine States of the Interactor Actor Component.
 */
UENUM(BlueprintType)
enum class EInteractorState : uint8
{
 EIS_Disabled UMETA(DisplayName = "Disabled", Tooltip = "Interactor is disabled"),
 EIS_StandBy  UMETA(DisplayName = "StandBy", Tooltip = "Interactor is enabled"),
 EIS_Active   UMETA(DisplayName = "Active", Tooltip = "Interactor is in use"),

 Default      UMETA(Hidden)
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
 EIP_High    UMETA(DisplayName = "High Precision", Tooltip = "High Precision is using less tolerant Line Tracing method to find Interactable Actor."),
 EIP_Low     UMETA(DisplayName = "Low Precision",Tooltip = "Low Precision is using more tolerant Box Tracing method to find Interactable Actor."),

 Default     UMETA(Hidden)
};

/**
 * Template that allows reading Name value from any given UENUM.
 * @param Name: Name of the UENUM (ECollisionChannel, for instance)
 * @param Value: Enum value to be translated (ECC_Visibility, for instance)
 * @return FString of the Value (ECC_Visibility in our example, or invalid of name not specified nor UENUM does not exist)
 */
template<typename TEnum>
static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
{
  const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
  if (!enumPtr) return FString("invalid");
  return enumPtr->GetNameByValue((int64)Value).ToString();
}
