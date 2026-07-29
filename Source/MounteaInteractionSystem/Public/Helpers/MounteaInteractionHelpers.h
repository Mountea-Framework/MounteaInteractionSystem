// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputCoreTypes.h"
#include "Engine/EngineTypes.h"
#include "MounteaInteractionHelpers.generated.h"

class UMaterialInterface;

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
 };

 FCollisionShapeCache(bool GeneratesOverlaps, TEnumAsByte<ECollisionEnabled::Type> collisionEnabled, const FCollisionResponseContainer& InResponseContainer) :
 bGenerateOverlapEvents(GeneratesOverlaps),
  CollisionEnabled(collisionEnabled),
  ResponseContainer(InResponseContainer)
 {};

 UPROPERTY(Category="MounteaInteraction|Collision Cache", VisibleAnywhere)
 uint8 bGenerateOverlapEvents : 1;
 UPROPERTY(Category="MounteaInteraction|Collision Cache", VisibleAnywhere)
 TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled;
 UPROPERTY(Category="MounteaInteraction|Collision Cache", VisibleAnywhere)
 FCollisionResponseContainer ResponseContainer;
	
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
    UPROPERTY(Category="MounteaInteraction|Debug", BlueprintReadWrite, EditAnywhere)
    uint8 DebugMode : 1;
   /**
    * Enables Warnings in Editor.
    * Default: On
    */
    UPROPERTY(Category="MounteaInteraction|Debug", BlueprintReadWrite, EditAnywhere)
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
 if (!enumPtr)
				return FString("invalid");
 return enumPtr->GetDisplayNameTextByValue((int64)Value).ToString();
}

#pragma endregion TEMPLATES

#pragma region HighlightType

UENUM(BlueprintType)
enum class EHighlightType : uint8
{
	EHT_PostProcessing		UMETA(DisplayName="PostProcessing",			Tooltip="PostProcessing Material will be used. This option is highly optimised, however, requires Project setup."),
	EHT_OverlayMaterial		UMETA(DisplayName="Overlay Material",		Tooltip="Overlay Material will be used. Unique for 5.1 and newer versions. For very complex meshes might cause performance issues."),

	EHT_Default					UMETA(Hidden)
};

#pragma endregion

#pragma region HighlightSetup

/**
 * Mountea Highlight Setup
 */
USTRUCT(BlueprintType)
struct FInteractionHighlightSetup
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="Highlight Setup")
	EHighlightType HighlightType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="Highlight Setup", meta=(EditCondition="HighlightType==EHighlightType::EHT_PostProcessing"))
	int32 StencilID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="Highlight Setup", meta=(EditCondition="HighlightType==EHighlightType::EHT_OverlayMaterial"))
	TObjectPtr<UMaterialInterface> HighlightMaterial;

	FInteractionHighlightSetup()
	{
		HighlightType = EHighlightType::EHT_OverlayMaterial;
		StencilID = 133;
		HighlightMaterial = nullptr;
	}
};

#pragma endregion

#pragma region SetupType
/**
 * Enumerator definition of setup modes.
 */
UENUM(BlueprintType)
enum class ESetupType : uint8
{
	EST_FullAll			UMETA(DisplayName="Full Auto Setup",			Tooltip="Will add all components from Owning Actor to Highlightable and Collision Components."),
	EST_AllParent		UMETA(DisplayName="All Parents Auto Setup", 	Tooltip="Will add all parent components to Highlightable and Collision Components."),
	EST_Quick			UMETA(DisplayName="Quick Auto Setup",			Tooltip="Will add only first parent component to Highlightable and Collision Components."),
	EST_None			UMETA(DisplayName="None",						Tooltip="No auto setup will be performed."),

	EST_Default		UMETA(Hidden)
};

#pragma endregion

#pragma region ComparisonMethod
UENUM(BlueprintType)
enum class ETimingComparison : uint8 // TODO: rename, because name is used
{
	ECM_LessThan	UMETA(DisplayName="Less Than"),
	ECM_MoreThan	UMETA(DisplayName="More Than"),
	ECM_None			UMETA(DisplayName="No comparison"),

	Default					UMETA(Hidden)
   };
#pragma endregion 

#pragma region SafetyTracing

/**
 * Enum to define the mode of safety tracing.
 */
