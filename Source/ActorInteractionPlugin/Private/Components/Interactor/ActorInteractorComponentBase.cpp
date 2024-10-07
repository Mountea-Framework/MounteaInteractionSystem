// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "Components/Interactor/ActorInteractorComponentBase.h"

#if WITH_EDITOR

#include "EditorHelper.h"
#include "Misc/DataValidation.h"

#endif

#include "Helpers/ActorInteractionFunctionLibrary.h"
#include "Helpers/InteractionHelpers.h"
#include "Helpers/MounteaInteractionSystemBFL.h"
#include "Helpers/ActorInteractionPluginLog.h"

#include "Interfaces/ActorInteractableInterface.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

UActorInteractorComponentBase::UActorInteractorComponentBase() :
		DebugSettings(false),
		CollisionChannel(ECC_Camera),
		DefaultInteractorState(EInteractorStateV2::EIS_Awake),
		SafetyTraceSetup(FSafetyTracingSetup(ESafetyTracingMode::ESTM_Location)),
		InteractorState(EInteractorStateV2::EIS_Asleep)
{
	bAutoActivate = true;
	
	SetIsReplicatedByDefault(true);
	SetActiveFlag(true);

	PrimaryComponentTick.bStartWithTickEnabled = false;

	ComponentTags.Add(FName("Mountea"));
	ComponentTags.Add(FName("Interaction"));

#if WITH_EDITOR || WITH_EDITORONLY_DATA
	if (GIsEditor && !GIsPlayInEditorWorld)
	{
		RequestEditorDefaults.AddUObject(this, &UActorInteractorComponentBase::ResetDefaults);
	}
#endif
}

void UActorInteractorComponentBase::BeginPlay()
{
	Super::BeginPlay();
	
	OnInteractableUpdated.			AddUniqueDynamic(this, &UActorInteractorComponentBase::InteractableSelected);
	OnInteractableFound.				AddUniqueDynamic(this, &UActorInteractorComponentBase::InteractableFound);
	OnInteractableLost.					AddUniqueDynamic(this, &UActorInteractorComponentBase::InteractableLost);
	
	OnInteractionKeyPressed.		AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractionKeyPressedEvent);
	OnInteractionKeyReleased.	AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractionKeyReleasedEvent);
	
	OnStateChanged.					AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractorStateChanged);
	OnCollisionChanged.				AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractorCollisionChanged);
	OnComponentActivated.			AddUniqueDynamic(this, &UActorInteractorComponentBase::OnInteractorComponentActivated);

	if (GetOwner() &&( bAutoActivate || IsActive()))
	{
		Execute_AddIgnoredActor(this, GetOwner());

		Execute_SetState(this, DefaultInteractorState);
	}	
}

FString UActorInteractorComponentBase::ToString_Implementation() const
{
	TScriptInterface<IActorInteractableInterface> activeInteractable = Execute_GetActiveInteractable(this);
	FText activeInteractableName = activeInteractable.GetObject() ? FText::FromString(activeInteractable->Execute_GetInteractableName(activeInteractable.GetObject()).ToString()) : FText::FromString("None");

	FText interactorStateText = FText::FromString(UEnum::GetValueAsString(Execute_GetState(this)));

	FText collisionChannelText = FText::FromString(UEnum::GetValueAsString(Execute_GetResponseChannel(this)));

	FText interactorTagText = InteractorTag.IsValid() ? FText::FromString(InteractorTag.ToString()) : FText::FromString("None");

	return FText::Format(
		NSLOCTEXT("InteractorDebugData", "Format", "Active Interactable: {0}\nInteractor State: {1}\nCollision Channel: {2}\nInteractor Tag: {3}"),
		activeInteractableName, interactorStateText, collisionChannelText, interactorTagText
	).ToString();
}

AActor* UActorInteractorComponentBase::GetOwningActor_Implementation() const
{
	return GetOwner();
}

FSafetyTracingSetup UActorInteractorComponentBase::GetSafetyTracingSetup_Implementation() const
{
	return SafetyTraceSetup;
}

void UActorInteractorComponentBase::SetSafetyTracingSetup_Implementation(const FSafetyTracingSetup& NewSafetyTracingSetup)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetSafetyTracingSetup] No owner!"))
		return;
	}

	if (NewSafetyTracingSetup == SafetyTraceSetup)
		return;
		
	if (GetOwner()->HasAuthority())
	{
		SafetyTraceSetup = NewSafetyTracingSetup;
	}
	else
	{
		SetSafetyTracingSetup_Server(NewSafetyTracingSetup);
	}
}

