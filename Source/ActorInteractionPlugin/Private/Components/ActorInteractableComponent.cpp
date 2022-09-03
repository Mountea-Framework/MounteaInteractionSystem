// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/ActorInteractableComponent.h"

#include "Components/ActorInteractorComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"

#include "Helpers/ActorInteractionPluginLog.h"
#include "Kismet/GameplayStatics.h"

#include "Widgets/ActorInteractableWidget.h"

UActorInteractableComponent::UActorInteractableComponent()
{
	UActorComponent::SetComponentTickEnabled(false);
	SetIsReplicatedByDefault(true);
	
	// Set default User Widget if none is selected.
	#if WITH_EDITOR
	if (!InteractableWidgetClass.Get())
	{
		static ConstructorHelpers::FClassFinder<UActorInteractableWidget>DefaultWidget(TEXT("/ActorInteractionPlugin/UMG/Examples/WBP_InteractableActorWidget_Example1"));
		if (DefaultWidget.Class.Get() != nullptr)
		{
			InteractableWidgetClass = DefaultWidget.Class;
		}
		else
		{
			InteractableWidgetClass = UActorInteractableWidget::StaticClass();
		}
	}
	#endif

	InteractableWidgetClass = WidgetClass;
	SetWidgetClass(InteractableWidgetClass);
	
	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(64, 64);
	bDrawAtDesiredSize = true;
	UActorComponent::SetActive(true);
	SetHiddenInGame(true);

	bAutoRegister = true;
	bAutoActivate = true;
	bInteractableAutoActivate = true;
	bInteractionHighlight = true;
	
	ComponentTags.Add(INTERACTABLE_TAG_NAME);
	
	UpdateCollisionChannels();

	UpdateLifecycleLogic();
}

void UActorInteractableComponent::BeginPlay()
{
	// Another wacky hack to overcome semi-transient properties UEditor makes
	SetInteractableWidgetClass(InteractableWidgetClass);
	
	Super::BeginPlay();

	if(!GetOwner())
	{
		AIP_LOG(Error, TEXT("[BeginPlay] Cannot get Owner reference. Please report this bug."))
		
		UnregisterComponent();
		MarkPendingKill();
		return;
	}
	if(!GetWorld())
	{
		AIP_LOG(Error, TEXT("[BeginPlay] Cannot get World reference. Please report this bug."))
		
		UnregisterComponent();
		MarkPendingKill();
		return;
	}
	if (InteractableWidgetClass.Get() == nullptr)
	{
		AIP_LOG(Error, TEXT("[BeginPlay] Invalid Interactable Widget Class!"))
	}
	if (CollisionShapes.Num())
	{
		for (const auto Itr : CollisionShapes)
		{
			BindCollisionEvents(Itr);
		}
	}
	
	OnInteractorFound.AddUniqueDynamic(this, &UActorInteractableComponent::InteractorFound);
	OnInteractorLost.AddUniqueDynamic(this, &UActorInteractableComponent::InteractorLost);
	OnInteractableTraced.AddUniqueDynamic(this, &UActorInteractableComponent::OnInteractorTraced);

	// Add required tag if missing
	if(!ComponentHasTag(INTERACTABLE_TAG_NAME))
	{
		ComponentTags.Add(INTERACTABLE_TAG_NAME);
	}
	
	UpdateCollisionChannels();
	
	if (GetInteractableAutoActivate())
	{
		FString ErrorMessage;
		if (!ActivateInteractable(ErrorMessage))
		{
			AIP_LOG(Error, TEXT("[BeginPlay] %s"), *ErrorMessage)
		}
	}
	else
	{
		DeactivateInteractable();
	}

	UpdateLifecycleLogic();
	
	InitializeInteractionComponent();
}