UENUM(BlueprintType)
enum class ESafetyTracingMode : uint8
{
	ESTM_None					UMETA(DisplayName="None",		ToolTip="Safety tracing will not perform."),
	ESTM_Location			UMETA(DisplayName="Location",	ToolTip="Location must be provided in order to cast the ray. If none is provided, GetLocation() will be used instead."),
	ESTM_Socket				UMETA(DisplayName="Socket",		ToolTip="Name of socket must be provided. If socket is not found, GetLocation() will be used instaed."),

	Default							UMETA(hidden)
};

/**
 * Struct to define the setup for safety tracing.
 */
USTRUCT(BlueprintType)
struct FSafetyTracingSetup
{
	GENERATED_BODY()

	/** Mode of safety tracing to be used. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Setup")
	ESafetyTracingMode SafetyTracingMode;

	/** Overrides the global TraceCollisionChannelName from the interactor runtime config.
	 *  NAME_None = use the global default. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Setup",
		meta=(NoResetToDefault),
		meta=(EditCondition="SafetyTracingMode != ESafetyTracingMode::ESTM_None"))
	FName OverrideTraceChannelName;

	/** Starting location for safety tracing, used when SafetyTracingMode is set to Location. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Setup", meta=(EditCondition="SafetyTracingMode == ESafetyTracingMode::ESTM_Location"))
	FVector StartLocation;

	/** Starting socket name for safety tracing, used when SafetyTracingMode is set to Socket. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Setup",  meta=(EditCondition="SafetyTracingMode == ESafetyTracingMode::ESTM_Socket"))
	FName ActorMeshName;

	/** Starting socket name for safety tracing, used when SafetyTracingMode is set to Socket. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Setup",  meta=(EditCondition="SafetyTracingMode == ESafetyTracingMode::ESTM_Socket"))
	FName StartSocketName;

	FSafetyTracingSetup()
		: SafetyTracingMode(ESafetyTracingMode::ESTM_None)
		, OverrideTraceChannelName(NAME_None)
		, StartLocation(FVector::ZeroVector)
		, ActorMeshName(NAME_None)
		, StartSocketName(FName("head"))
	{}

	FSafetyTracingSetup(ESafetyTracingMode InSafetyTracingMode, const FVector& InStartLocation = FVector(), const FName InActorMeshName = FName(), const FName InStartSocketName = FName())
		: SafetyTracingMode(InSafetyTracingMode)
		, OverrideTraceChannelName(NAME_None)
		, StartLocation(InStartLocation)
		, ActorMeshName(InActorMeshName)
		, StartSocketName(InStartSocketName)
	{}

	bool operator==(const FSafetyTracingSetup& Other) const
	{
		if (SafetyTracingMode != Other.SafetyTracingMode || OverrideTraceChannelName != Other.OverrideTraceChannelName)
		{
			return false;
		}

		switch (SafetyTracingMode)
		{
			case ESafetyTracingMode::ESTM_Location:
				return StartLocation == Other.StartLocation;
			case ESafetyTracingMode::ESTM_Socket:
				return ActorMeshName == Other.ActorMeshName && StartSocketName == Other.StartSocketName;
			case ESafetyTracingMode::ESTM_None:
			default:
				return true;
		}
	}


	bool operator!=(const FSafetyTracingSetup& Other) const
	{
		return !(*this == Other);
	}

	FString ToString() const
	{
		const FString channelStr = OverrideTraceChannelName.IsNone() ? TEXT("(global)") : OverrideTraceChannelName.ToString();
		switch (SafetyTracingMode)
		{
			case ESafetyTracingMode::ESTM_Location:
				return FString::Printf(TEXT("SafetyTracingMode: Location, TraceChannel: %s, StartLocation: %s"), *channelStr, *StartLocation.ToString());
			case ESafetyTracingMode::ESTM_Socket:
				return FString::Printf(TEXT("SafetyTracingMode: Socket, TraceChannel: %s, ActorMeshName: %s, StartSocketName: %s"), *channelStr, *ActorMeshName.ToString(), *StartSocketName.ToString());
			case ESafetyTracingMode::ESTM_None:
			default:
				return FString::Printf(TEXT("SafetyTracingMode: None, TraceChannel: %s"), *channelStr);
		}
	}

};

#pragma endregion

#pragma region V3_StateMachines

/**
 * Interactable state machine — v5.0.
 * Adds EIS_Attention for the distance-based widget system (in attention range but not interaction range).
 */
UENUM(BlueprintType, meta=(ScriptName="InteractableStateV3"))
enum class EInteractableStateV3 : uint8
{
	EIS_Active		UMETA(DisplayName = "Active",		Tooltip = "Interactable is being interacted with."),
	EIS_Awake		UMETA(DisplayName = "Awake",		Tooltip = "Interactable can be interacted with. Reacts to Interactors."),
	EIS_Attention	UMETA(DisplayName = "Attention",	Tooltip = "Interactor is in attention range but not yet in interaction range. Shows attention widget."),
	EIS_Cooldown	UMETA(DisplayName = "Cooldown",		Tooltip = "Interactable is recovering. Will awake after cooldown."),
	EIS_Paused		UMETA(DisplayName = "Paused",		Tooltip = "Interactable is paused. Retains progress."),
	EIS_Completed	UMETA(DisplayName = "Finished",		Tooltip = "Interaction completed. Cannot be re-activated."),
	EIS_Disabled	UMETA(DisplayName = "Disabled",		Tooltip = "Interactable is disabled. Can be awakened manually."),
	EIS_Suppressed	UMETA(DisplayName = "Suppressed",	Tooltip = "Interactable is suppressed by a dependency. Cannot be interacted with."),
};

/**
 * Interactor state machine — v5.0.
 * EIS_Scanning replaces EIS_Awake for clarity.
 */
UENUM(BlueprintType, meta=(ScriptName="InteractorStateV3"))
enum class EInteractorStateV3 : uint8
{
	EIS_Scanning	UMETA(DisplayName = "Scanning",		Tooltip = "Interactor is actively scanning for interactables."),
	EIS_Active		UMETA(DisplayName = "Interacting",	Tooltip = "Interactor is performing an interaction."),
	EIS_Asleep		UMETA(DisplayName = "Asleep",		Tooltip = "Interactor is asleep. Useful for cutscenes."),
	EIS_Suppressed	UMETA(DisplayName = "Suppressed",	Tooltip = "Interactor is suppressed. Cannot interact."),
	EIS_Disabled	UMETA(DisplayName = "Disabled",		Tooltip = "Interactor is disabled."),
};

#pragma endregion

#pragma region V5_WidgetMode

UENUM(BlueprintType)
enum class EInteractionWidgetMode : uint8
{
	EWM_None		UMETA(DisplayName = "None",			Tooltip = "No widget shown."),
	EWM_Attention	UMETA(DisplayName = "Attention",	Tooltip = "Subtle attention widget shown (interactor in attention range)."),
	EWM_Interaction	UMETA(DisplayName = "Interaction",	Tooltip = "Full interaction widget shown (interactor in interaction range)."),
};

#pragma endregion

#pragma region V5_WidgetRanges

/**
 * Widget display ranges for a single interactable component.
 * AttentionRange >= InteractionRange must hold; enforced by IsDataValid().
 */
USTRUCT(BlueprintType)
struct FInteractableWidgetRanges
{
	GENERATED_BODY()

