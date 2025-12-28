// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Components/Interactable/MounteaInteractableComponentBase.h"

#include "Helpers/MounteaInteractionSystemLog.h"

#if WITH_EDITOR

#include "EditorHelper.h"
#include "Misc/DataValidation.h"

#endif

#include "CommonInputSubsystem.h"
#include "TimerManager.h"

#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/InputDeviceSubsystem.h"

#include "Helpers/MounteaInteractionFunctionLibrary.h"
#include "Helpers/MounteaInteractionSystemBFL.h"

#include "Interfaces/MounteaInteractionWidget.h"
#include "Interfaces/MounteaInteractorInterface.h"


#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "MounteaInteractableComponentBase"

UMounteaInteractableComponentBase::UMounteaInteractableComponentBase() :
		DebugSettings(false),
		InteractionPeriod(1.5f),
		DefaultInteractableState(EInteractableStateV2::EIS_Awake),
		SetupType(ESetupType::EST_Quick),
		CooldownPeriod(3.0f),
		LifecycleMode(EInteractableLifecycle::EIL_Cycled),
		LifecycleCount(-1),
		InteractionWeight(1),
		HighlightType(EHighlightType::EHT_OverlayMaterial),
		bInteractionHighlight(true),
		StencilID(133),
		bCanPersist(false),
		InteractableName(LOCTEXT("MounteaInteractableComponentBase", "Base")),
		ComparisonMethod(ETimingComparison::ECM_None),
		TimeToStart(0.001f),
		InteractableState(EInteractableStateV2::EIS_Awake),
		RemainingLifecycleCount(LifecycleCount),
		CachedInteractionWeight(InteractionWeight),
		bInteractableInitialized(false)
{
	bAutoActivate = true;
	
	SetIsReplicatedByDefault(true);
	SetActiveFlag(true);

	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	DebugSettings.DebugMode = false;
	DebugSettings.EditorDebugMode = false;

	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(64, 64);
	bDrawAtDesiredSize = false;
	
	UActorComponent::SetActive(true);
	SetHiddenInGame(true);

#if WITH_EDITORONLY_DATA
	bVisualizeComponent = true;
#endif
}

void UMounteaInteractableComponentBase::BeginPlay()
{
	Super::BeginPlay();

	// Interaction Events
	{
		OnInteractableSelected.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableSelectedEvent);
		OnInteractorFound.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractorFound);
		OnInteractorLost.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractorLost);

		OnInteractorOverlapped.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableBeginOverlapEvent);
		OnInteractorStopOverlap.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableStopOverlapEvent);
		OnInteractorTraced.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableTraced);

		OnInteractionCompleted.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractionCompleted);
		OnInteractionCycleCompleted.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractionCycleCompleted);
		OnInteractionStarted.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractionStarted);
		OnInteractionStopped.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractionStopped);
		OnInteractionCanceled.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractionCanceled);
		OnLifecycleCompleted.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractionLifecycleCompleted);
		OnCooldownCompleted.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractionCooldownCompleted);
	}
	
	// Attributes Events
	{
		OnInteractableDependencyChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableDependencyChangedEvent);
		OnInteractableAutoSetupChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableAutoSetupChangedEvent);
		OnInteractableWeightChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableWeightChangedEvent);
		OnInteractableStateChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableStateChangedEvent);
		OnInteractableOwnerChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableOwnerChangedEvent);
		OnInteractableCollisionChannelChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableCollisionChannelChangedEvent);
		OnLifecycleModeChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnLifecycleModeChangedEvent);
		OnLifecycleCountChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnLifecycleCountChangedEvent);
		OnCooldownPeriodChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnCooldownPeriodChangedEvent);
		OnInteractorChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractorChangedEvent);
	}

	// Ignored Classes Events
	{
		OnIgnoredInteractorClassAdded.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnIgnoredClassAdded);
		OnIgnoredInteractorClassRemoved.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnIgnoredClassRemoved);
	}

	// Highlight Events
	{
		OnHighlightableComponentAdded.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnHighlightableComponentAddedEvent);
		OnHighlightableComponentRemoved.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnHighlightableComponentRemovedEvent);
	}
	
	// Collision Events
	{
		OnCollisionComponentAdded.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnCollisionComponentAddedEvent);
		OnCollisionComponentRemoved.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnCollisionComponentRemovedEvent);
	}

	// Widget
	{
		OnWidgetUpdated.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnWidgetUpdatedEvent);
	}
	
	// Highlight
	{
		OnHighlightTypeChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnHighlightTypeChangedEvent);
		OnHighlightMaterialChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnHighlightMaterialChangedEvent);
	}

	// Dependency
	{
		InteractableDependencyStarted.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractableDependencyStartedCallback);
		InteractableDependencyStopped.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractableDependencyStoppedCallback);
	}

	// Activation
	{
		OnComponentActivated.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractableComponentActivated);
	}

	// Bind Changing Input Devices
	{
		if (UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(GetWorld()))
		{
			if (const auto localPlayer = UMounteaInteractionSystemBFL::FindLocalPlayer(GetOwner()))
			{
				if (UCommonInputSubsystem* commonInputSubsystem = UCommonInputSubsystem::Get(localPlayer))
				{
					commonInputSubsystem->OnInputMethodChangedNative.AddUObject(this, &UMounteaInteractableComponentBase::OnInputModeChanged);
				}
			}
		}
		
		OnInteractionDeviceChanged.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInputDeviceChanged);
	}
	
	RemainingLifecycleCount = LifecycleCount;
	
	Execute_SetState(this, DefaultInteractableState);

	if (bAutoActivate)
	{
		AutoSetup();
	}

#if WITH_EDITOR
	
	DrawDebug();

#endif
}

void UMounteaInteractableComponentBase::InitWidget()
{
	Super::InitWidget();

	UpdateInteractionWidget();
}

void UMounteaInteractableComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);	
}

void UMounteaInteractableComponentBase::OnComponentCreated()
{
	Super::OnComponentCreated();
}

void UMounteaInteractableComponentBase::OnRegister()
{
	
#if WITH_EDITOR
	
	if (bVisualizeComponent && SpriteComponent == nullptr && GetOwner() && !GetWorld()->IsGameWorld() )
	{
		SpriteComponent = NewObject<UBillboardComponent>(GetOwner(), NAME_None, RF_Transactional | RF_Transient | RF_TextExportTransient);

		SpriteComponent->Sprite = LoadObject<UTexture2D>(nullptr, TEXT("/MounteaInteractionSystem/Textures/Editor/T_MounteaLogo"));
		SpriteComponent->SetRelativeScale3D_Direct(FVector(1.f));
		SpriteComponent->Mobility = EComponentMobility::Movable;
		SpriteComponent->AlwaysLoadOnClient = false;
		SpriteComponent->SetIsVisualizationComponent(true);
		SpriteComponent->SpriteInfo.Category = TEXT("Misc");
		SpriteComponent->SpriteInfo.DisplayName = NSLOCTEXT( "SpriteCategory", "Misc", "Misc" );
		SpriteComponent->CreationMethod = CreationMethod;
		SpriteComponent->bIsScreenSizeScaled = true;
		SpriteComponent->bUseInEditorScaling = true;

		SpriteComponent->SetupAttachment(this);
		SpriteComponent->RegisterComponent();
	}

#endif
	
	Super::OnRegister();
}

#pragma region InteractionImplementations

bool UMounteaInteractableComponentBase::DoesHaveInteractor_Implementation() const
{
	return Interactor.GetObject() != nullptr;
}

bool UMounteaInteractableComponentBase::DoesAutoSetup_Implementation() const
{ return SetupType != ESetupType::EST_None; }

void UMounteaInteractableComponentBase::ToggleAutoSetup_Implementation(const ESetupType& NewValue)
{
	SetupType = NewValue;
}

