// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Components/Interactable/MounteaInteractableComponentBase.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "CommonInputTypeEnum.h"
#include "GameplayTagContainer.h"
#include "Interfaces/MounteaInteractorInterface.h"
#include "Interfaces/MounteaInteractionWidget.h"
#include "Helpers/MounteaInteractionSystemLog.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "Helpers/MounteaInteractionStatics.h"
#include "Helpers/MounteaInteractionSystemSettings.h"
#include "Settings/MounteaInteractorRuntimeConfig.h"
#include "Settings/MounteaInteractableRuntimeConfig.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "MounteaInteractableComponentBase"

#if WITH_EDITOR
FSimpleMulticastDelegate UMounteaInteractableComponentBase::OnInteractableComponentChanged;
#endif

UMounteaInteractableComponentBase::UMounteaInteractableComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);

	bInteractableInitialized = false;
	bInteractionHighlight = true;
	StencilID = 133;

	RemainingLifecycleCount = -1;
	InteractionStartServerTime = -1.f;

	SetWidgetSpace(EWidgetSpace::World);
	SetDrawAtDesiredSize(true);
	SetVisibility(false);
}

void UMounteaInteractableComponentBase::BeginPlay()
{
	Super::BeginPlay();

	if (!bInteractableInitialized)
	{
		Execute_SetDefaults(this);
		bInteractableInitialized = true;
	}

	InteractableState = DefaultInteractableState;
	RemainingLifecycleCount = LifecycleCount;
}

void UMounteaInteractableComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (const FInteractorSlot& slot : ActiveInteractorSlots)
	{
		if (slot.Interactor.GetInterface())
		{
			Execute_InteractorLost(this, slot.Interactor);
		}
	}
	ActiveInteractorSlots.Empty();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(Timer_Cooldown);
	}

	Super::EndPlay(EndPlayReason);
}

void UMounteaInteractableComponentBase::InitWidget()
{
	Super::InitWidget();

	if (UUserWidget* interactionWidget = GetUserWidgetObject())
	{
		if (interactionWidget->GetClass()->ImplementsInterface(UActorInteractionWidget::StaticClass()))
		{
			IActorInteractionWidget::Execute_SetWidgetMode(interactionWidget, EInteractionWidgetMode::EWM_Interaction);
		}
	}
}

void UMounteaInteractableComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if (!UE_BUILD_SHIPPING || WITH_EDITOR)
	DrawDebug();
#endif
}

void UMounteaInteractableComponentBase::OnComponentCreated()
{
	Super::OnComponentCreated();

#if WITH_EDITOR
	RequestEditorDefaults.Broadcast();
#endif
}

void UMounteaInteractableComponentBase::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR
	if (GIsEditor && !GIsPlayInEditorWorld)
	{
		RequestEditorDefaults.Broadcast();
	}
#endif
}

void UMounteaInteractableComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInteractableComponentBase, ActiveInteractorSlots);
	DOREPLIFETIME(UMounteaInteractableComponentBase, InteractableState);
	DOREPLIFETIME(UMounteaInteractableComponentBase, InteractionStartServerTime);
	DOREPLIFETIME(UMounteaInteractableComponentBase, InteractionPeriod);
	DOREPLIFETIME(UMounteaInteractableComponentBase, DefaultInteractableState);
	DOREPLIFETIME(UMounteaInteractableComponentBase, SetupType);
	DOREPLIFETIME(UMounteaInteractableComponentBase, CollisionChannel);
	DOREPLIFETIME(UMounteaInteractableComponentBase, CooldownPeriod);
	DOREPLIFETIME(UMounteaInteractableComponentBase, LifecycleMode);
	DOREPLIFETIME(UMounteaInteractableComponentBase, LifecycleCount);
	DOREPLIFETIME(UMounteaInteractableComponentBase, RemainingLifecycleCount);
	DOREPLIFETIME(UMounteaInteractableComponentBase, InteractionWeight);
	DOREPLIFETIME(UMounteaInteractableComponentBase, MaxInteractors);
	DOREPLIFETIME(UMounteaInteractableComponentBase, bStandaloneInteraction);
	DOREPLIFETIME(UMounteaInteractableComponentBase, WidgetRanges);
	DOREPLIFETIME(UMounteaInteractableComponentBase, RequiredInteractorTags);
	DOREPLIFETIME(UMounteaInteractableComponentBase, InteractableCompatibleTags);
	DOREPLIFETIME(UMounteaInteractableComponentBase, HighlightType);
	DOREPLIFETIME(UMounteaInteractableComponentBase, HighlightMaterial);
	DOREPLIFETIME(UMounteaInteractableComponentBase, InteractableData);
	DOREPLIFETIME(UMounteaInteractableComponentBase, InteractableName);
}

#pragma region IMounteaInteractableInterface_SlotAPI

TArray<FInteractorSlot> UMounteaInteractableComponentBase::GetActiveInteractorSlots_Implementation() const
{
	return ActiveInteractorSlots;
}

int32 UMounteaInteractableComponentBase::GetMaxInteractors_Implementation() const
{
	return MaxInteractors;
}

FGameplayTagContainer UMounteaInteractableComponentBase::GetRequiredInteractorTags_Implementation() const
{
	return RequiredInteractorTags;
}

bool UMounteaInteractableComponentBase::IsSlotAvailable_Implementation() const
{
	return ActiveInteractorSlots.Num() < MaxInteractors;
}

bool UMounteaInteractableComponentBase::HasInteractor_Implementation() const
{
	return !ActiveInteractorSlots.IsEmpty();
}