bool UActorInteractorComponentBase::PerformSafetyTrace_Implementation(const AActor* InteractableActor)
{
	if (!InteractableActor)
		return false;

	if (!GetOwner())
		return false;
	
	FHitResult safetyTrace;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());

	FVector traceStartLocation = GetOwner()->GetActorLocation();

	switch (SafetyTraceSetup.SafetyTracingMode)
	{
		case ESafetyTracingMode::ESTM_Location:
			traceStartLocation = SafetyTraceSetup.StartLocation;
			break;
		case ESafetyTracingMode::ESTM_Socket:
			{
				if (const auto ownerMesh =  UMounteaInteractionSystemBFL::FindMeshByName(SafetyTraceSetup.StartSocketName, GetOwner()))
				{
					traceStartLocation =
						ownerMesh->DoesSocketExist(SafetyTraceSetup.StartSocketName) ?
						ownerMesh->GetSocketLocation(SafetyTraceSetup.StartSocketName) :
						traceStartLocation;
				}
			}
			break;
		case ESafetyTracingMode::Default:
		case ESafetyTracingMode::ESTM_None:
			return true;
	}

	bool bHit = GetWorld()->LineTraceSingleByChannel(safetyTrace, traceStartLocation, InteractableActor->GetActorLocation(), SafetyTraceSetup.ValidationCollisionChannel, queryParams);

#if WITH_EDITOR || UE_BUILD_DEBUG
	if (DebugSettings.DebugMode)
	{
		DrawDebugBox(GetWorld(), traceStartLocation, FVector(5.f), FColor::Blue, false, 2.f, 0, 1.f);
		DrawDebugBox(GetWorld(), InteractableActor->GetActorLocation(), FVector(5.f), FColor::Red, false, 2.f, 0, 1.f);
		DrawDebugDirectionalArrow(GetWorld(), traceStartLocation, InteractableActor->GetActorLocation(), 2.f, FColor::Purple, false, 2.f, 0, 1.f);
	}
#endif

	return bHit && safetyTrace.GetActor() == InteractableActor;
}

void UActorInteractorComponentBase::SetDefaults_Implementation()
{
	const auto defaultValues = UActorInteractionFunctionLibrary::GetDefaultInteractorSettings();
	{
		CollisionChannel			= defaultValues.InteractorCollisionChannel;
		InteractorTag				= defaultValues.InteractorTag;
		DefaultInteractorState = defaultValues.DefaultInteractorState;
	}
}

void UActorInteractorComponentBase::ConsumeInput_Implementation(UInputAction* ConsumedInput)
{
	OnInputActionConsumed.Broadcast(ConsumedInput);
}

void UActorInteractorComponentBase::InteractableSelected_Implementation(const TScriptInterface<IActorInteractableInterface>& SelectedInteractable)
{
	if (SelectedInteractable.GetObject())
	{
		SelectedInteractable->GetOnInteractableSelectedHandle().Broadcast(SelectedInteractable);
	}
	
	Execute_OnInteractableSelectedEvent(this, SelectedInteractable);
}

void UActorInteractorComponentBase::InteractableFound_Implementation(const TScriptInterface<IActorInteractableInterface>& FoundInteractable)
{
	if (FoundInteractable.GetInterface() == nullptr) return;

	if (FoundInteractable != ActiveInteractable)
	{
		for (const auto& Itr : InteractionDependencies)
		{
			if (Itr.GetInterface())
			{
				Itr->Execute_SetState(this, EInteractorStateV2::EIS_Suppressed);
			}
		}
	
		Execute_EvaluateInteractable(this, FoundInteractable);

		Execute_OnInteractableFoundEvent(this, FoundInteractable);
	}
}

void UActorInteractorComponentBase::InteractableLost_Implementation(const TScriptInterface<IActorInteractableInterface>& LostInteractable)
{
	if (LostInteractable.GetInterface() == nullptr)
		return;

	if (!ActiveInteractable.GetObject() || !ActiveInteractable.GetInterface())
		return;
	
	if (LostInteractable == ActiveInteractable)
	{
		ActiveInteractable->GetOnInteractorLostHandle().Broadcast(this);
		
		Execute_SetState(this, EInteractorStateV2::EIS_Awake);
		
		Execute_SetActiveInteractable(this, nullptr);
		
		Execute_OnInteractableLostEvent(this, LostInteractable);
	}
}