bool UMounteaInteractableComponentBase::ActivateInteractable_Implementation(FString& ErrorMessage)
{
	const EInteractableStateV2 CachedState = InteractableState;

	Execute_SetState(this, EInteractableStateV2::EIS_Active);

	switch (CachedState)
	{
		case EInteractableStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactable Component is already Active"));
			break;
		case EInteractableStateV2::EIS_Awake:
			ErrorMessage.Append(TEXT("Interactable Component has been Activated"));
			return true;
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::EIS_Cooldown:
		case EInteractableStateV2::EIS_Completed:
		case EInteractableStateV2::EIS_Disabled:
			ErrorMessage.Append(TEXT("Interactable Component cannot be Activated"));
			break;
		case EInteractableStateV2::Default: 
		default:
			ErrorMessage.Append(TEXT("Interactable Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UMounteaInteractableComponentBase::WakeUpInteractable_Implementation(FString& ErrorMessage)
{
	const EInteractableStateV2 CachedState = InteractableState;

	Execute_SetState(this, EInteractableStateV2::EIS_Awake);

	switch (CachedState)
	{
		case EInteractableStateV2::EIS_Awake:
			ErrorMessage.Append(TEXT("Interactable Component is already Awake"));
			break;
		case EInteractableStateV2::EIS_Active:
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::EIS_Cooldown:
		case EInteractableStateV2::EIS_Disabled:
			ErrorMessage.Append(TEXT("Interactable Component has been Awaken"));
			return true;
		case EInteractableStateV2::EIS_Completed:
			ErrorMessage.Append(TEXT("Interactable Component cannot be Awaken"));
			break;
		case EInteractableStateV2::Default: 
		default:
			ErrorMessage.Append(TEXT("Interactable Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UMounteaInteractableComponentBase::CompleteInteractable_Implementation(FString& ErrorMessage)
{
	const EInteractableStateV2 CachedState = InteractableState;

	Execute_SetState(this, EInteractableStateV2::EIS_Completed);

	switch (CachedState)
	{
		case EInteractableStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactable Component is Completed"));
			return true;
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::EIS_Awake:
		case EInteractableStateV2::EIS_Disabled:
		case EInteractableStateV2::EIS_Cooldown:
			ErrorMessage.Append(TEXT("Interactable Component cannot be Completed"));
			break;
		case EInteractableStateV2::EIS_Completed:
			ErrorMessage.Append(TEXT("Interactable Component is already Completed"));
			break;
		case EInteractableStateV2::Default: 
		default:
			ErrorMessage.Append(TEXT("Interactable Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

void UMounteaInteractableComponentBase::DeactivateInteractable_Implementation()
{
	Execute_SetState(this, EInteractableStateV2::EIS_Disabled);
}

void UMounteaInteractableComponentBase::PauseInteraction_Implementation(const float ExpirationTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (!GetWorld()) return;
	
	Execute_SetState(this, EInteractableStateV2::EIS_Paused);
	const bool bIsUnlimited = FMath::IsWithinInclusive(InteractionProgressExpiration, -1.f, 0.f) || FMath::IsNearlyZero(InteractionProgressExpiration, 0.001f);

	const float expirationTime = GetWorld()->GetTimeSeconds();
	if (bCanPersist)
	{
		GetWorld()->GetTimerManager().PauseTimer(Timer_Interaction);
		
		FTimerDelegate TimerDelegate_ProgressExpiration;
		
		TimerDelegate_ProgressExpiration.BindUFunction(this, "OnInteractionProgressExpired", expirationTime, CausingInteractor);

		const float ClampedExpiration = FMath::Max(InteractionProgressExpiration, 0.01f);
		
		GetWorld()->GetTimerManager().SetTimer(Timer_ProgressExpiration, TimerDelegate_ProgressExpiration, ClampedExpiration, false);
		GetWorld()->GetTimerManager().PauseTimer(Timer_Interaction);
	}
	else
	{
		OnInteractionProgressExpired(expirationTime, CausingInteractor);
	}
}

bool UMounteaInteractableComponentBase::CanInteract_Implementation() const
{
	if (!GetWorld()) return false;
	
	switch (InteractableState)
	{
		case EInteractableStateV2::EIS_Awake:
		case EInteractableStateV2::EIS_Active:
		case EInteractableStateV2::EIS_Paused:
			return Execute_GetInteractor(this).GetInterface() != nullptr;
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Disabled:
		case EInteractableStateV2::EIS_Cooldown:
		case EInteractableStateV2::EIS_Completed:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::Default: 
		default: break;
	}
	
	return false;
}

bool UMounteaInteractableComponentBase::CanBeTriggered_Implementation() const
{
	if (!GetWorld()) return false;
	
	switch (InteractableState)
	{
		case EInteractableStateV2::EIS_Awake:
		case EInteractableStateV2::EIS_Active:
		case EInteractableStateV2::EIS_Paused:
			return Interactor.GetObject() == nullptr;
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Disabled:
		case EInteractableStateV2::EIS_Cooldown:
		case EInteractableStateV2::EIS_Completed:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::Default: 
		default: break;
	}
	
	return false;
}

bool UMounteaInteractableComponentBase::IsInteracting_Implementation() const
{
	if (GetWorld())
	{
		return GetWorld()->GetTimerManager().IsTimerActive(Timer_Interaction);
	}

	LOG_ERROR(TEXT("[IsInteracting] Cannot find World!"))
	return false;
}

EInteractableStateV2 UMounteaInteractableComponentBase::GetDefaultState_Implementation() const
{ return DefaultInteractableState; }

void UMounteaInteractableComponentBase::SetDefaultState_Implementation(const EInteractableStateV2 NewState)
{
	if
	(
		DefaultInteractableState == EInteractableStateV2::EIS_Active ||
		DefaultInteractableState == EInteractableStateV2::EIS_Completed ||
		DefaultInteractableState == EInteractableStateV2::EIS_Cooldown
	)
	{
		LOG_ERROR(TEXT("[SetDefaultState] Tried to set invalid Default State!"))
		return;
	}
	DefaultInteractableState = NewState;
}

EInteractableStateV2 UMounteaInteractableComponentBase::GetState_Implementation() const
{ return InteractableState; }

void UMounteaInteractableComponentBase::CleanupComponent()
{
	Execute_StopHighlight(this);
	OnInteractableStateChanged.Broadcast(InteractableState);
	if (GetWorld()) GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	OnInteractorLost.Broadcast(Interactor);

	Execute_RemoveHighlightableComponents(this, HighlightableComponents);
	Execute_RemoveCollisionComponents(this, CollisionComponents);
}

void UMounteaInteractableComponentBase::SetState_Implementation(const EInteractableStateV2 NewState)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetState] No owner!"))
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		switch (NewState)
		{
			case EInteractableStateV2::EIS_Active:
				switch (InteractableState)
				{
					case EInteractableStateV2::EIS_Paused:
					case EInteractableStateV2::EIS_Awake:
						InteractableState = NewState;
						OnInteractableStateChanged.Broadcast(InteractableState);
						break;
					case EInteractableStateV2::EIS_Active:
						break;
					case EInteractableStateV2::EIS_Asleep:
					case EInteractableStateV2::EIS_Suppressed:
					case EInteractableStateV2::EIS_Cooldown:
					case EInteractableStateV2::EIS_Completed:
					case EInteractableStateV2::EIS_Disabled:
					case EInteractableStateV2::Default:
					default: break;
				}
				break;
			case EInteractableStateV2::EIS_Awake:
				switch (InteractableState)
				{
					case EInteractableStateV2::EIS_Active:
					case EInteractableStateV2::EIS_Asleep:
					case EInteractableStateV2::EIS_Suppressed:
					case EInteractableStateV2::EIS_Cooldown:
					case EInteractableStateV2::EIS_Disabled:
					case EInteractableStateV2::EIS_Paused:
						{
							InteractableState = NewState;
							OnInteractableStateChanged.Broadcast(InteractableState);
							for (const auto& Itr : CollisionComponents)
							{
								Execute_BindCollisionShape(this, Itr);
							}
							break;
						}
					case EInteractableStateV2::EIS_Completed:
					case EInteractableStateV2::EIS_Awake:
					case EInteractableStateV2::Default: 
					default: break;
				}
				break;
			case EInteractableStateV2::EIS_Asleep:
				switch (InteractableState)
				{
					case EInteractableStateV2::EIS_Active:
					case EInteractableStateV2::EIS_Paused:
					case EInteractableStateV2::EIS_Awake:
					case EInteractableStateV2::EIS_Suppressed:
					case EInteractableStateV2::EIS_Cooldown:
					case EInteractableStateV2::EIS_Disabled:
						{
							InteractableState = NewState;
							Execute_StopHighlight(this);
							OnInteractableStateChanged.Broadcast(InteractableState);
							if (GetWorld()) GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
							OnInteractorLost.Broadcast(Interactor);
									
							for (const auto& Itr : CollisionComponents)
							{
								Execute_UnbindCollisionShape(this, Itr);
							}
							break;
						}
					case EInteractableStateV2::EIS_Completed:
					case EInteractableStateV2::EIS_Asleep:
					case EInteractableStateV2::Default: 
					default: break;
				}
				break;
			case EInteractableStateV2::EIS_Cooldown:
				switch (InteractableState)
				{
					case EInteractableStateV2::EIS_Awake:
					case EInteractableStateV2::EIS_Active:
						InteractableState = NewState;
						Execute_StopHighlight(this);
						OnInteractableStateChanged.Broadcast(InteractableState);
						break;
					case EInteractableStateV2::EIS_Suppressed:
					case EInteractableStateV2::EIS_Disabled:
						{
							InteractableState = NewState;
							Execute_StopHighlight(this);
							OnInteractableStateChanged.Broadcast(InteractableState);
							if (GetWorld()) GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
							OnInteractorLost.Broadcast(Interactor);
									
							for (const auto& Itr : CollisionComponents)
							{
								Execute_UnbindCollisionShape(this, Itr);
							}
							break;
						}
					case EInteractableStateV2::EIS_Paused:
					case EInteractableStateV2::EIS_Cooldown:
					case EInteractableStateV2::EIS_Completed:
					case EInteractableStateV2::EIS_Asleep:
					case EInteractableStateV2::Default: 
					default: break;
				}
				break;
			case EInteractableStateV2::EIS_Completed:
				switch (InteractableState)
				{
					case EInteractableStateV2::EIS_Active:
						{
							InteractableState = NewState;
							CleanupComponent();
							break;
						}
					case EInteractableStateV2::EIS_Completed:
					case EInteractableStateV2::EIS_Paused:
					case EInteractableStateV2::EIS_Suppressed:
					case EInteractableStateV2::EIS_Awake:
					case EInteractableStateV2::EIS_Cooldown:
					case EInteractableStateV2::EIS_Disabled:
					case EInteractableStateV2::EIS_Asleep:
					case EInteractableStateV2::Default: 
					default: break;
				}
				break;
			case EInteractableStateV2::EIS_Disabled:
				switch (InteractableState)
				{
					case EInteractableStateV2::EIS_Active:
					case EInteractableStateV2::EIS_Paused:
					case EInteractableStateV2::EIS_Completed:
					case EInteractableStateV2::EIS_Awake:
					case EInteractableStateV2::EIS_Suppressed:
					case EInteractableStateV2::EIS_Cooldown:
					case EInteractableStateV2::EIS_Asleep:
						{
							InteractableState = NewState;

							// Replacing Cleanup
							Execute_StopHighlight(this);
							OnInteractableStateChanged.Broadcast(InteractableState);
							if (GetWorld()) GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
							OnInteractorLost.Broadcast(Interactor);
									
							for (const auto& Itr : CollisionComponents)
							{
								Execute_UnbindCollisionShape(this, Itr);
							}

							break;
						}
					case EInteractableStateV2::EIS_Disabled:
					case EInteractableStateV2::Default: 
					default: break;
				}
				break;
			case EInteractableStateV2::EIS_Suppressed:
				switch (InteractableState)
				{
					case EInteractableStateV2::EIS_Active:
					case EInteractableStateV2::EIS_Awake:
					case EInteractableStateV2::EIS_Asleep:
					case EInteractableStateV2::EIS_Disabled:
					case EInteractableStateV2::EIS_Paused:
						{
							OnInteractionCanceled.Broadcast();
							InteractableState = NewState;
							Execute_StopHighlight(this);
							OnInteractableStateChanged.Broadcast(InteractableState);
							break;
						}
					case EInteractableStateV2::EIS_Cooldown:
						{
							OnInteractionCanceled.Broadcast();
							InteractableState = NewState;
							Execute_StopHighlight(this);
							OnInteractableStateChanged.Broadcast(InteractableState);
							GetWorld()->GetTimerManager().ClearTimer(Timer_Cooldown);
							break;
						}
					case EInteractableStateV2::EIS_Completed:
					case EInteractableStateV2::EIS_Suppressed:
					case EInteractableStateV2::Default:
					default: break;
				}
				break;
			case EInteractableStateV2::EIS_Paused:
				switch (InteractableState)
				{
					case EInteractableStateV2::EIS_Active:
						{
							InteractableState = NewState;
							OnInteractableStateChanged.Broadcast(InteractableState);
							break;
						}
					case EInteractableStateV2::EIS_Paused:
					case EInteractableStateV2::EIS_Awake:
					case EInteractableStateV2::EIS_Asleep:
					case EInteractableStateV2::EIS_Disabled:
					case EInteractableStateV2::EIS_Cooldown:
					case EInteractableStateV2::EIS_Completed:
					case EInteractableStateV2::EIS_Suppressed:
					case EInteractableStateV2::Default:
					default: break;
				}
				break;
			case EInteractableStateV2::Default: 
			default:
				Execute_StopHighlight(this);
				break;
		}
	
		Execute_ProcessDependencies(this);
	}
	else
	{
		SetState_Server(NewState);
	}
}

void UMounteaInteractableComponentBase::StartHighlight_Implementation()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(GetWorld()))
		{
			ProcessStartHighlight();
		}
		else
		{
			StartHighlight_Client();
		}
	}
	else
	{
		ProcessStartHighlight();
	}
}

void UMounteaInteractableComponentBase::StopHighlight_Implementation()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(GetWorld()))
		{
			ProcessStopHighlight();
		}
		else
		{
			StopHighlight_Client();
		}
	}
	else
	{
		ProcessStopHighlight();
	}
}

TArray<TSoftClassPtr<UObject>> UMounteaInteractableComponentBase::GetIgnoredClasses_Implementation() const
{ return IgnoredClasses; }

void UMounteaInteractableComponentBase::SetIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& NewIgnoredClasses)
{
	IgnoredClasses.Empty();

	IgnoredClasses = NewIgnoredClasses;
}

void UMounteaInteractableComponentBase::AddIgnoredClass_Implementation(const TSoftClassPtr<UObject>& AddIgnoredClass)
{
	if (AddIgnoredClass == nullptr) return;

	if (IgnoredClasses.Contains(AddIgnoredClass)) return;

	IgnoredClasses.Add(AddIgnoredClass);

	OnIgnoredInteractorClassAdded.Broadcast(AddIgnoredClass);
}

void UMounteaInteractableComponentBase::AddIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& AddIgnoredClasses)
{
	for (const auto& Itr : AddIgnoredClasses)
	{
		Execute_AddIgnoredClass(this, Itr);
	}
}

void UMounteaInteractableComponentBase::RemoveIgnoredClass_Implementation(const TSoftClassPtr<UObject>& RemoveIgnoredClass)
{
	if (RemoveIgnoredClass == nullptr) return;

	if (!IgnoredClasses.Contains(RemoveIgnoredClass)) return;

	IgnoredClasses.Remove(RemoveIgnoredClass);

	OnIgnoredInteractorClassRemoved.Broadcast(RemoveIgnoredClass);
}

void UMounteaInteractableComponentBase::RemoveIgnoredClasses_Implementation(const TArray<TSoftClassPtr<UObject>>& RemoveIgnoredClasses)
{
	for (const auto& Itr : RemoveIgnoredClasses)
	{
		Execute_RemoveIgnoredClass(this, Itr);
	}
}

void UMounteaInteractableComponentBase::AddInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency)
{
	if (InteractionDependency.GetObject() == nullptr) return;
	if (InteractionDependencies.Contains(InteractionDependency)) return;

	OnInteractableDependencyChanged.Broadcast(InteractionDependency);
	
	InteractionDependencies.Add(InteractionDependency);

	InteractionDependency->GetInteractableDependencyStarted().Broadcast(this);
}