void UMounteaInteractableComponentBase::RequestInteractorJoin_Implementation(const TScriptInterface<IMounteaInteractorInterface>& RequestingInteractor)
{
	if (!RequestingInteractor.GetInterface())
		return;

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		RequestInteractorJoin_Server(RequestingInteractor);
		return;
	}

	// Re-entrant/duplicate join calls must not consume a second slot for the same interactor.
	const bool bAlreadyJoined = ActiveInteractorSlots.ContainsByPredicate([&RequestingInteractor](const FInteractorSlot& Slot)
	{
		return Slot.Interactor.GetObject() == RequestingInteractor.GetObject();
	});
	
	if (bAlreadyJoined) 
		return;

	if (!Execute_IsSlotAvailable(this))
	{
		OnInteractorRejected.Broadcast(RequestingInteractor);
		return;
	}

	if (!RequiredInteractorTags.IsEmpty())
	{
		const FGameplayTag interactorTag = IMounteaInteractorInterface::Execute_GetInteractorTag(RequestingInteractor.GetObject());
		FGameplayTagContainer interactorTags;
		interactorTags.AddTag(interactorTag);

		if (!interactorTags.HasAny(RequiredInteractorTags))
		{
			OnInteractorRejected.Broadcast(RequestingInteractor);
			return;
		}
	}

	FInteractorSlot newSlot;
	newSlot.Interactor = RequestingInteractor;
	newSlot.JoinServerTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	if (!RequiredInteractorTags.IsEmpty())
	{
		const FGameplayTag interactorTag = IMounteaInteractorInterface::Execute_GetInteractorTag(RequestingInteractor.GetObject());
		newSlot.MatchedTags.AddTag(interactorTag);
	}

	ActiveInteractorSlots.Add(newSlot);
	OnRep_ActiveInteractorSlots();

	// Reconnects the join path to the per-type interaction logic (Press/Automatic/Hold/Mash all
	// do their real work inside InteractionStarted_Implementation). Hover bypasses this function
	// entirely and calls Execute_InteractionStarted directly from cursor events, so it is unaffected.
	Execute_InteractionStarted(this, newSlot.JoinServerTime, RequestingInteractor);
}

void UMounteaInteractableComponentBase::RequestInteractorLeave_Implementation(const TScriptInterface<IMounteaInteractorInterface>& LeavingInteractor)
{
	if (!LeavingInteractor.GetInterface())
				return;

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		RequestInteractorLeave_Server(LeavingInteractor);
		return;
	}

	const int32 removed = ActiveInteractorSlots.RemoveAll([&](const FInteractorSlot& Slot)
	{
		return Slot.Interactor.GetObject() == LeavingInteractor.GetObject();
	});

	if (removed > 0)
	{
		OnRep_ActiveInteractorSlots();
	}
}

#pragma endregion

#pragma region IMounteaInteractableInterface_Ranges

FInteractableWidgetRanges UMounteaInteractableComponentBase::GetWidgetRanges_Implementation() const
{
	return WidgetRanges;
}

void UMounteaInteractableComponentBase::SetWidgetRanges_Implementation(const FInteractableWidgetRanges& NewRanges)
{
	WidgetRanges = NewRanges;
}

#pragma endregion

#pragma region IMounteaInteractableInterface_ActionCycling

bool UMounteaInteractableComponentBase::IsStandaloneInteraction_Implementation() const
{
	return bStandaloneInteraction;
}

void UMounteaInteractableComponentBase::SetStandaloneInteraction_Implementation(const bool bStandalone)
{
	bStandaloneInteraction = bStandalone;
}

#pragma endregion

#pragma region IMounteaInteractableInterface_StateMachine

bool UMounteaInteractableComponentBase::ActivateInteractable_Implementation(FString& ErrorMessage)
{
	if (InteractableState == EInteractableStateV3::EIS_Active)
	{
		ErrorMessage = TEXT("Already active");
		return false;
	}
	if (InteractableState == EInteractableStateV3::EIS_Disabled || InteractableState == EInteractableStateV3::EIS_Completed)
	{
		ErrorMessage = TEXT("Cannot activate from Disabled/Completed state");
		return false;
	}

	Execute_SetState(this, EInteractableStateV3::EIS_Active);
	return true;
}

bool UMounteaInteractableComponentBase::WakeUpInteractable_Implementation(FString& ErrorMessage)
{
	if (InteractableState == EInteractableStateV3::EIS_Awake)
	{
		ErrorMessage = TEXT("Already awake");
		return false;
	}
	if (InteractableState == EInteractableStateV3::EIS_Disabled || InteractableState == EInteractableStateV3::EIS_Completed)
	{
		ErrorMessage = TEXT("Cannot wake from Disabled/Completed state");
		return false;
	}

	Execute_SetState(this, EInteractableStateV3::EIS_Awake);
	return true;
}

bool UMounteaInteractableComponentBase::CompleteInteractable_Implementation(FString& ErrorMessage)
{
	if (InteractableState != EInteractableStateV3::EIS_Active)
	{
		ErrorMessage = TEXT("Not active");
		return false;
	}

	if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
	{
		if (RemainingLifecycleCount > 0)
		{
			--RemainingLifecycleCount;
		}

		if (RemainingLifecycleCount == 0)
		{
			Execute_SetState(this, EInteractableStateV3::EIS_Completed);
			Execute_InteractionLifecycleCompleted(this);
		}
		else
		{
			Execute_TriggerCooldown(this);
		}
	}
	else
	{
		Execute_SetState(this, EInteractableStateV3::EIS_Completed);
		Execute_InteractionLifecycleCompleted(this);
	}

	return true;
}

void UMounteaInteractableComponentBase::DeactivateInteractable_Implementation()
{
	Execute_SetState(this, EInteractableStateV3::EIS_Disabled);

	ActiveInteractorSlots.Empty();
	OnRep_ActiveInteractorSlots();

	InteractionStartServerTime = -1.f;
}

void UMounteaInteractableComponentBase::PauseInteraction_Implementation(const float ExpirationTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	Execute_SetState(this, EInteractableStateV3::EIS_Paused);
}

bool UMounteaInteractableComponentBase::CanInteract_Implementation() const
{
	return InteractableState == EInteractableStateV3::EIS_Awake || InteractableState == EInteractableStateV3::EIS_Active;
}