	/** Distance at which the attention widget appears. Must be >= InteractionRange. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget Ranges", meta=(Units="cm", UIMin=0.f, ClampMin=0.f))
	float AttentionRange = 500.f;

	/** Distance at which the full interaction widget appears. Must be <= AttentionRange. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget Ranges", meta=(Units="cm", UIMin=0.f, ClampMin=0.f))
	float InteractionRange = 200.f;

	bool IsValid() const { return InteractionRange <= AttentionRange; }
};

#pragma endregion

#pragma region V5_InteractionActionCandidate

class IMounteaInteractableInterface;

/**
 * Represents a single interactable component discovered by an interactor.
 * The candidate list is sorted by weight (desc) then distance squared (asc).
 */
USTRUCT(BlueprintType)
struct FInteractionActionCandidate
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Candidate")
	TScriptInterface<IMounteaInteractableInterface> Interactable;

	UPROPERTY(BlueprintReadOnly, Category="Candidate")
	float CachedWeight = 0.f;

	/** Squared distance at detection time — avoids sqrt for sort comparisons. */
	UPROPERTY(BlueprintReadOnly, Category="Candidate")
	float CachedDistanceSq = 0.f;

	bool operator==(const FInteractionActionCandidate& Other) const
	{
		return Interactable.GetObject() == Other.Interactable.GetObject();
	}

	bool operator<(const FInteractionActionCandidate& Other) const
	{
		if (CachedWeight != Other.CachedWeight)
				return CachedWeight > Other.CachedWeight;
		return CachedDistanceSq < Other.CachedDistanceSq;
	}

