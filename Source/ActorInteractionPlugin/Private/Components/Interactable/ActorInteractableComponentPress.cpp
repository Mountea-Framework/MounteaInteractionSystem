// All rights reserved Dominik Morse (Pavlicek) 2024.


#include "Components/Interactable/ActorInteractableComponentPress.h"

#if WITH_EDITOR

#include "EditorHelper.h"
#include "Misc/DataValidation.h"

#endif

#define LOCTEXT_NAMESPACE "InteractableComponentPress"

UActorInteractableComponentPress::UActorInteractableComponentPress()
{
	bInteractionHighlight = true;
	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = -1.f;
	InteractableName = NSLOCTEXT("InteractableComponentPress", "Press", "Press");
}

void UActorInteractableComponentPress::BeginPlay()
{
	Super::BeginPlay();

	Execute_SetInteractionPeriod(this, -1.f);
	
	Timer_Interaction.Invalidate();
}

void UActorInteractableComponentPress::InteractionStarted_Implementation(const float& TimeStarted, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	Super::InteractionStarted_Implementation(TimeStarted, CausingInteractor);
	
	if (Execute_CanInteract(this))
	{
		if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
		{
			if (Execute_TriggerCooldown(this)) return;
		}
		
		OnInteractionCompleted.Broadcast(TimeStarted, CausingInteractor);
	}
}

void UActorInteractableComponentPress::SetDefaults_Implementation()
{
	Super::SetDefaults_Implementation();

	InteractionPeriod = -1.f;
}

#if WITH_EDITOR

void UActorInteractableComponentPress::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
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
	
	if (PropertyName == TEXT("InteractionPeriod") && !(FMath::IsNearlyEqual(InteractionPeriod, -1.f)))
	{
		InteractionPeriod = -1.f;
		
		if (DebugSettings.EditorDebugMode)
		{
			const FText ErrorMessage = FText::FromString
			(
				interactableName.Append(TEXT(": Interaction period for Press component cannot be changed!"))
			);
				
			FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
		}
	}
}

EDataValidationResult UActorInteractableComponentPress::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);

	bool bAnyError = SuperResult == EDataValidationResult::Invalid;
	
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

	if (!FMath::IsNearlyEqual(InteractionPeriod, -1.f))
	{
		const FText ErrorMessage = FText::FromString
		(
			interactableName.Append(TEXT(": InteractionPeriod must be -1.f!"))
		);

		Context.AddError(ErrorMessage);
		bAnyError = true;
	}

	if (bAnyError && RequestEditorDefaults.IsBound())
	{
		RequestEditorDefaults.Broadcast();
	}

	return bAnyError ? EDataValidationResult::Invalid : SuperResult;
}

#endif

#undef LOCTEXT_NAMESPACE