bool UActorInteractorComponentBase::IsValidInteractor_Implementation() const
{
	switch (InteractorState)
	{
		case EInteractorStateV2::EIS_Awake:
		case EInteractorStateV2::EIS_Asleep:
		case EInteractorStateV2::EIS_Active:
			return true;
		case EInteractorStateV2::EIS_Suppressed:
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::Default:
		default:
			return false;
	}
}

void UActorInteractorComponentBase::EvaluateInteractable_Implementation(const TScriptInterface<IActorInteractableInterface>& FoundInteractable)
{
	if (!Execute_IsValidInteractor(this)) return;
	
	if (FoundInteractable.GetInterface() == nullptr)
	{
		OnInteractableLost.Broadcast(ActiveInteractable);
		return;
	}
	
	if (ActiveInteractable.GetInterface() == nullptr)
	{
		Execute_SetActiveInteractable(this, FoundInteractable);
		OnInteractableUpdated.Broadcast(FoundInteractable);

		return;
	}

	/***
	 * TODO ?
	 * Even when not tracing again active, active stays
	 */
	if (ActiveInteractable != FoundInteractable)
	{
		if (FoundInteractable->Execute_GetInteractableWeight(FoundInteractable.GetObject()) > ActiveInteractable->Execute_GetInteractableWeight(FoundInteractable.GetObject()))
		{
			if (ActiveInteractable.GetInterface() != nullptr)
			{
				OnInteractableLost.Broadcast(ActiveInteractable);
			}
			
			
			Execute_SetActiveInteractable(this, FoundInteractable);
			OnInteractableUpdated.Broadcast(FoundInteractable);
		}
		else
		{
			if (FoundInteractable.GetInterface() != nullptr)
			{
				OnInteractableLost.Broadcast(FoundInteractable);
			}

			OnInteractableUpdated.Broadcast(ActiveInteractable);
		}
	}
	else
	{
		Execute_SetActiveInteractable(this, FoundInteractable);
		OnInteractableUpdated.Broadcast(FoundInteractable);
	}
}

void UActorInteractorComponentBase::StartInteraction_Implementation(const float StartTime)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[StartInteraction] No Owner!"))
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (Execute_CanInteract(this) && ActiveInteractable.GetInterface())
		{
			Execute_SetState(this,EInteractorStateV2::EIS_Active);
			ActiveInteractable->GetOnInteractionStartedHandle().Broadcast(StartTime, this);
		}
	}
	else
	{
		StartInteraction_Server(StartTime);
	}
}

void UActorInteractorComponentBase::StopInteraction_Implementation(const float StopTime)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[StopInteraction] No Owner!"))
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (Execute_CanInteract(this) && ActiveInteractable.GetInterface())
		{
			Execute_SetState(this,DefaultInteractorState);
			ActiveInteractable->GetOnInteractionStoppedHandle().Broadcast(StopTime, this);
		}
	}
	else
	{
		StopInteraction_Server(StopTime);
	}
}