bool UMounteaInteractableComponentBase::CanBeTriggered_Implementation() const
{
	return Execute_IsSlotAvailable(this) && Execute_CanInteract(this);
}

bool UMounteaInteractableComponentBase::IsInteracting_Implementation() const
{
	return InteractableState == EInteractableStateV3::EIS_Active && !ActiveInteractorSlots.IsEmpty();
}

EInteractableStateV3 UMounteaInteractableComponentBase::GetDefaultState_Implementation() const
{
	return DefaultInteractableState;
}

void UMounteaInteractableComponentBase::SetDefaultState_Implementation(const EInteractableStateV3 NewState)
{
	DefaultInteractableState = NewState;
}

EInteractableStateV3 UMounteaInteractableComponentBase::GetState_Implementation() const
{
	return InteractableState;
}

void UMounteaInteractableComponentBase::SetState_Implementation(const EInteractableStateV3 NewState)
{
	if (InteractableState == NewState)
				return;

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		SetState_Server(NewState);
		return;
	}

	InteractableState = NewState;
	OnRep_InteractableState();
}

bool UMounteaInteractableComponentBase::TriggerCooldown_Implementation()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
				return false;

	Execute_SetState(this, EInteractableStateV3::EIS_Cooldown);

	if (CooldownPeriod > 0.f && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(Timer_Cooldown, [this]()
		{
			Execute_InteractionCooldownCompleted(this);
			FString err;
			Execute_WakeUpInteractable(this, err);
		}, CooldownPeriod, false);
	}

	return true;
}

#pragma endregion

#pragma region IMounteaInteractableInterface_LifecycleEvents

void UMounteaInteractableComponentBase::InteractorFound_Implementation(const TScriptInterface<IMounteaInteractorInterface>& FoundInteractor)
{
	OnInteractorFound.Broadcast(FoundInteractor);
	Execute_OnInteractorFoundEvent(this, FoundInteractor);
}

void UMounteaInteractableComponentBase::InteractorLost_Implementation(const TScriptInterface<IMounteaInteractorInterface>& LostInteractor)
{
	OnInteractorLost.Broadcast(LostInteractor);
	Execute_OnInteractorLostEvent(this, LostInteractor);
}

void UMounteaInteractableComponentBase::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InteractionStartServerTime = GetWorld() ? GetWorld()->GetTimeSeconds() : TimeStarted;
	}

	OnInteractionStarted.Broadcast(TimeStarted, CausingInteractor);
	Execute_OnInteractionStartedEvent(this, TimeStarted, CausingInteractor);
}

void UMounteaInteractableComponentBase::InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InteractionStartServerTime = -1.f;
	}

	OnInteractionStopped.Broadcast(TimeStarted, CausingInteractor);
	Execute_OnInteractionStoppedEvent(this, TimeStarted, CausingInteractor);
}

void UMounteaInteractableComponentBase::InteractionCompleted_Implementation(const float& TimeCompleted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	OnInteractionCompleted.Broadcast(TimeCompleted, CausingInteractor);
	Execute_OnInteractionCompletedEvent(this, TimeCompleted, CausingInteractor);
}

void UMounteaInteractableComponentBase::InteractionCycleCompleted_Implementation(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	OnInteractionCycleCompleted.Broadcast(CompletedTime, CyclesRemaining, CausingInteractor);
	Execute_OnInteractionCycleCompletedEvent(this, CompletedTime, CyclesRemaining, CausingInteractor);
}

void UMounteaInteractableComponentBase::InteractionCanceled_Implementation()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InteractionStartServerTime = -1.f;
	}

	OnInteractionCanceled.Broadcast();
	Execute_OnInteractionCanceledEvent(this);
}

void UMounteaInteractableComponentBase::InteractionLifecycleCompleted_Implementation()
{
	OnLifecycleCompleted.Broadcast();
	Execute_OnLifecycleCompletedEvent(this);
}

void UMounteaInteractableComponentBase::InteractionCooldownCompleted_Implementation()
{
	OnCooldownCompleted.Broadcast();
	Execute_OnCooldownCompletedEvent(this);
}

#pragma endregion

#pragma region IMounteaInteractableInterface_Properties

float UMounteaInteractableComponentBase::GetInteractionProgress_Implementation() const
{
	if (InteractionStartServerTime < 0.f || InteractionPeriod <= 0.f)
				return 0.f;
	if (!GetWorld())
				return 0.f;

	const float elapsed = GetWorld()->GetTimeSeconds() - InteractionStartServerTime;
	return FMath::Clamp(elapsed / InteractionPeriod, 0.f, 1.f);
}

float UMounteaInteractableComponentBase::GetInteractionPeriod_Implementation() const
{
	return InteractionPeriod;
}

void UMounteaInteractableComponentBase::SetInteractionPeriod_Implementation(const float NewPeriod)
{
	InteractionPeriod = FMath::Max(-1.f, NewPeriod);
}

int32 UMounteaInteractableComponentBase::GetInteractableWeight_Implementation() const
{
	return InteractionWeight;
}

void UMounteaInteractableComponentBase::SetInteractableWeight_Implementation(const int32 NewWeight)
{
	InteractionWeight = FMath::Max(0, NewWeight);
	CachedInteractionWeight = InteractionWeight;
	OnInteractableWeightChanged.Broadcast(InteractionWeight);
}

AActor* UMounteaInteractableComponentBase::GetInteractableOwner_Implementation() const
{
	return GetOwner();
}

AActor* UMounteaInteractableComponentBase::GetOwningActor_Implementation() const
{
	return GetOwner();
}

ECollisionChannel UMounteaInteractableComponentBase::GetCollisionChannel_Implementation() const
{
	return CollisionChannel.GetValue();
}

void UMounteaInteractableComponentBase::SetCollisionChannel_Implementation(const TEnumAsByte<ECollisionChannel>& NewChannel)
{
	CollisionChannel = NewChannel;
	OnInteractableCollisionChannelChanged.Broadcast(NewChannel.GetValue());
}