void UActorInteractableComponent::OnActorBeginsOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	switch (GetInteractionState())
	{
		case EInteractableState::EIS_Inactive:
			break;
		case EInteractableState::EIS_Cooldown:
			return;
		case EInteractableState::EIS_Active:
			AIP_LOG(Error, TEXT("[OnActorBeginsOverlapping] Interactable with Active State has been overlapped. This might be a Bug, please report it!"))
			return;
		case EInteractableState::EIS_Disabled:
		case EInteractableState::EIS_Standby:
		case EInteractableState::EIS_Finished:
		case EInteractableState::Default:
		default:
			return;
	}
	
	ensure(IsValid(OtherActor));
	
	if (UActorInteractorComponent* InteractorComponent = Cast<UActorInteractorComponent>(OtherActor->GetComponentByClass(UActorInteractorComponent::StaticClass())))
	{
		// Allowed only with passive Interactors
		if (InteractorComponent->GetInteractorType() != EInteractorType::EIT_Active)
		{
			OnInteractorFound.Broadcast(InteractorComponent);
			OnInteractorOverlapped.Broadcast(OverlappedComponent);
		}
	}
}

void UActorInteractableComponent::OnActorStopsOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	switch (GetInteractionState())
	{
		case EInteractableState::EIS_Inactive:
			break;
		case EInteractableState::EIS_Cooldown:
			SetLastInteractionTime(GetWorld()->GetTimeSeconds());
			return;
		case EInteractableState::EIS_Active:
		case EInteractableState::EIS_Standby:
			SetLastInteractionTime(GetWorld()->GetTimeSeconds());
			SetInteractionState(EInteractableState::EIS_Inactive);
			break;
		case EInteractableState::EIS_Disabled:
		case EInteractableState::EIS_Finished:
		case EInteractableState::Default:
		default:
			return;
	}

	ensure(IsValid(OtherActor));
	
	if (UActorInteractorComponent* InteractorComponent = Cast<UActorInteractorComponent>(OtherActor->GetComponentByClass(UActorInteractorComponent::StaticClass())))
	{
		// Allowed only with passive Interactors
		if (InteractorComponent->GetInteractorType() != EInteractorType::EIT_Active)
		{
			OnInteractorOverlapped.RemoveAll(this);
			OnInteractorLost.Broadcast(InteractorComponent);
		}
	}
}

void UActorInteractableComponent::SetMeshComponentsHighlight(const bool bShowHighlight)
{
	if(ListOfHighlightables.Num())
	{
		for (UPrimitiveComponent* Itr : ListOfHighlightables)
		{
			Itr->SetCustomDepthStencilValue(bShowHighlight ? StencilID : 0);
		}
	}
}

void UActorInteractableComponent::OnWidgetClassChanged()
{
	SetWidgetClass(InteractableWidgetClass);
	
	if (GetWidget())
	{
		GetWidget()->RemoveFromParent();
		SetWidget(nullptr);

		InitWidget();
	}
}

void UActorInteractableComponent::InteractorFound(UActorInteractorComponent* FoundInteractor)
{
	// TODO This is soooo ugly, change it!
	if (FoundInteractor->GetInteractingWith() != this)
	{
		FoundInteractor->OnInteractableLost.Broadcast(FoundInteractor->GetInteractingWith());
	}
	
	SetMeshComponentsHighlight(bInteractionHighlight);
	
	SetInteractionState(EInteractableState::EIS_Standby);
	SetInteractorComponent(FoundInteractor);
	
	SetHiddenInGame(false);
	UpdateInteractableWidget();
		
	FoundInteractor->OnInteractableFound.Broadcast(this);

	const float Time = GetWorld()->GetTimeSeconds();

	switch (GetInteractionType())
	{
		case EInteractableType::EIT_Hold:
		case EInteractableType::EIT_Press:
		case EInteractableType::EIT_Hybrid:
			FoundInteractor->OnInteractionKeyPressed.AddUniqueDynamic(this, &UActorInteractableComponent::StartInteraction);
			FoundInteractor->OnInteractionKeyReleased.AddUniqueDynamic(this, &UActorInteractableComponent::StopInteraction);
			FoundInteractor->OnInteractorTypeChanged.AddUniqueDynamic(this, &UActorInteractableComponent::InteractorChanged);
			FoundInteractor->OnInteractableLost.AddUniqueDynamic(this, &UActorInteractableComponent::CancelInteraction);
			break;
		case EInteractableType::EIT_Mash:
			FoundInteractor->OnInteractionKeyPressed.AddUniqueDynamic(this, &UActorInteractableComponent::StartInteraction);
			FoundInteractor->OnInteractionKeyPressed.AddUniqueDynamic(this, &UActorInteractableComponent::StartMashing);
			FoundInteractor->OnInteractorTypeChanged.AddUniqueDynamic(this, &UActorInteractableComponent::InteractorChanged);
			FoundInteractor->OnInteractableLost.AddUniqueDynamic(this, &UActorInteractableComponent::CancelInteraction);
		case EInteractableType::EIT_Auto:
			StartInteraction(Time);
			FoundInteractor->OnInteractorTypeChanged.AddUniqueDynamic(this, &UActorInteractableComponent::InteractorChanged);
			FoundInteractor->OnInteractableLost.AddUniqueDynamic(this, &UActorInteractableComponent::CancelInteraction);
			break;
		case EInteractableType::Default:
		default: break;
	}
}