	bool IsValid() const { return Interactable.GetInterface() != nullptr; }
};

#pragma endregion

#pragma region V5_InteractorSlot

class IMounteaInteractorInterface;

/**
 * Tracks a single interactor occupying a slot on an interactable.
 * Stored in the interactable's replicated ActiveInteractorSlots array.
 */
USTRUCT(BlueprintType)
struct FInteractorSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Slot")
	TScriptInterface<IMounteaInteractorInterface> Interactor;

	/** Tags the interactor satisfied when joining (subset of RequiredInteractorTags). */
	UPROPERTY(BlueprintReadOnly, Category="Slot")
	FGameplayTagContainer MatchedTags;

	/** Server world time at the moment this interactor joined. Used for Hold/Mash progress. */
	UPROPERTY(BlueprintReadOnly, Category="Slot")
	float JoinServerTime = 0.f;

	bool IsValid() const { return Interactor.GetInterface() != nullptr; }
};

#pragma endregion

#pragma region InteractorBaseSettings

USTRUCT(BlueprintType)
struct FInteractorBaseSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="InteractorSettings")
	EInteractorStateV2 DefaultInteractorState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="InteractorSettings")
	TEnumAsByte<ECollisionChannel> InteractorCollisionChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="InteractorSettings")
	FSafetyTracingSetup SafetyTracingSetup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="InteractorSettings")
	FGameplayTag InteractorTag;

	FInteractorBaseSettings()
		: DefaultInteractorState(EInteractorStateV2::EIS_Awake)
		, InteractorCollisionChannel(ECC_Visibility)
	{
		SafetyTracingSetup.StartSocketName = FName("head");
		SafetyTracingSetup.ActorMeshName = FName("CharacterMesh0");
		SafetyTracingSetup.SafetyTracingMode = ESafetyTracingMode::ESTM_Location;
	}
};

#pragma endregion

#pragma region InteractableBaseSettings

USTRUCT(BlueprintType)
struct FInteractableBaseSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="InteractableSettings", meta=(UIMin=-1, ClampMin=-1, Units="seconds", NoResetToDefault))
	float																													DefaultInteractionPeriod;

	UPROPERTY(EditAnywhere, Category="InteractableSettings", meta=(NoResetToDefault))
	EInteractableStateV2																						DefaultInteractableState;

	UPROPERTY(EditAnywhere, Category="InteractableSettings")
	ESetupType																										DefaultSetupType;
	
	UPROPERTY(EditAnywhere, Category="InteractableSettings", meta=(NoResetToDefault))
	TEnumAsByte<ECollisionChannel>																	DefaultCollisionChannel;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="InteractableSettings", meta=(NoResetToDefault))
	uint8																												DefaultInteractionHighlight : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="InteractableSettings", meta=(NoResetToDefault))
	FGameplayTag																								InteractableMainTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="InteractableSettings", meta=(NoResetToDefault))
	FInteractionHighlightSetup																				DefaultHighlightSetup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="InteractableSettings", meta=(NoResetToDefault))
	EInteractableLifecycle																						DefaultLifecycleMode;
	
	UPROPERTY(EditAnywhere, Category="InteractableSettings", meta=(NoResetToDefault, EditCondition = "DefaultLifecycleMode == EInteractableLifecycle::EIL_Cycled", UIMin=0.1, ClampMin=0.1, Units="Seconds"))
	float																													DefaultCooldownPeriod;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="InteractableSettings", meta=(NoResetToDefault, EditCondition = "DefaultLifecycleMode == EInteractableLifecycle::EIL_Cycled"))
	int32																												DefaultLifecycles;

	UPROPERTY(EditAnywhere, Category="InteractableSettings", meta=(UIMin=-1, ClampMin=-1, NoResetToDefault))
	int32																												DefaultInteractableWeight;


	FInteractableBaseSettings()
		: DefaultInteractionPeriod(3.f)
		  , DefaultInteractableState(EInteractableStateV2::EIS_Awake)
		  , DefaultSetupType(ESetupType::EST_Quick)
		  , DefaultCollisionChannel(ECC_Camera)
		  , DefaultInteractionHighlight(true)
		  , DefaultHighlightSetup(FInteractionHighlightSetup())
		  , DefaultLifecycleMode(EInteractableLifecycle::EIL_Cycled)
		  , DefaultCooldownPeriod(3.f)
		  , DefaultLifecycles(-1),
			DefaultInteractableWeight(1)
	{
	}
};

#pragma endregion