void UMounteaInteractableComponentBase::RemoveInteractionDependency_Implementation(const TScriptInterface<IMounteaInteractableInterface>& InteractionDependency)
{
	if (InteractionDependency.GetObject() == nullptr) return;
	if (!InteractionDependencies.Contains(InteractionDependency)) return;

	OnInteractableDependencyChanged.Broadcast(InteractionDependency);

	InteractionDependencies.Remove(InteractionDependency);

	InteractionDependency->GetInteractableDependencyStopped().Broadcast(this);
}

TArray<TScriptInterface<IMounteaInteractableInterface>> UMounteaInteractableComponentBase::GetInteractionDependencies_Implementation() const
{ return InteractionDependencies; }

void UMounteaInteractableComponentBase::ProcessDependencies_Implementation()
{
	if (InteractionDependencies.Num() == 0) return;

	auto Dependencies = InteractionDependencies;
	for (const auto& Itr : Dependencies)
	{
		switch (InteractableState)
		{
			case EInteractableStateV2::EIS_Active:
			case EInteractableStateV2::EIS_Suppressed:
				Itr->GetInteractableDependencyStarted().Broadcast(this);
				switch (Itr->Execute_GetState(Itr.GetObject()))
				{
					case EInteractableStateV2::EIS_Active:
					case EInteractableStateV2::EIS_Awake:
					case EInteractableStateV2::EIS_Asleep:
						Itr->Execute_SetState(this, EInteractableStateV2::EIS_Suppressed);
						break;
					case EInteractableStateV2::EIS_Cooldown:
						
						Itr->Execute_SetState(this, EInteractableStateV2::EIS_Suppressed);
						
						break;
					case EInteractableStateV2::EIS_Completed: break;
					case EInteractableStateV2::EIS_Disabled: break;
					case EInteractableStateV2::EIS_Suppressed: break;
					case EInteractableStateV2::Default: break;
					default: break;
				}
				break;
			case EInteractableStateV2::EIS_Cooldown:
			case EInteractableStateV2::EIS_Awake:
			case EInteractableStateV2::EIS_Asleep:
				Itr->GetInteractableDependencyStarted().Broadcast(this);
				switch (Itr->Execute_GetState(Itr.GetObject()))
				{
					
					case EInteractableStateV2::EIS_Awake:
					case EInteractableStateV2::EIS_Asleep:
					case EInteractableStateV2::EIS_Suppressed: 
						Itr->Execute_SetState(this, Itr->Execute_GetDefaultState(Itr.GetObject()));
						break;
					case EInteractableStateV2::EIS_Cooldown: break;
					case EInteractableStateV2::EIS_Completed: break;
					case EInteractableStateV2::EIS_Disabled: break;
					case EInteractableStateV2::EIS_Active:
					case EInteractableStateV2::Default: break;
					default: break;
				}
				break;
			case EInteractableStateV2::EIS_Disabled:
			case EInteractableStateV2::EIS_Completed:
				Itr->GetInteractableDependencyStopped().Broadcast(this);
				Itr->Execute_SetState(this, Itr->Execute_GetDefaultState(Itr.GetObject()));
				Execute_RemoveInteractionDependency(this, Itr);
				break;
			case EInteractableStateV2::Default:
			default:
				break;
		}
	}
}

TScriptInterface<IMounteaInteractorInterface> UMounteaInteractableComponentBase::GetInteractor_Implementation() const
{ return Interactor; }

void UMounteaInteractableComponentBase::SetInteractor_Implementation(const TScriptInterface<IMounteaInteractorInterface>& NewInteractor)
{
	const TScriptInterface<IMounteaInteractorInterface> OldInteractor = Interactor;

	Interactor = NewInteractor;
	
	if (NewInteractor.GetInterface() != nullptr)
	{
		//NewInteractor->GetOnInteractableSelectedHandle().AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractableSelected);
		//NewInteractor->GetOnInteractableFoundHandle().Broadcast(this);

		NewInteractor->GetInputActionConsumedHandle().AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractorActionConsumed);

		if (GetOwner() && GetOwner()->HasAuthority())
		{
			GetOwner()->SetOwner(Interactor->Execute_GetOwningActor(Interactor.GetObject()));
		}
	}
	else
	{
		if (OldInteractor.GetInterface() != nullptr)
		{
			//OldInteractor->GetOnInteractableSelectedHandle().RemoveDynamic(this, &UMounteaInteractableComponentBase::InteractableSelected);
			OldInteractor->GetInputActionConsumedHandle().RemoveDynamic(this, &UMounteaInteractableComponentBase::InteractorActionConsumed);
		}

		if (GetOwner() && GetOwner()->HasAuthority())
		{
			GetOwner()->SetOwner(nullptr);
		}

		Execute_StopHighlight(this);
	}

	//Interactor = NewInteractor;
	OnInteractorChanged.Broadcast(Interactor);
}

float UMounteaInteractableComponentBase::GetInteractionProgress_Implementation() const
{
	if (!GetWorld()) return -1;

	if (Timer_Interaction.IsValid())
	{
		return GetWorld()->GetTimerManager().GetTimerElapsed(Timer_Interaction) / InteractionPeriod;
	}
	return 0.f;
}

float UMounteaInteractableComponentBase::GetInteractionPeriod_Implementation() const
{ return InteractionPeriod; }

