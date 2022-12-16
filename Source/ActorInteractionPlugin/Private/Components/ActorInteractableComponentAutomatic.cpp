// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ActorInteractableComponentAutomatic.h"

#define LOCTEXT_NAMESPACE "ActorInteractableComponentAutomatic"

UActorInteractableComponentAutomatic::UActorInteractableComponentAutomatic()
{
	InteractableName = LOCTEXT("ActorInteractableComponentAutomatic", "Auto");
	InteractionKeysPerPlatform.Empty();

	DefaultInteractableState = EInteractableStateV2::EIS_Awake;
	InteractionPeriod = 1.f;

	bInteractionHighlight = false;
}

void UActorInteractableComponentAutomatic::BeginPlay()
{
	Super::BeginPlay();
}

void UActorInteractableComponentAutomatic::InteractableSelected(const TScriptInterface<IActorInteractableInterface>& Interactable)
{
	if(!GetWorld()) return;
	Super::InteractableSelected(Interactable);
	
	if (Interactable == this)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(Timer_Interaction) == false)
		{
			OnInteractionStarted.Broadcast(GetWorld()->GetTimeSeconds(), FKey(""));
		}
	}
}

void UActorInteractableComponentAutomatic::InteractionStarted(const float& TimeStarted, const FKey& PressedKey)
{
	Super::InteractionStarted(TimeStarted, PressedKey);
	
	if (CanInteract())
	{
		// Force Interaction Period to be at least 0.01s
		const float TempInteractionPeriod = FMath::Max(0.01f, InteractionPeriod);

		if (!GetWorld()) return;
	
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &UActorInteractableComponentAutomatic::OnInteractionCompletedCallback);

		GetWorld()->GetTimerManager().SetTimer
		(
			Timer_Interaction,
			Delegate,
			TempInteractionPeriod,
			false
		);

		UpdateInteractionWidget();
	}
}

void UActorInteractableComponentAutomatic::OnInteractionCompletedCallback()
{
	if (!GetWorld())
	{
		OnInteractionCanceled.Broadcast();
		return;
	}

	ToggleWidgetVisibility(false);
	if (LifecycleMode == EInteractableLifecycle::EIL_Cycled)
	{
		if (TriggerCooldown()) return;
	}
	
	OnInteractionCompleted.Broadcast(GetWorld()->GetTimeSeconds());
}

#undef LOCTEXT_NAMESPACE