EInteractableLifecycle UMounteaInteractableComponentBase::GetLifecycleMode_Implementation() const
{
	return LifecycleMode;
}

void UMounteaInteractableComponentBase::SetLifecycleMode_Implementation(const EInteractableLifecycle& NewMode)
{
	LifecycleMode = NewMode;
	OnLifecycleModeChanged.Broadcast(NewMode);
}

int32 UMounteaInteractableComponentBase::GetLifecycleCount_Implementation() const
{
	return LifecycleCount;
}

void UMounteaInteractableComponentBase::SetLifecycleCount_Implementation(const int32 NewLifecycleCount)
{
	LifecycleCount = NewLifecycleCount;
	RemainingLifecycleCount = NewLifecycleCount;
	OnLifecycleCountChanged.Broadcast(NewLifecycleCount);
}

int32 UMounteaInteractableComponentBase::GetRemainingLifecycleCount_Implementation() const
{
	return RemainingLifecycleCount;
}

float UMounteaInteractableComponentBase::GetCooldownPeriod_Implementation() const
{
	return CooldownPeriod;
}

void UMounteaInteractableComponentBase::SetCooldownPeriod_Implementation(const float NewCooldownPeriod)
{
	CooldownPeriod = FMath::Max(0.f, NewCooldownPeriod);
	OnCooldownPeriodChanged.Broadcast(CooldownPeriod);
}

ETimingComparison UMounteaInteractableComponentBase::GetComparisonMethod_Implementation() const
{
	return ComparisonMethod;
}

void UMounteaInteractableComponentBase::SetComparisonMethod_Implementation(const ETimingComparison Value)
{
	ComparisonMethod = Value;
}

FText UMounteaInteractableComponentBase::GetInteractableName_Implementation() const
{
	return InteractableName;
}

void UMounteaInteractableComponentBase::SetInteractableName_Implementation(const FText& NewName)
{
	InteractableName = NewName;
}

FDataTableRowHandle UMounteaInteractableComponentBase::GetInteractableData_Implementation() const
{
	return InteractableData;
}

void UMounteaInteractableComponentBase::SetInteractableData_Implementation(FDataTableRowHandle NewData)
{
	InteractableData = NewData;
}

#pragma endregion

#pragma region IMounteaInteractableInterface_Setup

bool UMounteaInteractableComponentBase::DoesAutoSetup_Implementation() const
{
	return SetupType != ESetupType::EST_None;
}

void UMounteaInteractableComponentBase::ToggleAutoSetup_Implementation(const ESetupType& NewValue)
{
	SetupType = NewValue;
}

void UMounteaInteractableComponentBase::FindAndAddCollisionShapes_Implementation()
{
	if (!GetOwner() || SetupType == ESetupType::EST_None)
				return;

	TArray<UPrimitiveComponent*> primitives;

	switch (SetupType)
	{
		case ESetupType::EST_Quick:
		{
			UPrimitiveComponent* parent = Cast<UPrimitiveComponent>(GetAttachParent());
			if (IsValid(parent))
				primitives.Add(parent);
			break;
		}
		case ESetupType::EST_AllParent:
		{
			USceneComponent* current = GetAttachParent();
			while (current)
			{
				if (UPrimitiveComponent* prim = Cast<UPrimitiveComponent>(current))
					primitives.Add(prim);
				current = current->GetAttachParent();
			}
			break;
		}
		case ESetupType::EST_FullAll:
		default:
		{
			GetOwner()->GetComponents<UPrimitiveComponent>(primitives);
			break;
		}
	}

	for (UPrimitiveComponent* prim : primitives)
	{
		if (!IsValid(prim))
				continue;
		if (CollisionOverrides.Num() > 0 && !CollisionOverrides.Contains(prim->GetFName()))
				continue;
		if (!CollisionComponents.Contains(prim))
		{
			Execute_AddCollisionComponent(this, prim);
		}
	}
}

void UMounteaInteractableComponentBase::FindAndAddHighlightableMeshes_Implementation()
{
	if (!GetOwner() || SetupType == ESetupType::EST_None)
				return;

	TArray<UMeshComponent*> meshes;

	switch (SetupType)
	{
		case ESetupType::EST_Quick:
		{
			UMeshComponent* parent = Cast<UMeshComponent>(GetAttachParent());
			if (IsValid(parent))
				meshes.Add(parent);
			break;
		}
		case ESetupType::EST_AllParent:
		{
			USceneComponent* current = GetAttachParent();
			while (current)
			{
				if (UMeshComponent* mesh = Cast<UMeshComponent>(current))
					meshes.Add(mesh);
				current = current->GetAttachParent();
			}
			break;
		}
		case ESetupType::EST_FullAll:
		default:
		{
			GetOwner()->GetComponents<UMeshComponent>(meshes);
			break;
		}
	}

	for (UMeshComponent* mesh : meshes)
	{
		if (!IsValid(mesh))
				continue;
		if (HighlightableOverrides.Num() > 0 && !HighlightableOverrides.Contains(mesh->GetFName()))
				continue;
		if (!HighlightableComponents.Contains(mesh))
		{
			Execute_AddHighlightableComponent(this, mesh);
		}
	}
}

void UMounteaInteractableComponentBase::SetDefaults_Implementation()
{
	if (const UMounteaInteractionSystemSettings* settings = GetDefault<UMounteaInteractionSystemSettings>())
	{
		if (const UMounteaInteractableRuntimeConfig* config = settings->GetInteractableConfig())
		{
			InteractionPeriod = config->DefaultInteractionPeriod;
			DefaultInteractableState = config->DefaultInteractableState;
			LifecycleMode = config->DefaultLifecycleMode;
			CooldownPeriod = config->DefaultCooldownPeriod;
			InteractionWeight = config->DefaultInteractableWeight;
			MaxInteractors = config->DefaultMaxInteractors;
			WidgetRanges = config->DefaultWidgetRanges;
			bInteractionHighlight = config->bDefaultHighlightEnabled;
			HighlightType = config->DefaultHighlightSetup.HighlightType;
			HighlightMaterial = config->DefaultHighlightSetup.HighlightMaterial;

			bool bFound = false;
			const ECollisionChannel resolvedChannel = UMounteaInteractionStatics::ResolveInteractionChannel(config->OverlapCollisionChannelName, bFound);
			if (bFound)
				CollisionChannel = resolvedChannel;
		}
	}

	CachedInteractionWeight = InteractionWeight;

	if (Execute_DoesAutoSetup(this))
	{
		Execute_FindAndAddCollisionShapes(this);
		Execute_FindAndAddHighlightableMeshes(this);
	}

	for (UPrimitiveComponent* Prim : CollisionComponents)
	{
		if (IsValid(Prim))
		{
			Execute_BindCollisionShape(this, Prim);
		}
	}
}