void UActorInteractableComponent::InteractorLost(UActorInteractorComponent* LostInteractor)
{
	StopInteractionLink(LostInteractor);
}

void UActorInteractableComponent::OnInteractorTraced(UActorInteractorComponent* TracingInteractor)
{
	switch (GetInteractionState())
	{
		case EInteractableState::EIS_Inactive:
			break;
		case EInteractableState::EIS_Disabled:
		case EInteractableState::EIS_Active:
		case EInteractableState::EIS_Cooldown:
		case EInteractableState::EIS_Standby:
		case EInteractableState::EIS_Finished:
		case EInteractableState::Default:
		default:
			return;
	}
	
	if (TracingInteractor)
	{
		// Already using this component
		if (GetInteractorComponent() == TracingInteractor)
		{
			return;
		}
		
		// Not allowed for Passive Interactors
		if (TracingInteractor->GetInteractorType() == EInteractorType::EIT_Passive)
		{
			return;
		}

		OnInteractorFound.Broadcast(TracingInteractor);
	}
}

void UActorInteractableComponent::InteractorChanged(float TimeHappened)
{
	OnInteractorLost.Broadcast(InteractingInteractorComponent);
}

void UActorInteractableComponent::StopInteractionLink(UActorInteractorComponent* OtherComponent)
{
	if (!OtherComponent) return;

	if (OtherComponent->GetInteractingWith() == this)
	{
		OtherComponent->OnInteractionKeyPressed.RemoveDynamic(this, &UActorInteractableComponent::StartInteraction);
		OtherComponent->OnInteractionKeyReleased.RemoveDynamic(this, &UActorInteractableComponent::StopInteraction);
		OtherComponent->OnInteractorTypeChanged.RemoveDynamic(this, &UActorInteractableComponent::InteractorChanged);

		OtherComponent->OnInteractableLost.RemoveDynamic(this, &UActorInteractableComponent::CancelInteraction);
		
		OtherComponent->OnInteractionKeyPressed.RemoveDynamic(this, &UActorInteractableComponent::StartMashing);
		
		OnInteractorOverlapped.RemoveAll(this);

		OtherComponent->OnInteractableLost.Broadcast(nullptr);

		SetInteractorComponent(nullptr);

		SetHiddenInGame(true);
		SetMeshComponentsHighlight(false);
		SetRemainingInteractionProgress(1.f);

		SetInteractionState(EInteractableState::EIS_Inactive);
	}
}

void UActorInteractableComponent::SetInteractionType(const EInteractableType NewType)
{
	InteractionType = NewType;

	if (InteractionType == EInteractableType::EIT_Auto)
	{
		SetInteractionLifecycleType(EInteractableLifecycle::EIL_OnlyOnce);
	}
}

void UActorInteractableComponent::SetInteractionLifecycleType(const EInteractableLifecycle NewLifecycleType)
{
	InteractableLifecycle = NewLifecycleType;

	if (InteractableLifecycle == EInteractableLifecycle::EIL_OnlyOnce)
	{
		SetInteractionAllowedLifecycles(0);
	}
}