bool UActorInteractorComponentBase::ActivateInteractor_Implementation(FString& ErrorMessage)
{
	const EInteractorStateV2 CachedState = InteractorState;
	
	Execute_SetState(this, EInteractorStateV2::EIS_Active);
	
	switch (CachedState)
	{
		case EInteractorStateV2::EIS_Awake:
			ErrorMessage.Append(TEXT("Interactor Component has been Activated"));
			return true;
		case EInteractorStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactor Component is already Active"));
			break;
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::EIS_Suppressed:
		case EInteractorStateV2::EIS_Asleep:
			ErrorMessage.Append(TEXT("Interactor Component cannot be Activated"));
			break;
		case EInteractorStateV2::Default:
		default:
			ErrorMessage.Append(TEXT("Interactor Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UActorInteractorComponentBase::EnableInteractor_Implementation(FString& ErrorMessage)
{
	const EInteractorStateV2 CachedState = InteractorState;
	
	Execute_SetState(this, EInteractorStateV2::EIS_Awake);
	
	switch (CachedState)
	{
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::EIS_Asleep:
		case EInteractorStateV2::EIS_Suppressed:
		case EInteractorStateV2::EIS_Awake:
			ErrorMessage.Append(TEXT("Interactor Component has been Awaken"));
			return true;
		case EInteractorStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactor Component is Awaken and no longer Active"));
			return true;
		case EInteractorStateV2::Default:
		default:
			ErrorMessage.Append(TEXT("Interactor Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

bool UActorInteractorComponentBase::SuppressInteractor_Implementation(FString& ErrorMessage)
{
	const EInteractorStateV2 CachedState = InteractorState;
	
	Execute_SetState(this, EInteractorStateV2::EIS_Suppressed);

	switch (CachedState)
	{
		case EInteractorStateV2::EIS_Asleep:
			ErrorMessage.Append(TEXT("Interactor Component is Asleep, cannot be Suppressed"));
			break;
		case EInteractorStateV2::EIS_Disabled:
			ErrorMessage.Append(TEXT("Interactor Component is Inactive, cannot be Suppressed"));
			break;
		case EInteractorStateV2::EIS_Suppressed:
			ErrorMessage.Append(TEXT("Interactor Component is already Suppressed"));
			break;
		case EInteractorStateV2::EIS_Awake:
		case EInteractorStateV2::EIS_Active:
			ErrorMessage.Append(TEXT("Interactor Component has been Suppressed"));
			return true;
		case EInteractorStateV2::Default:
		default:
			ErrorMessage.Append(TEXT("Interactor Component cannot proces activation request, invalid state"));
			break;
	}
	
	return false;
}

void UActorInteractorComponentBase::DeactivateInteractor_Implementation()
{	Execute_SetState(this, EInteractorStateV2::EIS_Disabled); }

void UActorInteractorComponentBase::AddIgnoredActor_Implementation(AActor* IgnoredActor)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[AddIgnoredActor] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (IgnoredActor == nullptr) return;
		if (ListOfIgnoredActors.Contains(IgnoredActor)) return;

		ListOfIgnoredActors.Add(IgnoredActor);

		OnIgnoredActorAdded.Broadcast(IgnoredActor);

		MARK_PROPERTY_DIRTY_FROM_NAME(UActorInteractorComponentBase, ListOfIgnoredActors, this);
	}
	else
	{
		AddIgnoredActor_Server(IgnoredActor);
	}
}

void UActorInteractorComponentBase::AddIgnoredActors_Implementation(const TArray<AActor*>& IgnoredActors)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[AddIgnoredActors] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		bool bListModified = false;
		for (const auto& Itr : IgnoredActors)
		{
			if (Itr == nullptr) continue;
			if (ListOfIgnoredActors.Contains(Itr)) continue;	

			ListOfIgnoredActors.Add(Itr);
			OnIgnoredActorAdded.Broadcast(Itr);
			bListModified = true;
		}

		if (bListModified)
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(UActorInteractorComponentBase, ListOfIgnoredActors, this);
		}
	}
	else
	{
		AddIgnoredActors_Server(IgnoredActors);
	}
}

void UActorInteractorComponentBase::RemoveIgnoredActor_Implementation(AActor* UnignoredActor)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[RemoveIgnoredActor] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (UnignoredActor == nullptr) return;
		if (ListOfIgnoredActors.Contains(UnignoredActor))
		{
			ListOfIgnoredActors.Remove(UnignoredActor);
			OnIgnoredActorRemoved.Broadcast(UnignoredActor);

			MARK_PROPERTY_DIRTY_FROM_NAME(UActorInteractorComponentBase, ListOfIgnoredActors, this);
		}
	}
	else
	{
		RemoveIgnoredActor_Server(UnignoredActor);
	}
}

void UActorInteractorComponentBase::RemoveIgnoredActors_Implementation(const TArray<AActor*>& UnignoredActors)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[RemoveIgnoredActors] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		bool bListModified = false;
		for (const auto& Itr : UnignoredActors)
		{
			if (Itr == nullptr) continue;
			if (ListOfIgnoredActors.Contains(Itr))
			{
				ListOfIgnoredActors.Remove(Itr);
				OnIgnoredActorRemoved.Broadcast(Itr);
				bListModified = true;
			}
		}

		if (bListModified)
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(UActorInteractorComponentBase, ListOfIgnoredActors, this);
		}
	}
	else
	{
		RemoveIgnoredActors_Server(UnignoredActors);
	}
}