void UMounteaInteractableComponentBase::SetInteractionPeriod_Implementation(const float NewPeriod)
{
	float TempPeriod = NewPeriod;
	if (TempPeriod > -1.f && TempPeriod < 0.01f)
	{
		TempPeriod = 0.01f;
	}
	if (FMath::IsNearlyZero(TempPeriod, 0.0001f))
	{
		TempPeriod = 0.01f;
	}

	InteractionPeriod = FMath::Max(-1.f, TempPeriod);
}

int32 UMounteaInteractableComponentBase::GetInteractableWeight_Implementation() const
{ return InteractionWeight; }

void UMounteaInteractableComponentBase::SetInteractableWeight_Implementation(const int32 NewWeight)
{
	InteractionWeight = NewWeight;

	OnInteractableWeightChanged.Broadcast(InteractionWeight);
}

AActor* UMounteaInteractableComponentBase::GetInteractableOwner_Implementation() const
{ return GetOwner(); }

ECollisionChannel UMounteaInteractableComponentBase::GetCollisionChannel_Implementation() const
{ return CollisionChannel; }

void UMounteaInteractableComponentBase::SetCollisionChannel_Implementation(const TEnumAsByte<ECollisionChannel>& NewChannel)
{
	CollisionChannel = NewChannel;

	OnInteractableCollisionChannelChanged.Broadcast(CollisionChannel);
}

TArray<UPrimitiveComponent*> UMounteaInteractableComponentBase::GetCollisionComponents_Implementation() const
{	return CollisionComponents;}

EInteractableLifecycle UMounteaInteractableComponentBase::GetLifecycleMode_Implementation() const
{	return LifecycleMode;}

void UMounteaInteractableComponentBase::SetLifecycleMode_Implementation(const EInteractableLifecycle& NewMode)
{
	LifecycleMode = NewMode;

	OnLifecycleModeChanged.Broadcast(LifecycleMode);
}

int32 UMounteaInteractableComponentBase::GetLifecycleCount_Implementation() const
{	return LifecycleCount;}

void UMounteaInteractableComponentBase::SetLifecycleCount_Implementation(const int32 NewLifecycleCount)
{
	switch (LifecycleMode)
	{
		case EInteractableLifecycle::EIL_Cycled:
			if (NewLifecycleCount <= -1)
			{
				LifecycleCount = -1;
				OnLifecycleCountChanged.Broadcast(LifecycleCount);
			}
			else if (NewLifecycleCount < 2)
			{
				LifecycleCount = 2;
				OnLifecycleCountChanged.Broadcast(LifecycleCount);
			}
			else if (NewLifecycleCount > 2)
			{
				LifecycleCount = NewLifecycleCount;
				OnLifecycleCountChanged.Broadcast(LifecycleCount);
			}
			break;
		case EInteractableLifecycle::EIL_OnlyOnce:
		case EInteractableLifecycle::Default:
		default: break;
	}
}

int32 UMounteaInteractableComponentBase::GetRemainingLifecycleCount_Implementation() const
{ return RemainingLifecycleCount; }

float UMounteaInteractableComponentBase::GetCooldownPeriod_Implementation() const
{ return CooldownPeriod; }

void UMounteaInteractableComponentBase::SetCooldownPeriod_Implementation(const float NewCooldownPeriod)
{
	switch (LifecycleMode)
	{
		case EInteractableLifecycle::EIL_Cycled:
			CooldownPeriod = FMath::Max(0.1f, NewCooldownPeriod);
			OnCooldownPeriodChanged.Broadcast(CooldownPeriod);
			break;
		case EInteractableLifecycle::EIL_OnlyOnce:
		case EInteractableLifecycle::Default:
		default: break;
	}
}

void UMounteaInteractableComponentBase::AddCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp)
{
	if (CollisionComp == nullptr) return;
	if (CollisionComponents.Contains(CollisionComp)) return;
	
	CollisionComponents.Add(CollisionComp);
	
	Execute_BindCollisionShape(this, CollisionComp);
	
	OnCollisionComponentAdded.Broadcast(CollisionComp);
}

void UMounteaInteractableComponentBase::AddCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& NewCollisionComponents)
{
	for (UPrimitiveComponent* const Itr : NewCollisionComponents)
	{
		Execute_AddCollisionComponent(this, Itr);
	}
}

void UMounteaInteractableComponentBase::RemoveCollisionComponent_Implementation(UPrimitiveComponent* CollisionComp)
{
	if (CollisionComp == nullptr) return;
	if (!CollisionComponents.Contains(CollisionComp)) return;
	
	CollisionComponents.Remove(CollisionComp);

	Execute_UnbindCollisionShape(this, CollisionComp);
	
	OnCollisionComponentRemoved.Broadcast(CollisionComp);
}

void UMounteaInteractableComponentBase::RemoveCollisionComponents_Implementation(const TArray<UPrimitiveComponent*>& RemoveCollisionComponents)
{
	for (UPrimitiveComponent* const Itr : RemoveCollisionComponents)
	{
		Execute_RemoveCollisionComponent(this, Itr);
	}
}

TArray<UMeshComponent*> UMounteaInteractableComponentBase::GetHighlightableComponents_Implementation() const
{	return HighlightableComponents;}

void UMounteaInteractableComponentBase::AddHighlightableComponent_Implementation(UMeshComponent* MeshComponent)
{
	if (MeshComponent == nullptr) return;
	if (HighlightableComponents.Contains(MeshComponent)) return;

	HighlightableComponents.Add(MeshComponent);

	Execute_BindHighlightableMesh(this, MeshComponent);

	OnHighlightableComponentAdded.Broadcast(MeshComponent);
}

void UMounteaInteractableComponentBase::AddHighlightableComponents_Implementation(const TArray<UMeshComponent*>& AddMeshComponents)
{
	for (UMeshComponent* const Itr : HighlightableComponents)
	{
		Execute_AddHighlightableComponent(this, Itr);
	}
}

void UMounteaInteractableComponentBase::RemoveHighlightableComponent_Implementation(UMeshComponent* MeshComponent)
{
	if (MeshComponent == nullptr) return;
	if (!HighlightableComponents.Contains(MeshComponent)) return;

	HighlightableComponents.Remove(MeshComponent);

	Execute_UnbindHighlightableMesh(this, MeshComponent);

	OnHighlightableComponentRemoved.Broadcast(MeshComponent);
}

void UMounteaInteractableComponentBase::RemoveHighlightableComponents_Implementation(const TArray<UMeshComponent*>& RemoveMeshComponents)
{
	for (UMeshComponent* const Itr : RemoveMeshComponents)
	{
		Execute_RemoveHighlightableComponent(this, Itr);
	}
}

TArray<FName> UMounteaInteractableComponentBase::GetCollisionOverrides_Implementation() const
{	return CollisionOverrides;}

TArray<FName> UMounteaInteractableComponentBase::GetHighlightableOverrides_Implementation() const
{	return HighlightableOverrides;}

FDataTableRowHandle UMounteaInteractableComponentBase::GetInteractableData_Implementation() const
{ return InteractableData; }

void UMounteaInteractableComponentBase::SetInteractableData_Implementation(FDataTableRowHandle NewData)
{ InteractableData = NewData; }

FText UMounteaInteractableComponentBase::GetInteractableName_Implementation() const
{ return InteractableName; }

void UMounteaInteractableComponentBase::SetInteractableName_Implementation(const FText& NewName)
{
	if (NewName.IsEmpty()) return;
	InteractableName = NewName;
}

EHighlightType UMounteaInteractableComponentBase::GetHighlightType_Implementation() const
{ return HighlightType; }

void UMounteaInteractableComponentBase::SetHighlightType_Implementation(const EHighlightType NewHighlightType)
{
	HighlightType = NewHighlightType;

	OnHighlightTypeChanged.Broadcast(NewHighlightType);
}

UMaterialInterface* UMounteaInteractableComponentBase::GetHighlightMaterial_Implementation() const
{ return HighlightMaterial; }

void UMounteaInteractableComponentBase::SetHighlightMaterial_Implementation(UMaterialInterface* NewHighlightMaterial)
{
	HighlightMaterial = NewHighlightMaterial;

	OnHighlightMaterialChanged.Broadcast(NewHighlightMaterial);
}

ETimingComparison UMounteaInteractableComponentBase::GetComparisonMethod_Implementation() const
{ return ComparisonMethod; }

void UMounteaInteractableComponentBase::SetComparisonMethod_Implementation(const ETimingComparison Value)
{ ComparisonMethod = Value; }

void UMounteaInteractableComponentBase::SetDefaults_Implementation()
{
	if (const auto DefaultTable = UMounteaInteractionFunctionLibrary::GetInteractableDefaultDataTable())
	{
		InteractableData.DataTable = DefaultTable;
	}
	
	if (const auto DefaultWidgetClass = UMounteaInteractionFunctionLibrary::GetInteractableDefaultWidgetClass())
	{
		if (DefaultWidgetClass != nullptr)
		{
			SetWidgetClass(DefaultWidgetClass.Get());
		}
	}
	
	auto defaultSettings = UMounteaInteractionFunctionLibrary::GetDefaultInteractableSettings();
	{
		HighlightType		= defaultSettings.DefaultHighlightSetup.HighlightType;
		StencilID				= defaultSettings.DefaultHighlightSetup.StencilID;
		HighlightMaterial	= defaultSettings.DefaultHighlightSetup.HighlightMaterial;

		InteractionPeriod = defaultSettings.DefaultInteractionPeriod;
		InteractableState = defaultSettings.DefaultInteractableState;
		SetupType			= defaultSettings.DefaultSetupType;
		CollisionChannel = defaultSettings.DefaultCollisionChannel;
		CooldownPeriod = defaultSettings.DefaultCooldownPeriod;
		bInteractionHighlight = defaultSettings.DefaultInteractionHighlight;
		InteractionWeight = defaultSettings.DefaultInteractableWeight;
		if (!InteractableCompatibleTags.HasTag(defaultSettings.InteractableMainTag))
			InteractableCompatibleTags.AddTag(defaultSettings.InteractableMainTag);
	}
}

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