void UActorInteractableComponent::SetInteractionState(const EInteractableState NewState)
{
	/* Full State Machine is available in Documentation
	 * @see https://sites.google.com/view/dominikpavlicek/home/documentation
	*/
	switch (GetInteractionState())
	{
		case EInteractableState::EIS_Standby:
			if (NewState == EInteractableState::EIS_Active || NewState == EInteractableState::EIS_Inactive)
			{
				InteractableState = NewState;
			}
			break;
		case EInteractableState::EIS_Active:
			if (NewState == EInteractableState::EIS_Cooldown || NewState == EInteractableState::EIS_Finished || NewState == EInteractableState::EIS_Standby || NewState == EInteractableState::EIS_Inactive)
			{
				InteractableState = NewState;
			}
			break;
		case EInteractableState::EIS_Inactive:
			if (NewState == EInteractableState::EIS_Standby || NewState == EInteractableState::EIS_Finished)
			{
				InteractableState = NewState;
			}
			break;
		case EInteractableState::EIS_Cooldown:
			if (NewState == EInteractableState::EIS_Active || NewState == EInteractableState::EIS_Standby || NewState == EInteractableState::EIS_Inactive)
			{
				InteractableState = NewState;
			}
			break;
		case EInteractableState::EIS_Finished:
				InteractableState = NewState;
		case EInteractableState::EIS_Disabled:
			if(NewState == EInteractableState::EIS_Inactive)
			{
				InteractableState = NewState;
			}
		case EInteractableState::Default:
		default:
			break;
	}
}

void UActorInteractableComponent::InitializeInteractionComponent()
{
	InteractionWidget = Cast<UActorInteractableWidget>(GetUserWidgetObject());
	if (InteractionWidget)
	{
		InteractionWidget->InitializeInteractionWidget
		(
			GetInteractionActionKey(),
			GetInteractionActionTitle(),
			GetInteractionActionKey(),
			this,
			GetInteractionActionTexture()
		);
	}

	if (!bInteractableAutoActivate)
	{
		DeactivateInteractable();
	}
	
	UpdateInteractableWidget();

	// Force allow Stencil for those Meshes
	if(ListOfHighlightables.Num())
	{
		for (UPrimitiveComponent* Itr : ListOfHighlightables)
		{
			Itr->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_Default);
		}
	}
}

void UActorInteractableComponent::SetRemainingInteractionProgress(const float& RemainingProgress) const
{
	if (InteractionWidget)
	{
		InteractionWidget->SetInteractionProgress(RemainingProgress);
	}	
}

void UActorInteractableComponent::StartInteraction(const float TimeKeyPressed)
{
	if(!CanInteract()) return;

	OnInteractionStarted.Broadcast(GetInteractionType(), TimeKeyPressed);
	
	SetInteractionState(EInteractableState::EIS_Active);
	SetLastInteractionTime(TimeKeyPressed);

	AIP_LOG(Warning, TEXT("KeyPressed"))

	switch (GetInteractionType())
	{
		case EInteractableType::EIT_Press:
			FinishInteraction(TimeKeyPressed);
			break;
		case EInteractableType::EIT_Hybrid:
		case EInteractableType::EIT_Hold:
		case EInteractableType::EIT_Auto:
			GetWorld()->GetTimerManager().SetTimer
			(
				TimerHandle_InteractionTime,
				this,
				&UActorInteractableComponent::FinishInteraction_TimerFunction,
				GetInteractionTime()
			);
			break;
		case EInteractableType::EIT_Mash:
			if(!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_InteractionTime))
			{
				GetWorld()->GetTimerManager().SetTimer
				(
					TimerHandle_InteractionTime,
					this,
					&UActorInteractableComponent::FinishInteraction_TimerFunction,
					GetInteractionTime()
				);
			}
			break;
		default:
			break;
	}
}