TArray<AActor*> UActorInteractorComponentBase::GetIgnoredActors_Implementation() const
{ return ListOfIgnoredActors; }

void UActorInteractorComponentBase::AddInteractionDependency_Implementation(const TScriptInterface<IActorInteractorInterface>& InteractionDependency)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[AddInteractionDependency] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (InteractionDependency.GetInterface() == nullptr) return;
		if (InteractionDependencies.Contains(InteractionDependency))
		{
			return;
		}

		InteractionDependencies.Add(InteractionDependency);
		Execute_ProcessDependencies(this);

		MARK_PROPERTY_DIRTY_FROM_NAME(UActorInteractorComponentBase, InteractionDependencies, this);
	}
	else
	{
		AddInteractionDependency_Server(InteractionDependency);
	}
}

void UActorInteractorComponentBase::RemoveInteractionDependency_Implementation(const TScriptInterface<IActorInteractorInterface>& InteractionDependency)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[RemoveInteractionDependency] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (InteractionDependency.GetInterface() == nullptr) return;
		if (InteractionDependencies.Contains(InteractionDependency))
		{
			InteractionDependency->Execute_SetState(this, InteractionDependency->Execute_GetDefaultState(this));
			InteractionDependencies.Remove(InteractionDependency);

			MARK_PROPERTY_DIRTY_FROM_NAME(UActorInteractorComponentBase, InteractionDependencies, this);
		}
	}
	else
	{
		RemoveInteractionDependency_Server(InteractionDependency);
	}
}

TArray<TScriptInterface<IActorInteractorInterface>> UActorInteractorComponentBase::GetInteractionDependencies_Implementation() const
{	return InteractionDependencies;}

void UActorInteractorComponentBase::ProcessDependencies_Implementation()
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[ProcessDependencies] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (InteractionDependencies.Num() == 0) return;
	
		for (const auto& Itr : InteractionDependencies)
		{
			switch (InteractorState)
			{
			case EInteractorStateV2::EIS_Active:
			case EInteractorStateV2::EIS_Suppressed:
			case EInteractorStateV2::EIS_Asleep:
				Itr->Execute_SetState(this, EInteractorStateV2::EIS_Suppressed);
				break;
			case EInteractorStateV2::EIS_Awake:
				Itr->Execute_SetState(this, Itr->Execute_GetDefaultState(this));
				break;
			case EInteractorStateV2::EIS_Disabled:
				Itr->Execute_SetState(this, Itr->Execute_GetDefaultState(this));
				RemoveInteractionDependency(Itr);
				break;
			case EInteractorStateV2::Default:
			default:
				break;
			}
		}
	}
	else
	{
		ProcessDependencies_Server();
	}
}

bool UActorInteractorComponentBase::CanInteract_Implementation() const
{
	switch (InteractorState)
	{
		case EInteractorStateV2::EIS_Active:
		case EInteractorStateV2::EIS_Awake:
			return true;
		case EInteractorStateV2::EIS_Asleep:
		case EInteractorStateV2::EIS_Suppressed:
		case EInteractorStateV2::EIS_Disabled:
		case EInteractorStateV2::Default:
		default:
			return false;
	}
}

ECollisionChannel UActorInteractorComponentBase::GetResponseChannel_Implementation() const
{ return CollisionChannel; }

void UActorInteractorComponentBase::SetResponseChannel_Implementation(const ECollisionChannel NewResponseChannel)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetResponseChannel] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		CollisionChannel = NewResponseChannel;

		OnCollisionChanged.Broadcast(NewResponseChannel);
	}
	else
	{
		SetResponseChannel_Server(NewResponseChannel);
	}
}

EInteractorStateV2 UActorInteractorComponentBase::GetState_Implementation() const
{	return InteractorState; }