bool UMounteaInteractableComponentBase::HasInteractor_Implementation() const
{
	return Interactor.GetObject() != nullptr && Interactor.GetInterface() != nullptr;
}

AActor* UMounteaInteractableComponentBase::GetOwningActor_Implementation() const
{
	return GetOwner();
}

FString UMounteaInteractableComponentBase::ToString_Implementation() const
{
	FText interactableNameText = InteractableName;
	FText interactableStateText = FText::FromString(UEnum::GetValueAsString(InteractableState));
	FText lifecycleModeText = FText::FromString(UEnum::GetValueAsString(LifecycleMode));
	FText interactableCompatibleTagsText = FText::FromString(InteractableCompatibleTags.ToStringSimple());
	FText interactionHighlightText = FText::FromString(bInteractionHighlight ? TEXT("True") : TEXT("False"));
	FText highlightTypeText = FText::FromString(UEnum::GetValueAsString(HighlightType));
	FText canPersistText = FText::FromString(bCanPersist ? TEXT("True") : TEXT("False"));
	FText interactionProgressExpirationText = bCanPersist ? FText::AsNumber(InteractionProgressExpiration) : FText::FromString("N/A");
	FText interactableDataText = FText::FromString(InteractableData.RowName.ToString());
	FText lifecycleCountText = (LifecycleMode == EInteractableLifecycle::EIL_Cycled) ? FText::AsNumber(LifecycleCount) : FText::FromString("N/A");
	FText remainingLifecycleCountText = (LifecycleMode == EInteractableLifecycle::EIL_Cycled) ? FText::AsNumber(RemainingLifecycleCount) : FText::FromString("N/A");
	FText collisionComponentsCountText = FText::AsNumber(CollisionComponents.Num());
	FText highlightableComponentsCountText = FText::AsNumber(HighlightableComponents.Num());
	FText interactionWeightText = FText::AsNumber(InteractionWeight);
	FText interactorText = Interactor.GetObject() ? FText::FromString(Interactor->Execute_GetOwningActor(Interactor.GetObject()) ? Interactor->Execute_GetOwningActor(Interactor.GetObject())->GetName() : TEXT("invalid")) : FText::FromString("None");

	return FText::Format(
		NSLOCTEXT("InteractableDebugData", "Format", 
			"Interactable Name: {0}\n"
			"Interactable State: {1}\n"
			"Lifecycle Mode: {2}\n"
			"Interactable Compatible Tags: {3}\n"
			"Interaction Highlight: {4}\n"
			"Highlight Type: {5}\n"
			"Can Persist: {6}\n"
			"Interaction Progress Expiration: {7}\n"
			"Interactable Data: {8}\n"
			"Lifecycle Count: {9}\n"
			"Remaining Lifecycle Count: {10}\n"
			"Collision Components Count: {11}\n"
			"Highlightable Components Count: {12}\n"
			"Interaction Weight: {13}\n"
			"Interactor: {14}"),
		interactableNameText, interactableStateText, lifecycleModeText, interactableCompatibleTagsText, interactionHighlightText, highlightTypeText,
		canPersistText, interactionProgressExpirationText, interactableDataText, lifecycleCountText, remainingLifecycleCountText,
		collisionComponentsCountText, highlightableComponentsCountText, interactionWeightText, interactorText
	).ToString();
}

void UMounteaInteractableComponentBase::InteractorFound_Implementation(const TScriptInterface<IMounteaInteractorInterface>& FoundInteractor)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (Execute_CanBeTriggered(this))
		{
			Execute_SetInteractor(this, FoundInteractor);
			
			InteractorFound_Client(FoundInteractor);
			ProcessToggleActive_Client(true);
		
			Execute_OnInteractorFoundEvent(this, FoundInteractor);
		}
	}
}

void UMounteaInteractableComponentBase::InteractorFound_Client_Implementation(const TScriptInterface<IMounteaInteractorInterface>& DirtyInteractor)
{
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		OnInteractorFound.Broadcast(DirtyInteractor);
	}
}

void UMounteaInteractableComponentBase::InteractorLost_Implementation(const TScriptInterface<IMounteaInteractorInterface>& LostInteractor)
{
	if (LostInteractor.GetInterface() == nullptr) return;

	if (Interactor != LostInteractor)
		return;	
	
	GetWorld()->GetTimerManager().ClearTimer(Timer_Interaction);
	GetWorld()->GetTimerManager().ClearTimer(Timer_ProgressExpiration);
		
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(GetWorld()))
		{
			ProcessToggleActive(false);
		}
		else
		{
			InteractorLost_Client(LostInteractor);
			ProcessToggleActive_Client(false);
		}
	}

	switch (InteractableState)
	{
		case EInteractableStateV2::EIS_Cooldown:
			break;
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::EIS_Suppressed:
			Execute_SetState(this, DefaultInteractableState);
			break;
		case EInteractableStateV2::EIS_Active:
		case EInteractableStateV2::EIS_Awake:
		case EInteractableStateV2::EIS_Paused:
			Execute_SetState(this, DefaultInteractableState);
			break;
		case EInteractableStateV2::EIS_Completed:
		case EInteractableStateV2::EIS_Disabled:
		case EInteractableStateV2::Default:
		default: break;
	}
		
	if (Interactor.GetInterface() != nullptr)
	{
		Interactor->GetOnInteractableSelectedHandle().RemoveDynamic(this, &UMounteaInteractableComponentBase::InteractableSelected);
		Interactor->GetOnInteractableLostHandle().RemoveDynamic(this, &UMounteaInteractableComponentBase::InteractableLost);
	}
		
	Execute_SetInteractor(this, nullptr);
	Execute_OnInteractorLostEvent(this, LostInteractor);

	OnInteractionCanceled.Broadcast();
}

void UMounteaInteractableComponentBase::InteractorLost_Client_Implementation(const TScriptInterface<IMounteaInteractorInterface>& DirtyInteractor)
{
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		OnInteractorLost.Broadcast(DirtyInteractor);
		OnInteractionCanceled.Broadcast();
	}
}

void UMounteaInteractableComponentBase::InteractionCompleted_Implementation(const float& TimeCompleted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	Execute_ToggleWidgetVisibility(this, false);
	
	if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
	{
		if (Execute_TriggerCooldown(this)) return;
	}
	
	FString ErrorMessage;
	if( Execute_CompleteInteractable(this, ErrorMessage))
	{
		Execute_OnInteractionCompletedEvent(this, TimeCompleted, CausingInteractor);
	}
	else LOG_INFO(TEXT("%s"), *ErrorMessage);
}

void UMounteaInteractableComponentBase::InteractionCycleCompleted_Implementation(const float& CompletedTime, const int32 CyclesRemaining, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	Execute_OnInteractionCycleCompletedEvent(this, CompletedTime, CyclesRemaining, CausingInteractor);
}

void UMounteaInteractableComponentBase::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (Execute_CanInteract(this) && GetOwner() && GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(Timer_ProgressExpiration);
		
		Execute_SetState(this, EInteractableStateV2::EIS_Active);
		Execute_OnInteractionStartedEvent(this, TimeStarted, CausingInteractor);

		if (UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(GetWorld()))
		{
			Interactor->GetInputActionConsumedHandle().AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractorActionConsumed);
		}
		else
		{
			InteractionStarted_Client(TimeStarted, CausingInteractor);
		}
	}
}

void UMounteaInteractableComponentBase::InteractionStarted_Client_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (GetOwner() && !GetOwner()->HasAuthority() && CausingInteractor.GetObject() != nullptr && CausingInteractor.GetInterface() != nullptr)
	{
		// Just to keep everything safe LOCALLY update Interactor
		if (Interactor != CausingInteractor)
			Interactor = CausingInteractor;
		
		CausingInteractor->GetInputActionConsumedHandle().AddUniqueDynamic(this, &UMounteaInteractableComponentBase::InteractorActionConsumed);
		
		OnInteractionStarted.Broadcast(TimeStarted, CausingInteractor);

		if (bCanPersist && GetWorld()->GetTimerManager().IsTimerPaused(Timer_Interaction))
		{
			GetWorld()->GetTimerManager().UnPauseTimer(Timer_Interaction);
		}
		else
		{
			FTimerDelegate Delegate;

			GetWorld()->GetTimerManager().SetTimer
			(
				Timer_Interaction,
				Delegate,
				InteractionPeriod,
				false
			);
		}
	}
}

void UMounteaInteractableComponentBase::InteractionStopped_Implementation(const float& TimeStarted, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (!GetWorld()) return;

	// Only Active interaction can be stopped!
	switch (InteractableState)
	{
		case EInteractableStateV2::EIS_Active:
			break;
		case EInteractableStateV2::EIS_Awake:
		case EInteractableStateV2::EIS_Cooldown:
		case EInteractableStateV2::EIS_Paused:
		case EInteractableStateV2::EIS_Completed:
		case EInteractableStateV2::EIS_Disabled:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::Default:
			return;
	}
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InteractionStopped_Client(TimeStarted, CausingInteractor);
	}

	if (bCanPersist)
	{
		Execute_PauseInteraction(this, InteractionProgressExpiration, CausingInteractor);
	}
	else
	{
		Execute_InteractionCanceled(this);
	}
}

void UMounteaInteractableComponentBase::InteractionStopped_Client_Implementation(const float& TimeStopped, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (GetOwner() && !GetOwner()->HasAuthority())
	{		
		if (bCanPersist)
			GetWorld()->GetTimerManager().PauseTimer(Timer_Interaction);
		else
			GetOwner()->GetWorldTimerManager().ClearTimer(Timer_Interaction);
		
		OnInteractionStopped.Broadcast(TimeStopped, CausingInteractor);
	}
}