void UActorInteractableComponent::StopInteraction(float TimeKeyReleased)
{
	// We are no longer interacting anyway
	if (!GetInteractorComponent())
	{
		return;
	}

	bool bHybridAutoFinish = false;
	if (GetInteractionType() == EInteractableType::EIT_Hybrid)
	{
		if (TimeKeyReleased - LastInteractionTime < HybridTimeThreshold)
		{
			bHybridAutoFinish = true;
		}
	}
	
	OnInteractionStopped.Broadcast();
	
	if (TimeKeyReleased - GetLastInteractionTime() > GetInteractionTime() || bHybridAutoFinish)
	{
		FinishInteraction(TimeKeyReleased);
	}
	else
	{
		SetInteractionState(EInteractableState::EIS_Standby);
		SetLastInteractionTime(TimeKeyReleased);

		SetRemainingInteractionProgress(1.f);
	}	
}

void UActorInteractableComponent::FinishInteraction(float TimeInteractionFinished)
{
	if (GetInteractionState() != EInteractableState::EIS_Active) return;

	const float FinishTime = GetWorld()->GetTimeSeconds();
	
	SetMeshComponentsHighlight(false);

	bool bMashFailed = false;
	if (GetInteractionType() == EInteractableType::EIT_Mash)
	{
		bMashFailed = InteractionPressed < MinMashAmountRequired;
		InteractionPressed = 0;
	}
	
	if (GetInteractionLifecycle() == EInteractableLifecycle::EIL_Cycled)
	{
		// Increment only if Mash didn't fail or not even Mash type
		if (!bMashFailed)
		{
			IncrementInteractionPassedLifecycles();
		}
		
		const int32 RemainingCycles = GetMaxAllowedLifecycles() == -1 ? -1 : GetMaxAllowedLifecycles() - GetPassedLifecycles();

		if (RemainingCycles == -1 || RemainingCycles > 0)
		{
			if (GetInteractionCooldown() > KINDA_SMALL_NUMBER)
			{
				SetInteractionState(EInteractableState::EIS_Cooldown);

				SetHiddenInGame(true);
				
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CooldownTime);

				GetWorld()->GetTimerManager().SetTimer
				(
					TimerHandle_CooldownTime, this,
					&UActorInteractableComponent::CooldownElapsed_TimerFunction,
					GetInteractionCooldown()
				);
			}
			else
			{
				CooldownElapsed_TimerFunction();
			}
		}
	}
	else
	{
		if (!bMashFailed)
		{
			StopInteractionLink(GetInteractorComponent());
		
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InteractionTime);
		
			SetInteractionState(EInteractableState::EIS_Finished);
		}
	}

	// Call Mash failed before Finishing Interaction if needed
	if (bMashFailed)
	{
		OnInteractionMashFailed.Broadcast();
	}
	OnInteractionCompleted.Broadcast(GetInteractionType(), FinishTime);	
}

void UActorInteractableComponent::CancelInteraction(UActorInteractableComponent* Component)
{
	if(Component == this || Component == nullptr)
	{
		OnInteractorLost.Broadcast(GetInteractorComponent());
	}
}

void UActorInteractableComponent::StartMashing(float TimeMashHappened)
{
	if(GetInteractionState() != EInteractableState::EIS_Active) return;
	
	InteractionPressed++;
	OnInteractionMashKeyPressed.Broadcast();
}

bool UActorInteractableComponent::ActivateInteractable(FString& ErrorMessage)
{
	bool bResult = true;
	ErrorMessage = FString("");

	if(!GetOwner())
	{
		AIP_LOG(Error, TEXT("[ActivateInteractable] No Owner. Please report this bug."))
		ErrorMessage.Append(FString("Error: Invalid Owner;"));
		bResult = false;
	}

	if (!GetWorld())
	{
		AIP_LOG(Error, TEXT("[ActivateInteractable] No World. Please report this bug."))
		const FString NoWorldString = TEXT("Error: Invaid World;");
		if(ErrorMessage.Len() == 0) ErrorMessage = NoWorldString;
		else ErrorMessage.Append(FString("|")).Append(NoWorldString);
		bResult = false;
	}
	
	SetInteractionState(EInteractableState::EIS_Inactive);
	
	return bResult;
}

void UActorInteractableComponent::DeactivateInteractable()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CooldownTime);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InteractionTime);
	}
	else
	{
		AIP_LOG(Error, TEXT("[DeactivateInteractable] No World. Report this bug please."))
	}
	
	StopInteractionLink(GetInteractorComponent());
	
	SetInteractionState(EInteractableState::EIS_Disabled);
}