void UActorInteractorComponentBase::SetState_Implementation(const EInteractorStateV2 NewState)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetState] Interactor has no Owner!"))
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		switch (NewState)
		{
			case EInteractorStateV2::EIS_Awake:
				switch (InteractorState)
				{
					case EInteractorStateV2::EIS_Asleep:
					case EInteractorStateV2::EIS_Disabled:
					case EInteractorStateV2::EIS_Suppressed:
					case EInteractorStateV2::EIS_Active:
						InteractorState = NewState;
						ProcessStateChanged();
						break;
					case EInteractorStateV2::EIS_Awake:
					case EInteractorStateV2::Default:
					default: break;
				}
				break;
			case EInteractorStateV2::EIS_Asleep:
				switch (InteractorState)
				{
					case EInteractorStateV2::EIS_Awake:
					case EInteractorStateV2::EIS_Suppressed:
					case EInteractorStateV2::EIS_Active:
					case EInteractorStateV2::EIS_Disabled:
						InteractorState = NewState;
						ProcessStateChanged();
						break;
					case EInteractorStateV2::EIS_Asleep:
					case EInteractorStateV2::Default:
					default: break;
				}
				break;
			case EInteractorStateV2::EIS_Suppressed:
				switch (InteractorState)
				{
					case EInteractorStateV2::EIS_Awake:
					case EInteractorStateV2::EIS_Asleep:
					case EInteractorStateV2::EIS_Active:
						InteractorState = NewState;
						ProcessStateChanged();
						break;
					case EInteractorStateV2::EIS_Suppressed:
					case EInteractorStateV2::EIS_Disabled:
					case EInteractorStateV2::Default:
					default: break;
				}
				break;
			case EInteractorStateV2::EIS_Active:
				switch (InteractorState)
				{
					case EInteractorStateV2::EIS_Awake:
						InteractorState = NewState;
						ProcessStateChanged();
						break;
					case EInteractorStateV2::EIS_Asleep:
					case EInteractorStateV2::EIS_Active:
					case EInteractorStateV2::EIS_Suppressed:
					case EInteractorStateV2::EIS_Disabled:
					case EInteractorStateV2::Default:
					default: break;
				}
				break;
			case EInteractorStateV2::EIS_Disabled:
				switch (InteractorState)
				{
					case EInteractorStateV2::EIS_Asleep:
					case EInteractorStateV2::EIS_Awake:
					case EInteractorStateV2::EIS_Suppressed:
					case EInteractorStateV2::EIS_Active:
						InteractorState = NewState;
						ProcessStateChanged();
						break;
					case EInteractorStateV2::EIS_Disabled:
					case EInteractorStateV2::Default:
					default: break;
				}
				break;
			case EInteractorStateV2::Default:
			default: break;
		}

		Execute_ProcessDependencies(this);
	}
	else
	{
		SetState_Server(NewState);
	}
}

EInteractorStateV2 UActorInteractorComponentBase::GetDefaultState_Implementation() const
{ return DefaultInteractorState; }

void UActorInteractorComponentBase::SetDefaultState_Implementation(const EInteractorStateV2 NewState)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetDefaultState] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if
		(
			NewState == EInteractorStateV2::EIS_Active  ||
			NewState == EInteractorStateV2::Default
		)
		{
			LOG_ERROR(TEXT("[SetDefaultState] Tried to set invalid Default State!"));
			return;
		}

		DefaultInteractorState = NewState;
	}
	else
	{
		SetDefaultState_Server(NewState);
	}
}

bool UActorInteractorComponentBase::DoesAutoActivate_Implementation() const
{	return DefaultInteractorState == EInteractorStateV2::EIS_Awake ? true : false;}

void UActorInteractorComponentBase::SetActiveInteractable_Implementation(const TScriptInterface<IActorInteractableInterface>& NewInteractable)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetActiveInteractable] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (NewInteractable.GetInterface() == nullptr && ActiveInteractable.GetInterface() == nullptr)
		{
			return;
		}

		if (NewInteractable.GetInterface() == nullptr && ActiveInteractable.GetInterface() != nullptr)
		{
			ActiveInteractable = NewInteractable;
		}

		if (NewInteractable.GetInterface() != nullptr && ActiveInteractable.GetInterface() == nullptr)
		{
			ActiveInteractable = NewInteractable;

			OnInteractableUpdated.Broadcast(ActiveInteractable);
		}

		SetActiveInteractable_Client(ActiveInteractable);
	}
	else
	{
		SetActiveInteractable_Server(NewInteractable);
	}
}

TScriptInterface<IActorInteractableInterface> UActorInteractorComponentBase::GetActiveInteractable_Implementation() const
{	return ActiveInteractable; }

void UActorInteractorComponentBase::ToggleDebug_Implementation()
{
	DebugSettings.DebugMode = !DebugSettings.DebugMode;
}