void UMounteaInteractableComponentBase::InteractionCanceled_Implementation()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (Execute_CanInteract(this))
		{		
			GetWorld()->GetTimerManager().ClearTimer(Timer_Interaction);
			GetWorld()->GetTimerManager().ClearTimer(Timer_ProgressExpiration);
			
			if (!UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(GetWorld()))
			{
				InteractionCancelled_Client(GetWorld()->GetTimeSeconds(), Interactor);	
			}
		
			switch (InteractableState)
			{
				case EInteractableStateV2::EIS_Cooldown:
				case EInteractableStateV2::EIS_Awake:
					if (Execute_GetInteractor(this).GetObject() == nullptr)
					{
						Execute_SetState(this, DefaultInteractableState);
					}
					break;
				case EInteractableStateV2::EIS_Active:
				case EInteractableStateV2::EIS_Asleep:
				case EInteractableStateV2::EIS_Completed:
				case EInteractableStateV2::EIS_Disabled:
				case EInteractableStateV2::EIS_Suppressed:
					Execute_SetState(this, DefaultInteractableState);
					break;
				case EInteractableStateV2::Default:
				default: break;
			}
		
			Execute_OnInteractionCanceledEvent(this);
		}
	}
}

void UMounteaInteractableComponentBase::InteractionCancelled_Client_Implementation(const float& TimeStopped, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		OnInteractionCanceled.Broadcast();
	}
}

void UMounteaInteractableComponentBase::InteractionLifecycleCompleted_Implementation()
{
	Execute_SetState(this, EInteractableStateV2::EIS_Completed);

	Execute_OnLifecycleCompletedEvent(this);
}

void UMounteaInteractableComponentBase::InteractionCooldownCompleted_Implementation()
{
	if (Interactor.GetInterface() != nullptr)
	{		
		if (Interactor->Execute_GetActiveInteractable(Interactor.GetObject()) == this)
		{
			ProcessToggleActive_Client(true);
			
			Execute_SetState(this, EInteractableStateV2::EIS_Awake);
		}
		else
		{
			ProcessToggleActive_Client(false);
			Execute_SetState(this, DefaultInteractableState);
		}
	}
	else
	{
		ProcessToggleActive_Client(false);
		Execute_SetState(this, DefaultInteractableState);
	}
	
	Execute_OnCooldownCompletedEvent(this);
}

void UMounteaInteractableComponentBase::OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void UMounteaInteractableComponentBase::OnInteractableStopOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void UMounteaInteractableComponentBase::OnInteractableTraced(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

void UMounteaInteractableComponentBase::OnInteractionProgressExpired(const float ExpirationTime, const TScriptInterface<IMounteaInteractorInterface>& CausingInteractor)
{
	if (!GetWorld()) return;
	
	switch (InteractableState)
	{
		case EInteractableStateV2::EIS_Paused:
			{
				GetWorld()->GetTimerManager().ClearTimer(Timer_Interaction);
				GetWorld()->GetTimerManager().ClearTimer(Timer_ProgressExpiration);
				
				auto localInteractor = Execute_GetInteractor(this);
				if (Execute_DoesHaveInteractor(this) && localInteractor.GetObject() && localInteractor->Execute_GetActiveInteractable(localInteractor.GetObject()) == this)
				{
					Execute_SetState(this, EInteractableStateV2::EIS_Active);
				}
				else
				{
					Execute_OnInteractionStoppedEvent(this, ExpirationTime, CausingInteractor);
				}
			}
			break;
		case EInteractableStateV2::EIS_Active: break;
		case EInteractableStateV2::EIS_Awake: break;
		case EInteractableStateV2::EIS_Cooldown: break;
		case EInteractableStateV2::EIS_Completed: break;
		case EInteractableStateV2::EIS_Disabled: break;
		case EInteractableStateV2::EIS_Suppressed: break;
		case EInteractableStateV2::EIS_Asleep: break;
		case EInteractableStateV2::Default: break;
	}
}

void UMounteaInteractableComponentBase::InteractableComponentActivated(UActorComponent* Component, bool bReset)
{
	if (IsActive())
	{
		AutoSetup();
	}
}

void UMounteaInteractableComponentBase::InteractableSelected_Implementation(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
 	if (Interactable == this)
 	{
 		Execute_SetState(this, EInteractableStateV2::EIS_Active);
 		OnInteractableSelected.Broadcast(Interactable);

 		if (GetOwner() && GetOwner()->HasAuthority())
 		{
 			if (UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(GetWorld()))
 			{
 				ProcessToggleActive(false);
 			}
 			else
 			{
 				ProcessToggleActive_Client(false);
 			}
 		}
 	}
	else
	{
		if (GetOwner() && GetOwner()->HasAuthority())
		{
			if (UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(GetWorld()))
			{
				ProcessToggleActive(false);
			}
			else
			{
				ProcessToggleActive_Client(false);
			}
		}
		
		OnInteractionCanceled.Broadcast();
		
		Execute_SetState(this, DefaultInteractableState);
		OnInteractorLost.Broadcast(Execute_GetInteractor(this));
	}
}

void UMounteaInteractableComponentBase::InteractableLost_Implementation(const TScriptInterface<IMounteaInteractableInterface>& Interactable)
{
	if (Interactable == this)
	{
		switch (InteractableState)
		{
			case EInteractableStateV2::EIS_Active:
				Execute_SetState(this, DefaultInteractableState);
				break;
			case EInteractableStateV2::EIS_Cooldown:
			case EInteractableStateV2::EIS_Awake:
				if (Execute_GetInteractor(this).GetObject() == nullptr)
				{
					Execute_SetState(this, DefaultInteractableState);
				}
				break;
			case EInteractableStateV2::EIS_Asleep:
			case EInteractableStateV2::EIS_Completed:
			case EInteractableStateV2::EIS_Disabled:
			case EInteractableStateV2::EIS_Suppressed:
			case EInteractableStateV2::Default:
			default: break;
		}
		
		OnInteractorLost.Broadcast(Execute_GetInteractor(this));
	}
}

void UMounteaInteractableComponentBase::FindAndAddCollisionShapes_Implementation()
{
	for (const auto& Itr : CollisionOverrides)
	{
		if (const auto NewCollision = UMounteaInteractionSystemBFL::FindPrimitiveByName(Itr, GetOwner()))
		{
			Execute_AddCollisionComponent(this, NewCollision);
			Execute_BindCollisionShape(this, NewCollision);
		}
		else
		{
			if (const auto NewCollisionByTag = UMounteaInteractionSystemBFL::FindPrimitiveByTag(Itr, GetOwner()))
			{
				Execute_AddCollisionComponent(this, NewCollisionByTag);
				Execute_BindCollisionShape(this, NewCollisionByTag);
			}
			else LOG_ERROR(TEXT("[Actor Interactable Component] Primitive Component '%s' not found!"), *Itr.ToString())
		}
	}
}

void UMounteaInteractableComponentBase::FindAndAddHighlightableMeshes_Implementation()
{
	for (const auto& Itr : HighlightableOverrides)
	{
		if (const auto NewMesh = UMounteaInteractionSystemBFL::FindMeshByName(Itr, GetOwner()))
		{
			Execute_AddHighlightableComponent(this, NewMesh);
			Execute_BindHighlightableMesh(this, NewMesh);
		}
		else
		{
			if (const auto NewHighlightByTag = UMounteaInteractionSystemBFL::FindMeshByTag(Itr, GetOwner()))
			{
				Execute_AddHighlightableComponent(this, NewHighlightByTag);
				Execute_BindHighlightableMesh(this, NewHighlightByTag);
			}
			else
				LOG_ERROR(TEXT("[Actor Interactable Component] Mesh Component '%s' not found!"), *Itr.ToString())
		}
	}
}

bool UMounteaInteractableComponentBase::TriggerCooldown_Implementation()
{
	if (LifecycleCount != -1)
	{
		const int32 TempRemainingLifecycleCount = RemainingLifecycleCount - 1;
		RemainingLifecycleCount = FMath::Max(0, TempRemainingLifecycleCount);
	}
	
	if (GetWorld())
	{
		if (RemainingLifecycleCount == 0) return false;

		Execute_SetState(this, EInteractableStateV2::EIS_Cooldown);

		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &UMounteaInteractableComponentBase::OnCooldownCompletedCallback);
		
		GetWorld()->GetTimerManager().SetTimer
		(
			Timer_Cooldown,
			Delegate,
			CooldownPeriod,
			false
		);

		LOG_INFO(TEXT("[TriggerCooldown] Cooldown triggered"))

		/*
		for (const auto& Itr : CollisionComponents)
		{
			Execute_UnbindCollisionShape(this, Itr);
		}
		*/

		OnInteractionCycleCompleted.Broadcast(GetWorld()->GetTimeSeconds(), RemainingLifecycleCount, Execute_GetInteractor(this));
		return true;
	}

	return false;
}

void UMounteaInteractableComponentBase::ToggleWidgetVisibility_Implementation(const bool IsVisible)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(GetWorld()))
		{
			if (IsVisible)
				ProcessShowWidget();
			else
				ProcessHideWidget();
		}
		else
		{
			if (IsVisible)
				ShowWidget_Client();
			else
				HideWidget_Client();
		}
	}
	else
	{
		if (IsVisible)
			ShowWidget_Client();
		else
			HideWidget_Client();
	}
}

void UMounteaInteractableComponentBase::BindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const
{
	if (!PrimitiveComponent) return;
	
	PrimitiveComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableBeginOverlap);
	PrimitiveComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &UMounteaInteractableComponentBase::OnInteractableStopOverlap);

	FCollisionShapeCache CachedValues;
	CachedValues.bGenerateOverlapEvents = PrimitiveComponent->GetGenerateOverlapEvents();
	CachedValues.CollisionEnabled = PrimitiveComponent->GetCollisionEnabled();
	CachedValues.CollisionResponse = GetCollisionResponseToChannel(CollisionChannel);
	
	CachedCollisionShapesSettings.Add(PrimitiveComponent, CachedValues);
	
	PrimitiveComponent->SetGenerateOverlapEvents(true);
	PrimitiveComponent->SetCollisionResponseToChannel(CollisionChannel, ECollisionResponse::ECR_Overlap);

	switch (PrimitiveComponent->GetCollisionEnabled())
	{
		case ECollisionEnabled::NoCollision:
			PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			break;
		case ECollisionEnabled::QueryOnly:
		case ECollisionEnabled::PhysicsOnly:
		case ECollisionEnabled::QueryAndPhysics:
		default: break;
	}
}