#pragma endregion

#pragma region IMounteaInteractableInterface_Highlight

void UMounteaInteractableComponentBase::StartHighlight_Implementation()
{
	if (!bInteractionHighlight)
				return;

	for (UMeshComponent* Mesh : HighlightableComponents)
	{
		if (!IsValid(Mesh))
				continue;

		if (HighlightType == EHighlightType::EHT_OverlayMaterial && IsValid(HighlightMaterial))
		{
			Mesh->SetOverlayMaterial(HighlightMaterial);
		}
		else if (HighlightType == EHighlightType::EHT_PostProcessing)
		{
			Mesh->SetRenderCustomDepth(true);
			Mesh->SetCustomDepthStencilValue(StencilID);
		}
	}

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		StartHighlight_Client();
	}
}

void UMounteaInteractableComponentBase::StopHighlight_Implementation()
{
	for (UMeshComponent* Mesh : HighlightableComponents)
	{
		if (!IsValid(Mesh))
				continue;
		Mesh->SetOverlayMaterial(nullptr);
		Mesh->SetRenderCustomDepth(false);
	}

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		StopHighlight_Client();
	}
}

EHighlightType UMounteaInteractableComponentBase::GetHighlightType_Implementation() const
{
	return HighlightType;
}

void UMounteaInteractableComponentBase::SetHighlightType_Implementation(const EHighlightType NewHighlightType)
{
	HighlightType = NewHighlightType;
	OnHighlightTypeChanged.Broadcast(NewHighlightType);
}

UMaterialInterface* UMounteaInteractableComponentBase::GetHighlightMaterial_Implementation() const
{
	return HighlightMaterial;
}

void UMounteaInteractableComponentBase::SetHighlightMaterial_Implementation(UMaterialInterface* NewHighlightMaterial)
{
	HighlightMaterial = NewHighlightMaterial;
	OnHighlightMaterialChanged.Broadcast(NewHighlightMaterial);
}

#pragma endregion

#pragma region IMounteaInteractableInterface_Collision

TArray<UPrimitiveComponent*> UMounteaInteractableComponentBase::GetCollisionComponents_Implementation() const
{
	TArray<UPrimitiveComponent*> out;
	for (auto p : CollisionComponents) { if (IsValid(p)) out.Add(p); }
	return out;
}

void UMounteaInteractableComponentBase::AddCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp)
{
	if (!IsValid(CollisionComp) || CollisionComponents.Contains(CollisionComp))
				return;
	CollisionComponents.Add(CollisionComp);
	Execute_BindCollisionShape(this, CollisionComp);
}

void UMounteaInteractableComponentBase::AddCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& InComponents)
{
	for (UPrimitiveComponent* Comp : InComponents) Execute_AddCollisionComponent(this, Comp);
}

void UMounteaInteractableComponentBase::RemoveCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp)
{
	if (!IsValid(CollisionComp))
				return;
	Execute_UnbindCollisionShape(this, CollisionComp);
	CollisionComponents.Remove(CollisionComp);
}

void UMounteaInteractableComponentBase::RemoveCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& InComponents)
{
	for (UPrimitiveComponent* Comp : InComponents) Execute_RemoveCollisionComponent(this, Comp);
}

TArray<UMeshComponent*> UMounteaInteractableComponentBase::GetHighlightableComponents_Implementation() const
{
	TArray<UMeshComponent*> out;
	for (auto m : HighlightableComponents) { if (IsValid(m)) out.Add(m); }
	return out;
}

void UMounteaInteractableComponentBase::AddHighlightableComponent_Implementation(UMeshComponent* HighlightableComp)
{
	if (!IsValid(HighlightableComp) || HighlightableComponents.Contains(HighlightableComp))
				return;
	HighlightableComponents.Add(HighlightableComp);
	Execute_BindHighlightableMesh(this, HighlightableComp);
}

void UMounteaInteractableComponentBase::AddHighlightableComponents_Implementation(const TArray<UMeshComponent*>& InComponents)
{
	for (UMeshComponent* Mesh : InComponents) Execute_AddHighlightableComponent(this, Mesh);
}

void UMounteaInteractableComponentBase::RemoveHighlightableComponent_Implementation(UMeshComponent* HighlightableComp)
{
	if (!IsValid(HighlightableComp))
				return;
	Execute_UnbindHighlightableMesh(this, HighlightableComp);
	HighlightableComponents.Remove(HighlightableComp);
}

void UMounteaInteractableComponentBase::RemoveHighlightableComponents_Implementation(const TArray<UMeshComponent*>& InComponents)
{
	for (UMeshComponent* Mesh : InComponents) Execute_RemoveHighlightableComponent(this, Mesh);
}

TArray<FName> UMounteaInteractableComponentBase::GetCollisionOverrides_Implementation() const
{
	return CollisionOverrides;
}

TArray<FName> UMounteaInteractableComponentBase::GetHighlightableOverrides_Implementation() const
{
	return HighlightableOverrides;
}