FDebugSettings UActorInteractorComponentBase::GetDebugSettings_Implementation() const
{
	return DebugSettings;
}

FGameplayTag UActorInteractorComponentBase::GetInteractorTag_Implementation() const
{
	return InteractorTag;
}

void UActorInteractorComponentBase::SetInteractorTag_Implementation(const FGameplayTag& NewInteractorTag)
{
	if (!GetOwner())
	{
		LOG_ERROR(TEXT("[SetInteractorTag] No owner!"));
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		if (InteractorTag != NewInteractorTag)
		{
			InteractorTag = NewInteractorTag;

			OnInteractorTagChanged.Broadcast(NewInteractorTag);
		}
	}
	else
	{
		SetInteractorTag_Server(NewInteractorTag);
	}
}

void UActorInteractorComponentBase::OnInteractorComponentActivated_Implementation(UActorComponent* Component, bool bReset)
{
	Execute_AddIgnoredActor(this, GetOwner());

	Execute_SetState(this, DefaultInteractorState);
}

void UActorInteractorComponentBase::AddIgnoredActors_Server_Implementation(const TArray<AActor*>& IgnoredActors)
{
	Execute_AddIgnoredActors(this, IgnoredActors);
}

void UActorInteractorComponentBase::AddIgnoredActor_Server_Implementation(AActor* IgnoredActor)
{
	Execute_AddIgnoredActor(this, IgnoredActor);
}

void UActorInteractorComponentBase::AddInteractionDependency_Server_Implementation(const TScriptInterface<IActorInteractorInterface>& InteractionDependency)
{
	Execute_AddInteractionDependency(this, InteractionDependency);
}

void UActorInteractorComponentBase::RemoveInteractionDependency_Server_Implementation(const TScriptInterface<IActorInteractorInterface>& InteractionDependency)
{
	Execute_RemoveInteractionDependency(this, InteractionDependency);
}

void UActorInteractorComponentBase::RemoveIgnoredActor_Server_Implementation(AActor* IgnoredActor)
{
	Execute_RemoveIgnoredActor(this, IgnoredActor);
}

void UActorInteractorComponentBase::RemoveIgnoredActors_Server_Implementation(const TArray<AActor*>& IgnoredActors)
{
	Execute_RemoveIgnoredActors(this, IgnoredActors);
}

void UActorInteractorComponentBase::ProcessDependencies_Server_Implementation()
{
	Execute_ProcessDependencies(this);
}

void UActorInteractorComponentBase::SetResponseChannel_Server_Implementation(const ECollisionChannel NewResponseCollision)
{
	Execute_SetResponseChannel(this, NewResponseCollision);
}

void UActorInteractorComponentBase::SetDefaultState_Server_Implementation(const EInteractorStateV2 NewState)
{
	Execute_SetDefaultState(this, NewState);
}

void UActorInteractorComponentBase::SetActiveInteractable_Server_Implementation(const TScriptInterface<IActorInteractableInterface>& NewInteractable)
{
	Execute_SetActiveInteractable(this, NewInteractable);
}

void UActorInteractorComponentBase::SetInteractorTag_Server_Implementation(const FGameplayTag& NewInteractorTag)
{
	Execute_SetInteractorTag(this, NewInteractorTag);
}

void UActorInteractorComponentBase::SetActiveInteractable_Client_Implementation(const TScriptInterface<IActorInteractableInterface>& NewInteractable)
{
	if (NewInteractable.GetObject() != nullptr)
	{
		OnInteractableUpdated.Broadcast(NewInteractable);
	}
	else
		OnInteractableLost.Broadcast(NewInteractable);
}

void UActorInteractorComponentBase::SetSafetyTracingSetup_Server_Implementation(const FSafetyTracingSetup& NewSafetyTracingSetup)
{
	Execute_SetSafetyTracingSetup(this, NewSafetyTracingSetup);
}

void UActorInteractorComponentBase::OnRep_InteractorState()
{
	ProcessStateChanged_Client();
}

void UActorInteractorComponentBase::ProcessInteractableChanged()
{
	if (ActiveInteractable.GetObject() != nullptr)
	{
		OnInteractableUpdated.Broadcast(ActiveInteractable);
	}
	else
		OnInteractableLost.Broadcast(ActiveInteractable);
}

