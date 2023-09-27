// All rights reserved Dominik Pavlicek 2022.


#include "Components/ActorInteractableComponentPress.h"

#include "Misc/DataValidation.h"

#if WITH_EDITOR
#include "EditorHelper.h"
#endif

#define LOCTEXT_NAMESPACE "InteractableComponentPress"

// Sets default values for this component's properties
UActorInteractableComponentPress::UActorInteractableComponentPress()
{
	bInteractionHighlight = true;
	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = -1.f;
	InteractableName = LOCTEXT("InteractableComponentPress", "Press");
}

void UActorInteractableComponentPress::BeginPlay()
{
	Super::BeginPlay();

	SetInteractionPeriod(-1.f);
	
	Timer_Interaction.Invalidate();
}

void UActorInteractableComponentPress::InteractionStarted(const float& TimeStarted, const FKey& PressedKey, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	Super::InteractionStarted(TimeStarted, PressedKey, CausingInteractor);
	
	if (CanInteract())
	{
		if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
		{
			if (TriggerCooldown()) return;
		}
		
		OnInteractionCompleted.Broadcast(TimeStarted, CausingInteractor);
	}
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
			interactableName.Append(TEXT(": Press Interaction requires specific Interaction Period."))
		);

		Context.AddWarning(ErrorMessage);
		bAnyError = true;
	}

	if (bAnyError)
	{
		ResetDefaultValues.ExecuteIfBound();
	}
	
	return bAnyError ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}

void UActorInteractableComponentPress::ResetDefaultValuesImpl()
{
	Super::ResetDefaultValuesImpl();

	InteractionPeriod = -1.f;
}

#endif

#undef LOCTEXT_NAMESPACE