void UMounteaInteractableComponentBase::BindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const
{
	if (!IsValid(PrimitiveComponent))
				return;

	// Snapshot pre-setup collision state so UnbindCollisionShape can fully restore it
	FCollisionShapeCache cache;
	cache.bGenerateOverlapEvents = PrimitiveComponent->GetGenerateOverlapEvents();
	cache.CollisionEnabled       = PrimitiveComponent->GetCollisionEnabled();
	cache.ResponseContainer      = PrimitiveComponent->GetCollisionResponseToChannels();
	CachedCollisionShapesSettings.Add(PrimitiveComponent, cache);

	// Resolve named channels from the interactor runtime config (canonical source of channel names)
	FName overlapChannelName = TEXT("InteractionOverlap");
	FName traceChannelName   = TEXT("InteractionTrace");
	if (const UMounteaInteractionSystemSettings* settings = GetDefault<UMounteaInteractionSystemSettings>())
	{
		if (const UMounteaInteractorRuntimeConfig* cfg = settings->GetInteractorConfig())
		{
			overlapChannelName = cfg->OverlapCollisionChannelName;
			traceChannelName   = cfg->TraceCollisionChannelName;
		}
	}

	bool bOverlapFound = false;
	bool bTraceFound   = false;
	const ECollisionChannel overlapChannel = UMounteaInteractionStatics::ResolveInteractionChannel(overlapChannelName, bOverlapFound);
	const ECollisionChannel traceChannel   = UMounteaInteractionStatics::ResolveInteractionChannel(traceChannelName,   bTraceFound);

	// Apply minimal interaction profile: ignore all, overlap on overlap channel, block on trace channel
	PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PrimitiveComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	if (bOverlapFound) PrimitiveComponent->SetCollisionResponseToChannel(overlapChannel, ECR_Overlap);
	if (bTraceFound)   PrimitiveComponent->SetCollisionResponseToChannel(traceChannel,   ECR_Block);
	PrimitiveComponent->SetGenerateOverlapEvents(true);

	PrimitiveComponent->OnComponentBeginOverlap.AddUniqueDynamic(const_cast<UMounteaInteractableComponentBase*>(this), &UMounteaInteractableComponentBase::OnInteractableBeginOverlap);
	PrimitiveComponent->OnComponentEndOverlap.AddUniqueDynamic(const_cast<UMounteaInteractableComponentBase*>(this), &UMounteaInteractableComponentBase::OnInteractableStopOverlap);
	PrimitiveComponent->OnComponentHit.AddUniqueDynamic(const_cast<UMounteaInteractableComponentBase*>(this), &UMounteaInteractableComponentBase::OnInteractableTraced);
}

void UMounteaInteractableComponentBase::UnbindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const
{
	if (!IsValid(PrimitiveComponent))
				return;

	PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(const_cast<UMounteaInteractableComponentBase*>(this), &UMounteaInteractableComponentBase::OnInteractableBeginOverlap);
	PrimitiveComponent->OnComponentEndOverlap.RemoveDynamic(const_cast<UMounteaInteractableComponentBase*>(this), &UMounteaInteractableComponentBase::OnInteractableStopOverlap);
	PrimitiveComponent->OnComponentHit.RemoveDynamic(const_cast<UMounteaInteractableComponentBase*>(this), &UMounteaInteractableComponentBase::OnInteractableTraced);

	// Restore pre-interaction collision state from cache
	if (const FCollisionShapeCache* cache = CachedCollisionShapesSettings.Find(PrimitiveComponent))
	{
		PrimitiveComponent->SetCollisionEnabled(cache->CollisionEnabled);
		PrimitiveComponent->SetGenerateOverlapEvents(cache->bGenerateOverlapEvents);
		PrimitiveComponent->SetCollisionResponseToChannels(cache->ResponseContainer);
		CachedCollisionShapesSettings.Remove(PrimitiveComponent);
	}
}

void UMounteaInteractableComponentBase::BindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const
{
}

void UMounteaInteractableComponentBase::UnbindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const
{
}

#pragma endregion

#pragma region IMounteaInteractableInterface_Tags

FGameplayTagContainer UMounteaInteractableComponentBase::GetInteractableCompatibleTags_Implementation() const
{
	return InteractableCompatibleTags;
}

void UMounteaInteractableComponentBase::SetInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags)
{
	InteractableCompatibleTags = Tags;
}

void UMounteaInteractableComponentBase::AddInteractableCompatibleTag_Implementation(const FGameplayTag& Tag)
{
	InteractableCompatibleTags.AddTag(Tag);
}

void UMounteaInteractableComponentBase::AddInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags)
{
	InteractableCompatibleTags.AppendTags(Tags);
}

void UMounteaInteractableComponentBase::RemoveInteractableCompatibleTag_Implementation(const FGameplayTag& Tag)
{
	InteractableCompatibleTags.RemoveTag(Tag);
}

void UMounteaInteractableComponentBase::RemoveInteractableCompatibleTags_Implementation(const FGameplayTagContainer& Tags)
{
	InteractableCompatibleTags.RemoveTags(Tags);
}

void UMounteaInteractableComponentBase::ClearInteractableCompatibleTags_Implementation()
{
	InteractableCompatibleTags.Reset();
}

#pragma endregion

#pragma region IMounteaInteractableInterface_IgnoredClasses

TArray<TSoftClassPtr<UObject>> UMounteaInteractableComponentBase::GetIgnoredClasses_Implementation() const
{
	return IgnoredClasses;
}

void UMounteaInteractableComponentBase::SetIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& NewIgnoredClasses)
{
	IgnoredClasses = NewIgnoredClasses;
}

void UMounteaInteractableComponentBase::AddIgnoredClass_Implementation(const TSoftClassPtr<UObject>& AddIgnoredClass)
{
	IgnoredClasses.AddUnique(AddIgnoredClass);
}

void UMounteaInteractableComponentBase::AddIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& AddIgnoredClasses)
{
	for (const auto& C : AddIgnoredClasses) IgnoredClasses.AddUnique(C);
}

void UMounteaInteractableComponentBase::RemoveIgnoredClass_Implementation(const TSoftClassPtr<UObject>& RemoveIgnoredClass)
{
	IgnoredClasses.Remove(RemoveIgnoredClass);
}