void UActorInteractorComponentBase::OnRep_ActiveInteractable()
{
	// ...
}

void UActorInteractorComponentBase::ProcessStateChanged()
{
	// Client side call
	OnStateChanged.Broadcast(InteractorState);
}

void UActorInteractorComponentBase::ProcessStateChanged_Client()
{
	OnStateChanged_Client.Broadcast(InteractorState);
	
	ProcessStateChanged();
}

void UActorInteractorComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UActorInteractorComponentBase, InteractorTag,						COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentBase, CollisionChannel,					COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentBase, DefaultInteractorState,			COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentBase, ListOfIgnoredActors,				COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentBase, SafetyTraceSetup,				COND_OwnerOnly);
	
	DOREPLIFETIME_CONDITION(UActorInteractorComponentBase, InteractorState,						COND_None);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentBase, ActiveInteractable,				COND_None);
	DOREPLIFETIME_CONDITION(UActorInteractorComponentBase, InteractionDependencies,		COND_None);
}

bool UActorInteractorComponentBase::HasInteractable_Implementation() const
{
	return ActiveInteractable.GetObject() != nullptr && ActiveInteractable.GetInterface() != nullptr;
}

void UActorInteractorComponentBase::StopInteraction_Server_Implementation(const float StopTime)
{
	Execute_StopInteraction(this, StopTime);
}

void UActorInteractorComponentBase::StartInteraction_Server_Implementation(const float StartTime)
{
	Execute_StartInteraction(this, StartTime);
}

void UActorInteractorComponentBase::SetState_Server_Implementation(const EInteractorStateV2 NewState)
{
	Execute_SetState(this, NewState);
}

#if WITH_EDITOR

void UActorInteractorComponentBase::ResetDefaults()
{
	Execute_SetDefaults(this);
}

void UActorInteractorComponentBase::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	
	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;
	
	FString InteractorName = GetName();
	// Format Name
	{
		if (InteractorName.Contains(TEXT("_GEN_VARIABLE")))
		{
			InteractorName.ReplaceInline(TEXT("_GEN_VARIABLE"), TEXT(""));
		}
		if(InteractorName.EndsWith(TEXT("_C")) && InteractorName.StartsWith(TEXT("Default__")))
		{
		
			InteractorName.RightChopInline(9);
			InteractorName.LeftChopInline(2);
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UActorInteractorComponentBase, DefaultInteractorState))
	{
		if
		(
			DefaultInteractorState == EInteractorStateV2::EIS_Active  ||
			DefaultInteractorState == EInteractorStateV2::Default
		)
		{
			const FText ErrorMessage = FText::FromString
			(
				InteractorName.Append(TEXT(": DefaultInteractorState cannot be")).Append(GetEnumValueAsString("EInteractorStateV2", DefaultInteractorState)).Append(TEXT("!"))
			);
			FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));

			DefaultInteractorState = EInteractorStateV2::EIS_Awake;
		}
	}
}

EDataValidationResult UActorInteractorComponentBase::IsDataValid(FDataValidationContext& Context) const
{
	const auto DefaultValue = Super::IsDataValid(Context);
	bool bAnyError = false;

	FString InteractorName = GetName();
	// Format Name
	{
		if (InteractorName.Contains(TEXT("_GEN_VARIABLE")))
		{
			InteractorName.ReplaceInline(TEXT("_GEN_VARIABLE"), TEXT(""));
		}
		if(InteractorName.EndsWith(TEXT("_C")) && InteractorName.StartsWith(TEXT("Default__")))
		{
		
			InteractorName.RightChopInline(9);
			InteractorName.LeftChopInline(2);
		}
	}

	if
	(
		DefaultInteractorState == EInteractorStateV2::EIS_Active  ||
		DefaultInteractorState == EInteractorStateV2::Default
	)
	{
		const FText ErrorMessage = FText::FromString
		(
			InteractorName.Append(TEXT(": DefaultInteractorState cannot be ")).Append(GetEnumValueAsString("EInteractorStateV2", DefaultInteractorState)).Append(TEXT("!"))
		);
		
		Context.AddError(ErrorMessage);
		bAnyError = true;
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

#if WITH_EDITOR || WITH_EDITORONLY_DATA

void UActorInteractorComponentBase::SetDefaultValues()
{
	Execute_SetDefaults(this);
}

#endif