void UActorInteractableComponent::UpdateInteractableWidget() const
{
	if (InteractionWidget)
	{
		InteractionWidget->OnUpdateInteractionWidget();
	}
}

void UActorInteractableComponent::FinishInteraction_TimerFunction()
{
	FinishInteraction(GetWorld()->GetTimeSeconds());
}

void UActorInteractableComponent::CooldownElapsed_TimerFunction()
{
	if (GetInteractorComponent())
	{
		SetInteractionState(EInteractableState::EIS_Standby);

		SetHiddenInGame(false);
		
		SetRemainingInteractionProgress(1.f);
		UpdateInteractableWidget();
		SetMeshComponentsHighlight(bInteractionHighlight);

		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CooldownTime);
	}
	else
	{
		SetInteractionState(EInteractableState::EIS_Inactive);

		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CooldownTime);
	}
}

void UActorInteractableComponent::UpdateCollisionChannels() const
{
	if(CollisionShapes.Num())
	{
		for (auto& Itr : CollisionShapes)
		{
			UpdateCollisionChannel(Itr);
		}
	}
}

void UActorInteractableComponent::UpdateCollisionChannel(UShapeComponent* const CollisionShape) const
{
	if(!CollisionShape)
	{
		AIP_LOG(Error, TEXT("[UpdateCollisionChannel] Invalid Collision Shape, Cannot update Collision Channel"))
		return;
	}
	
	CollisionShape->SetCanEverAffectNavigation(false);
	CollisionShape->SetGenerateOverlapEvents(true);
	CollisionShape->SetCollisionProfileName(TEXT("Custom"));
	CollisionShape->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	CollisionShape->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionShape->SetCollisionResponseToChannel(InteractableCollisionChannel, ECollisionResponse::ECR_Block);
	CollisionShape->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

#if WITH_EDITOR
	
	CollisionShape->UpdateCollisionProfile();
	if(CollisionShape->GetBodyInstance()) CollisionShape->GetBodyInstance()->LoadProfileData(true);

#endif
	
	CollisionShape->OnComponentCollisionSettingsChangedEvent.Broadcast(CollisionShape);
}

void UActorInteractableComponent::UpdateLifecycleLogic()
{
	if (InteractionType == EInteractableType::EIT_Auto)
	{
		SetInteractionLifecycleType(EInteractableLifecycle::EIL_OnlyOnce);
	}
}

#pragma region Replication

void UActorInteractableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

#pragma endregion Replication

#if WITH_EDITOR

void UActorInteractableComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;
	
	if (PropertyName == TEXT("InteractableCollisionChannel"))
	{
		UpdateCollisionChannels();
	}
	
	// If any shape is changed refresh it all so no unwanted bindings stay there
	if (PropertyName == TEXT("CollisionShapes"))
	{
		const auto TempCollisionShapes = GetCollisionShapes();
		RemoveCollisionShapes(GetCollisionShapes());
		SetCollisionShapes(TempCollisionShapes);
	}
	
	const bool bTextChanged =
		PropertyName == TEXT("InteractionActorName") ||
		PropertyName == TEXT("InteractionActionName") ||
		PropertyName == TEXT("InteractionActionKey") ||
		PropertyName == TEXT("InteractionActionTexture");
	if (bTextChanged)
	{
		UpdateInteractableWidget();
	}

	if (PropertyName == TEXT("InteractableWidgetClass"))
	{
		WidgetClass = InteractableWidgetClass;
		SetInteractableWidgetClass(InteractableWidgetClass);
	}

	if (PropertyName == TEXT("InteractionType"))
	{
		UpdateLifecycleLogic();
	}
}

void UActorInteractableComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;

	// If any shape is changed refresh it all so no unwanted bindings stay there
	if (PropertyName == TEXT("CollisionShapes"))
	{
		const auto TempCollisionShapes = GetCollisionShapes();
		RemoveCollisionShapes(GetCollisionShapes());
		SetCollisionShapes(TempCollisionShapes);
	}
}

#endif