void UMounteaInteractableComponentBase::RemoveIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& RemoveIgnoredClasses)
{
	for (const auto& C : RemoveIgnoredClasses) IgnoredClasses.Remove(C);
}

#pragma endregion

#pragma region IMounteaInteractableInterface_Dependencies

void UMounteaInteractableComponentBase::AddInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency)
{
	if (!InteractionDependency.GetInterface())
				return;
	InteractionDependencies.AddUnique(InteractionDependency);
	OnInteractableDependencyChanged.Broadcast(InteractionDependency);
}

void UMounteaInteractableComponentBase::RemoveInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency)
{
	InteractionDependencies.Remove(InteractionDependency);
	OnInteractableDependencyChanged.Broadcast(InteractionDependency);
}

TArray<TScriptInterface<IMounteaInteractableInterface>> UMounteaInteractableComponentBase::GetInteractionDependencies_Implementation() const
{
	return InteractionDependencies;
}

void UMounteaInteractableComponentBase::ProcessDependencies_Implementation()
{
	for (const TScriptInterface<IMounteaInteractableInterface>& dep : InteractionDependencies)
	{
		if (!dep.GetInterface())
				continue;
		const EInteractableStateV3 depState = IMounteaInteractableInterface::Execute_GetState(dep.GetObject());
		if (depState != EInteractableStateV3::EIS_Completed)
		{
			Execute_DeactivateInteractable(this);
			return;
		}
	}
}

void UMounteaInteractableComponentBase::InteractableDependencyStartedCallback_Implementation(const TScriptInterface<IMounteaInteractableInterface>& NewMaster)
{
	Execute_DeactivateInteractable(this);
	OnInteractableDependencyStarted.Broadcast(NewMaster);
}

void UMounteaInteractableComponentBase::InteractableDependencyStoppedCallback_Implementation(const TScriptInterface<IMounteaInteractableInterface>& FormerMaster)
{
	FString err;
	Execute_WakeUpInteractable(this, err);
	OnInteractableDependencyStopped.Broadcast(FormerMaster);
}

#pragma endregion

#pragma region IMounteaInteractableInterface_Widget

void UMounteaInteractableComponentBase::ToggleWidgetVisibility_Implementation(const bool IsVisible)
{
	SetVisibility(IsVisible);
	OnInteractableWidgetVisibilityChanged.Broadcast(IsVisible);
}

#pragma endregion

#pragma region IMounteaInteractableInterface_Debug

void UMounteaInteractableComponentBase::ToggleDebug_Implementation()
{
	DebugSettings.DebugMode = !DebugSettings.DebugMode;
}

FDebugSettings UMounteaInteractableComponentBase::GetDebugSettings_Implementation() const
{
	return DebugSettings;
}

FString UMounteaInteractableComponentBase::ToString_Implementation() const
{
	return FString::Printf(TEXT("Interactable: %s | State: %s | Slots: %d/%d"),
		*GetName(),
		*StaticEnum<EInteractableStateV3>()->GetNameStringByValue(static_cast<int64>(InteractableState)),
		ActiveInteractorSlots.Num(),
		MaxInteractors);
}

void UMounteaInteractableComponentBase::OnInputDeviceChanged_Implementation(const ECommonInputType DeviceType, const FName& DeviceName)
{
	OnInteractionDeviceChanged.Broadcast(DeviceType, DeviceName);
}

#pragma endregion

#pragma region Networking

void UMounteaInteractableComponentBase::RequestInteractorJoin_Server_Implementation(const TScriptInterface<IMounteaInteractorInterface>& RequestingInteractor)
{
	Execute_RequestInteractorJoin(this, RequestingInteractor);
}

void UMounteaInteractableComponentBase::RequestInteractorLeave_Server_Implementation(const TScriptInterface<IMounteaInteractorInterface>& LeavingInteractor)
{
	Execute_RequestInteractorLeave(this, LeavingInteractor);
}

void UMounteaInteractableComponentBase::SetState_Server_Implementation(const EInteractableStateV3 NewState)
{
	Execute_SetState(this, NewState);
}

void UMounteaInteractableComponentBase::OnRep_ActiveInteractorSlots()
{
	OnInteractorSlotsChanged.Broadcast(ActiveInteractorSlots);

	if (UUserWidget* interactionWidget = GetUserWidgetObject())
	{
		if (interactionWidget->GetClass()->ImplementsInterface(UActorInteractionWidget::StaticClass()))
		{
			IActorInteractionWidget::Execute_SetActionCount(interactionWidget, ActiveInteractorSlots.Num());
		}
	}
}

void UMounteaInteractableComponentBase::OnRep_InteractionStartServerTime()
{
	// Clients derive progress from InteractionStartServerTime on each GetInteractionProgress() call
}

void UMounteaInteractableComponentBase::OnRep_InteractableState()
{
	OnInteractableStateChanged.Broadcast(InteractableState);

	const bool bShouldBeVisible =
		InteractableState == EInteractableStateV3::EIS_Active ||
		InteractableState == EInteractableStateV3::EIS_Awake;

	Execute_ToggleWidgetVisibility(this, bShouldBeVisible);
}

void UMounteaInteractableComponentBase::StartHighlight_Client_Implementation()
{
	Execute_StartHighlight(this);
}

void UMounteaInteractableComponentBase::StopHighlight_Client_Implementation()
{
	Execute_StopHighlight(this);
}

void UMounteaInteractableComponentBase::ShowWidget_Client_Implementation()
{
	Execute_ToggleWidgetVisibility(this, true);
}

void UMounteaInteractableComponentBase::HideWidget_Client_Implementation()
{
	Execute_ToggleWidgetVisibility(this, false);
}

#pragma endregion

#pragma region InternalFunctions