void UMounteaInteractableComponentBase::UnbindCollisionShape_Implementation(UPrimitiveComponent* PrimitiveComponent) const
{
	if(!PrimitiveComponent) return;
	
	PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UMounteaInteractableComponentBase::OnInteractableBeginOverlap);
	PrimitiveComponent->OnComponentEndOverlap.RemoveDynamic(this, &UMounteaInteractableComponentBase::OnInteractableStopOverlap);

	if (CachedCollisionShapesSettings.Find(PrimitiveComponent))
	{
		PrimitiveComponent->SetGenerateOverlapEvents(CachedCollisionShapesSettings[PrimitiveComponent].bGenerateOverlapEvents);
		PrimitiveComponent->SetCollisionEnabled(CachedCollisionShapesSettings[PrimitiveComponent].CollisionEnabled);
		PrimitiveComponent->SetCollisionResponseToChannel(CollisionChannel, CachedCollisionShapesSettings[PrimitiveComponent].CollisionResponse);
	}
	else
	{
		PrimitiveComponent->SetGenerateOverlapEvents(true);
		PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		PrimitiveComponent->SetCollisionResponseToChannel(CollisionChannel, ECollisionResponse::ECR_Overlap);
	}
}

void UMounteaInteractableComponentBase::BindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const
{
	if (!MeshComponent) return;
	
	MeshComponent->SetRenderCustomDepth(true);
}

void UMounteaInteractableComponentBase::UnbindHighlightableMesh_Implementation(UMeshComponent* MeshComponent) const
{
	if (!MeshComponent) return;
	
	MeshComponent->SetRenderCustomDepth(false);
}

void UMounteaInteractableComponentBase::ToggleDebug_Implementation()
{
	DebugSettings.DebugMode = !DebugSettings.DebugMode;
}

FDebugSettings UMounteaInteractableComponentBase::GetDebugSettings_Implementation() const
{
	return DebugSettings;
}

void UMounteaInteractableComponentBase::AutoSetup()
{
	switch (SetupType)
	{
		case ESetupType::EST_FullAll:
			{
				if (GetOwner() == nullptr) break;

				TArray<UPrimitiveComponent*> OwnerPrimitives;
				GetOwner()->GetComponents(OwnerPrimitives);

				TArray<UMeshComponent*> OwnerMeshes;
				GetOwner()->GetComponents(OwnerMeshes);

				for (const auto& Itr : OwnerPrimitives)
				{
					if (Itr)
					{
						Execute_AddCollisionComponent(this, Itr);
					}
				}

				for (const auto& Itr : OwnerMeshes)
				{
					if (Itr)
					{
						Execute_AddHighlightableComponent(this, Itr);
					}
				}
			}
			break;
		case ESetupType::EST_AllParent:
			{
				// Get all Parent Components
				TArray<USceneComponent*> ParentComponents;
				GetParentComponents(ParentComponents);

				// Iterate over them and assign them properly
				if (ParentComponents.Num() > 0)
				{
					for (const auto& Itr : ParentComponents)
					{
						if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Itr))
						{
							Execute_AddCollisionComponent(this, PrimitiveComp);

							if (UMeshComponent* MeshComp = Cast<UMeshComponent>(PrimitiveComp))
							{
								Execute_AddHighlightableComponent(this, MeshComp);
							}
						}
					}
				}
			}
			break;
		case ESetupType::EST_Quick:
			{
				if (USceneComponent* ParentComponent = GetAttachParent())
				{
					if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(ParentComponent))
					{
						Execute_AddCollisionComponent(this, PrimitiveComp);

						if (UMeshComponent* MeshComp = Cast<UMeshComponent>(PrimitiveComp))
						{
							Execute_AddHighlightableComponent(this, MeshComp);
						}
					}
				}
			}
			break;
		default:
			break;
	}
	
	Execute_FindAndAddCollisionShapes(this);
	Execute_FindAndAddHighlightableMeshes(this);
}

void UMounteaInteractableComponentBase::OnCooldownCompletedCallback()
{
	if (!GetWorld())
	{
		LOG_ERROR(TEXT("[TriggerCooldown] Interactable has no World, cannot request OnCooldownCompletedEvent!"))
		return;
	}
	
	for (const auto& Itr : CollisionComponents)
	{
		Execute_BindCollisionShape(this, Itr);
	}
	
	OnCooldownCompleted.Broadcast();
}

bool UMounteaInteractableComponentBase::ValidateInteractable() const
{
	if (GetWidgetClass().Get() == nullptr)
	{
		LOG_ERROR(TEXT("[%s] Has null Widget Class! Disabled!"), *GetName())
		return false;
	}
	
	if (GetWidgetClass()->ImplementsInterface(UActorInteractionWidget::StaticClass()) == false)
	{
		LOG_ERROR(TEXT("[%s] Has invalid Widget Class! Widget Class must implament `ActorInteractionWidget` interface!"), *GetName())
		return false;
	}

	return true;
}

void UMounteaInteractableComponentBase::UpdateInteractionWidget()
{
	if (UUserWidget* UserWidget = GetWidget() )
	{
		if (UserWidget->Implements<UActorInteractionWidget>())
		{
			TScriptInterface<IActorInteractionWidget> InteractionWidget = UserWidget;
			InteractionWidget.SetObject(UserWidget);
			InteractionWidget.SetInterface(Cast<IActorInteractionWidget>(UserWidget));

			InteractionWidget->Execute_UpdateWidget(UserWidget, this);
		}
	}
}

void UMounteaInteractableComponentBase::InteractableDependencyStartedCallback_Implementation(const TScriptInterface<IMounteaInteractableInterface>& NewMaster)
{
	if (NewMaster.GetObject() == nullptr) return;

	// Force lower weight but never higher than it was before.
	const int32 NewWeight = FMath::Min(InteractionWeight, NewMaster->Execute_GetInteractableWeight(this) - 1);
	Execute_SetInteractableWeight(this, NewWeight);
}

void UMounteaInteractableComponentBase::InteractableDependencyStoppedCallback_Implementation(const TScriptInterface<IMounteaInteractableInterface>& FormerMaster)
{
	Execute_SetInteractableWeight(this, CachedInteractionWeight);
}

void UMounteaInteractableComponentBase::StartHighlight_Client_Implementation()
{
	ProcessStartHighlight();
}

void UMounteaInteractableComponentBase::StopHighlight_Client_Implementation()
{
	ProcessStopHighlight();
}

void UMounteaInteractableComponentBase::ProcessToggleActive_Client_Implementation(const bool bIsEnabled)
{
	ProcessToggleActive(bIsEnabled);
}

void UMounteaInteractableComponentBase::ShowWidget_Client_Implementation()
{
	ProcessShowWidget();
}

void UMounteaInteractableComponentBase::HideWidget_Client_Implementation()
{
	ProcessHideWidget();
}

void UMounteaInteractableComponentBase::OnRep_InteractableState()
{
	switch (InteractableState)
	{
		case EInteractableStateV2::EIS_Active:
		case EInteractableStateV2::EIS_Awake:
		{
			break;
		}
		case EInteractableStateV2::EIS_Paused:
		case EInteractableStateV2::EIS_Suppressed:
		case EInteractableStateV2::EIS_Cooldown:
		case EInteractableStateV2::EIS_Disabled:	
		case EInteractableStateV2::EIS_Completed:
		case EInteractableStateV2::EIS_Asleep:
		case EInteractableStateV2::Default: 
		default:
		{
			Execute_StopHighlight(this);
			break;
		}
	}
}

void UMounteaInteractableComponentBase::OnRep_ActiveInteractor()
{
	if (Interactor.GetObject() == nullptr)
	{
		Execute_ToggleWidgetVisibility(this, false);

		Execute_StopHighlight(this);
	}
}

void UMounteaInteractableComponentBase::ProcessToggleActive(const bool bIsEnabled)
{
	if (bIsEnabled)
	{
		Execute_ToggleWidgetVisibility(this, true);
		Execute_StartHighlight(this);
	}
	else
	{
		Execute_ToggleWidgetVisibility(this, false);
		Execute_StopHighlight(this);
	}
}

void UMounteaInteractableComponentBase::ProcessStartHighlight()
{
	SetHiddenInGame(false, true);
	switch (HighlightType)
	{
		case EHighlightType::EHT_PostProcessing:
			{
				for (const auto& Itr : HighlightableComponents)
				{
					Itr->SetRenderCustomDepth(bInteractionHighlight);
					Itr->SetCustomDepthStencilValue(StencilID);
				}
			}
			break;
		case EHighlightType::EHT_OverlayMaterial:
			{
				for (const auto& Itr : HighlightableComponents)
				{
					Itr->SetOverlayMaterial(HighlightMaterial);
				}
			}
		case EHighlightType::EHT_Default:
		default:
			break;
	}
}

void UMounteaInteractableComponentBase::ProcessStopHighlight()
{
	SetHiddenInGame(true, true);
	switch (HighlightType)
	{
		case EHighlightType::EHT_PostProcessing:
			{
				for (const auto& Itr : HighlightableComponents)
				{
					Itr->SetCustomDepthStencilValue(0);
				}
			}
			break;
		case EHighlightType::EHT_OverlayMaterial:
			{
				for (const auto& Itr : HighlightableComponents)
				{
					Itr->SetOverlayMaterial(nullptr);
				}
			}
		case EHighlightType::EHT_Default:
		default:
			break;
	}
}

void UMounteaInteractableComponentBase::ProcessShowWidget()
{
	if (GetWidget())
	{
		UpdateInteractionWidget();

		SetHiddenInGame(false);
		SetVisibility(true);
		
		OnInteractableWidgetVisibilityChanged.Broadcast(true);
	}
}