void UMounteaInteractableComponentBase::ProcessToggleActive(const bool bIsEnabled)
{
	if (bIsEnabled)
	{
		Execute_StartHighlight(this);
		Execute_ToggleWidgetVisibility(this, true);
	}
	else
	{
		Execute_StopHighlight(this);
		Execute_ToggleWidgetVisibility(this, false);
	}
}

void UMounteaInteractableComponentBase::ProcessStartHighlight()
{
	Execute_StartHighlight(this);
}

void UMounteaInteractableComponentBase::ProcessStopHighlight()
{
	Execute_StopHighlight(this);
}

void UMounteaInteractableComponentBase::ProcessShowWidget()
{
	Execute_ToggleWidgetVisibility(this, true);
}

void UMounteaInteractableComponentBase::ProcessHideWidget()
{
	Execute_ToggleWidgetVisibility(this, false);
}

void UMounteaInteractableComponentBase::OnInputModeChanged(ECommonInputType CommonInput)
{
	Execute_OnInputDeviceChanged(this, CommonInput, NAME_None);
}

#pragma endregion

#pragma region OverlapCallbacks

void UMounteaInteractableComponentBase::OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor))
				return;

	for (const TSoftClassPtr<UObject>& ignoredClass : IgnoredClasses)
	{
		if (ignoredClass.IsValid() && OtherActor->IsA(ignoredClass.Get()))
				return;
	}

	TScriptInterface<IMounteaInteractorInterface> interactor;
	if (OtherActor->GetClass()->ImplementsInterface(UMounteaInteractorInterface::StaticClass()))
	{
		interactor.SetObject(OtherActor);
		interactor.SetInterface(Cast<IMounteaInteractorInterface>(OtherActor));
	}
	else
	{
		UActorComponent* interactorComp = OtherActor->FindComponentByInterface(UMounteaInteractorInterface::StaticClass());
		if (IsValid(interactorComp))
		{
			interactor.SetObject(interactorComp);
			interactor.SetInterface(Cast<IMounteaInteractorInterface>(interactorComp));
		}
	}

	if (interactor.GetInterface())
	{
		Execute_InteractorFound(this, interactor);
	}
}

void UMounteaInteractableComponentBase::OnInteractableStopOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValid(OtherActor))
				return;

	TScriptInterface<IMounteaInteractorInterface> interactor;
	if (OtherActor->GetClass()->ImplementsInterface(UMounteaInteractorInterface::StaticClass()))
	{
		interactor.SetObject(OtherActor);
		interactor.SetInterface(Cast<IMounteaInteractorInterface>(OtherActor));
	}
	else
	{
		UActorComponent* interactorComp = OtherActor->FindComponentByInterface(UMounteaInteractorInterface::StaticClass());
		if (IsValid(interactorComp))
		{
			interactor.SetObject(interactorComp);
			interactor.SetInterface(Cast<IMounteaInteractorInterface>(interactorComp));
		}
	}

	if (interactor.GetInterface())
	{
		Execute_RequestInteractorLeave(this, interactor);
		Execute_InteractorLost(this, interactor);
	}
}

void UMounteaInteractableComponentBase::OnInteractableTraced(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	OnInteractableBeginOverlap(HitComponent, OtherActor, OtherComp, -1, false, Hit);
}

#pragma endregion

#pragma region Editor

#if WITH_EDITOR

void UMounteaInteractableComponentBase::SetDefaultValues()
{
	Execute_SetDefaults(this);
	RequestEditorDefaults.Broadcast();
}

void UMounteaInteractableComponentBase::ResetDefaults()
{
	Execute_SetDefaults(this);
}

void UMounteaInteractableComponentBase::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	// Re-apply collision setup when setup-related properties change in the editor
	static const TSet<FName> setupProps = { GET_MEMBER_NAME_CHECKED(UMounteaInteractableComponentBase, SetupType),
	                                        GET_MEMBER_NAME_CHECKED(UMounteaInteractableComponentBase, CollisionChannel),
	                                        GET_MEMBER_NAME_CHECKED(UMounteaInteractableComponentBase, CollisionOverrides) };
	const FName changedProp = PropertyChangedEvent.GetPropertyName();
	if (setupProps.Contains(changedProp))
	{
		Execute_SetDefaults(this);
	}

	OnInteractableComponentChanged.Broadcast();
}

EDataValidationResult UMounteaInteractableComponentBase::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult result = Super::IsDataValid(Context);

	if (WidgetRanges.InteractionRange >= WidgetRanges.AttentionRange)
	{
		Context.AddError(NSLOCTEXT("InteractableBase", "InvalidRanges",
			"InteractionRange must be less than AttentionRange. Interactors would never see the Interaction widget."));
		result = EDataValidationResult::Invalid;
	}

	if (MaxInteractors < 1)
	{
		Context.AddError(NSLOCTEXT("InteractableBase", "InvalidMaxInteractors",
			"MaxInteractors must be >= 1."));
		result = EDataValidationResult::Invalid;
	}

	if (const TSubclassOf<UUserWidget> wClass = WidgetClass)
	{
		if (!wClass->ImplementsInterface(UActorInteractionWidget::StaticClass()))
		{
			Context.AddError(NSLOCTEXT("InteractableBase", "BadWidgetClass",
				"Widget class does not implement IActorInteractionWidget. Interaction UI will not work."));
			result = EDataValidationResult::Invalid;
		}
	}

	return result;
}

#endif

#if (!UE_BUILD_SHIPPING || WITH_EDITOR)

void UMounteaInteractableComponentBase::DrawDebug()
{
	if (!DebugSettings.DebugMode || !GetWorld())
				return;

	const FVector Origin = GetOwner() ? GetOwner()->GetActorLocation() : GetComponentLocation();
	DrawDebugSphere(GetWorld(), Origin, WidgetRanges.AttentionRange, 16, FColor::Yellow, false, -1.f, 0, 1.f);
	DrawDebugSphere(GetWorld(), Origin, WidgetRanges.InteractionRange, 16, FColor::Green, false, -1.f, 0, 1.f);
}

#endif

#pragma endregion

#undef LOCTEXT_NAMESPACE