void UMounteaInteractableComponentBase::ProcessHideWidget()
{
	if (GetWidget())
	{
		UpdateInteractionWidget();

		SetHiddenInGame(true);
		SetVisibility(false);

		OnInteractableWidgetVisibilityChanged.Broadcast(false);
	}
}

void UMounteaInteractableComponentBase::InteractorActionConsumed(UInputAction* ConsumedAction)
{
	OnInputActionConsumed.Broadcast(ConsumedAction);
}

void UMounteaInteractableComponentBase::OnInputModeChanged(ECommonInputType CommonInput)
{
	if (UMounteaInteractionSystemBFL::CanExecuteCosmeticEvents(GetWorld()))
	{
		if (!GetWidget())
		{
			return;
		}
		
		if (const auto localPlayer = GetWidget()->GetOwningLocalPlayer())
		{
			if (UCommonInputSubsystem* commonInputSubsystem = UCommonInputSubsystem::Get(localPlayer))
			{
				
				const auto currentInputType = commonInputSubsystem->GetCurrentInputType();
				const auto currentInputName = commonInputSubsystem->GetCurrentGamepadName();
				
				OnInteractionDeviceChanged.Broadcast(currentInputType, currentInputName);
			}
		}
	}
}

void UMounteaInteractableComponentBase::OnInputDeviceChanged_Implementation(const ECommonInputType DeviceType, const FName& DeviceName)
{
	FString InputTypeString = GetEnumValueAsString<ECommonInputType>("ECommonInputType", DeviceType);
}

#if WITH_EDITOR || WITH_EDITORONLY_DATA

void UMounteaInteractableComponentBase::SetDefaultValues()
{
	Execute_SetDefaults(this);
}

#endif

void UMounteaInteractableComponentBase::SetState_Server_Implementation(const EInteractableStateV2 NewState)
{
	Execute_SetState(this, NewState);
}

#if (!UE_BUILD_SHIPPING || WITH_EDITOR)
#if WITH_EDITOR

void UMounteaInteractableComponentBase::ResetDefaults()
{
	Execute_SetDefaults(this);
}

void UMounteaInteractableComponentBase::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	
	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;

	FString interactableName = GetName();
	// Format Name
	{
		if (interactableName.Contains(TEXT("_GEN_VARIABLE")))
		{
			interactableName.ReplaceInline(TEXT("_GEN_VARIABLE"), TEXT(""));
		}
		if(interactableName.EndsWith(TEXT("_C")) && interactableName.StartsWith(TEXT("Default__")))
		{
		
			interactableName.RightChopInline(9);
			interactableName.LeftChopInline(2);
		}
	}
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaInteractableComponentBase, DefaultInteractableState))
	{
		if
		(
			DefaultInteractableState == EInteractableStateV2::EIS_Active			||
			DefaultInteractableState == EInteractableStateV2::EIS_Completed	||
			DefaultInteractableState == EInteractableStateV2::EIS_Cooldown
		)
		{
			const FText ErrorMessage = FText::FromString
			(
				interactableName.Append(TEXT(": DefaultInteractableState cannot be ")).Append(GetEnumValueAsString("EInteractableStateV2", DefaultInteractableState)).Append(TEXT("!"))
			);
			FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));

			DefaultInteractableState = EInteractableStateV2::EIS_Awake;
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaInteractableComponentBase, LifecycleCount))
	{
		if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
		{
			if (LifecycleCount == 0 || LifecycleCount == 1)
			{
				if (DebugSettings.EditorDebugMode)
				{
					const FText ErrorMessage = FText::FromString
					(
						interactableName.Append(TEXT(": Cycled LifecycleCount cannot be: ")).Append(FString::FromInt(LifecycleCount)).Append(TEXT("!"))
					);
				
					FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
				}
				
				LifecycleCount = 2.f;
				RemainingLifecycleCount = LifecycleCount;
			}
		}
	}
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaInteractableComponentBase, InteractionPeriod))
	{
		if (InteractionPeriod < -1.f)
		{
			const FText ErrorMessage = FText::FromString
			(
				interactableName.Append(TEXT(": InteractionPeriod cannot be less than -1!"))
			);
			FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));

			InteractionPeriod = -1.f;
		}

		if (InteractionPeriod > -1.f && InteractionPeriod < 0.1f)
		{
			InteractionPeriod = 0.1f;
		}

		if (FMath::IsNearlyZero(InteractionPeriod, 0.001f))
		{
			InteractionPeriod = 0.1f;
		}
	}
		
	if (PropertyName == TEXT("WidgetClass"))
	{
		if (GetWidgetClass() == nullptr)
		{
			const FText ErrorMessage = FText::FromString
			(
				interactableName.Append(TEXT(": Widget Class is NULL!"))
			);
			FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
		}
		else
		{
			if (GetWidgetClass()->ImplementsInterface(UActorInteractionWidget::StaticClass()) == false)
			{
				const FText ErrorMessage = FText::FromString
				(
					interactableName.Append(TEXT(": Widget Class must either implement 'ActorInteractionWidget Interface'!"))
				);
					
				SetWidgetClass(nullptr);
				FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
			}
		}
	}

	if (PropertyName == TEXT("Space"))
	{
		if (GetWidgetSpace() == EWidgetSpace::World)
		{
			SetWorldScale3D(FVector(0.01f));
			SetDrawSize(FVector2D(2000));
			bDrawAtDesiredSize = false;
		}
		else
		{
			SetWorldScale3D(FVector(1.f));
			bDrawAtDesiredSize = false;
		}

		const FText ErrorMessage = FText::FromString
		(
			interactableName.Append(TEXT(": UI Space changed! Component Scale has been updated. Update 'DrawSize' to match new Widget Space!"))
		);
		FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("NotificationList.DefaultMessage"));
	}
}

EDataValidationResult UMounteaInteractableComponentBase::IsDataValid(FDataValidationContext& Context) const
{
	const auto DefaultValue = Super::IsDataValid(Context);
	bool bAnyError = false;

	// Validation
	{
		FString interactableName = GetName();
		{
			if (interactableName.Contains(TEXT("_GEN_VARIABLE")))
			{
				interactableName.ReplaceInline(TEXT("_GEN_VARIABLE"), TEXT(""));
			}
			if(interactableName.EndsWith(TEXT("_C")) && interactableName.StartsWith(TEXT("Default__")))
			{
		
				interactableName.RightChopInline(9);
				interactableName.LeftChopInline(2);
			}
		}
	
		if
		(
			DefaultInteractableState == EInteractableStateV2::EIS_Active ||
			DefaultInteractableState == EInteractableStateV2::EIS_Completed ||
			DefaultInteractableState == EInteractableStateV2::EIS_Cooldown
		)
		{
			const FText ErrorMessage = FText::FromString
			(
				interactableName.Append(TEXT(": DefaultInteractableState cannot be ")).Append(GetEnumValueAsString("EInteractableStateV2", DefaultInteractableState)).Append(TEXT("!"))
			);
		
			Context.AddError(ErrorMessage);
			bAnyError = true;
		}

		if (InteractionPeriod < -1.f)
		{
			const FText ErrorMessage = FText::FromString
			(
				interactableName.Append(TEXT(": DefaultInteractableState cannot be lesser than -1!"))
			);
		
			Context.AddError(ErrorMessage);
			bAnyError = true;
		}
	
		if (LifecycleMode == EInteractableLifecycle::EIL_Cycled && (LifecycleCount == 0 || LifecycleCount == 1))
		{
			const FText ErrorMessage = FText::FromString
			(
				interactableName.Append(TEXT(":")).Append(TEXT(" LifecycleCount cannot be %d!"), LifecycleCount)
			);
		
			Context.AddError(ErrorMessage);
			bAnyError = true;
		}

		if (GetWidgetClass() == nullptr)
		{
			const FText ErrorMessage = FText::FromString
			(
				interactableName.Append(TEXT(": Widget Class is NULL!"))
			);

			Context.AddError(ErrorMessage);
			bAnyError = true;
		}
		else
		{
			if (GetWidgetClass()->ImplementsInterface(UActorInteractionWidget::StaticClass()) == false)
			{
				const FText ErrorMessage = FText::FromString
				(
					interactableName.Append(TEXT(" : Widget Class must either implement 'ActorInteractionWidget Interface'!"))
				);
				
				Context.AddError(ErrorMessage);
				bAnyError = true;
			}
		}
	}

	if (bAnyError && RequestEditorDefaults.IsBound())
	{
		RequestEditorDefaults.Broadcast();
	}

	if (bAnyError)
	{
		Context.AddWarning(FText::FromString("Interactable failed to Validate. `SetDefaults` has been called. Some settings might have been overriden by default values!"));
	}
	
	return bAnyError ? EDataValidationResult::Invalid : DefaultValue;
}

#endif

void UMounteaInteractableComponentBase::DrawDebug()
{
	if (DebugSettings.DebugMode)
	{
		for (const auto& Itr : CollisionComponents)
		{
			Itr->SetHiddenInGame(false);
		}
	}
}

#endif

#pragma endregion

void UMounteaInteractableComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, InteractionPeriod,						COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, DefaultInteractableState,			COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, SetupType,									COND_SimulatedOnly);	
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, CooldownPeriod,						COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, InteractableCompatibleTags,		COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, HighlightType,								COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, HighlightMaterial,						COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, InteractableName,						COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, LifecycleMode,							COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, LifecycleCount,							COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, RemainingLifecycleCount,			COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, InteractionWeight,						COND_SimulatedOnly);

	//DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, Timer_Interaction,						COND_SimulatedOnly);

	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, Interactor,									COND_None);	
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, InteractableState,						COND_None);	
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, InteractableData,						COND_None);
	DOREPLIFETIME_CONDITION(UMounteaInteractableComponentBase, CollisionChannel,						COND_None);
}

#undef LOCTEXT_NAMES