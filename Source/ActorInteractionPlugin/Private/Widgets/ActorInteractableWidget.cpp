// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Widgets/ActorInteractableWidget.h"
#include "Components/ActorInteractableComponent.h"
#include "Components/TextBlock.h"

#include "Helpers/ActorInteractionPluginLog.h"

bool UActorInteractableWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return bSuccess;
	
	bSuccess =
		InteractableName != nullptr
		&& InteractionAction != nullptr
		&& InteractableKey != nullptr; 

	if (!bSuccess)
	{
		AIP_LOG(Error, TEXT("[Initialize] Initialize has returned false. Please report this bug."))
	}
	
	return bSuccess;
}

void UActorInteractableWidget::InitializeInteractionWidget(const FText& NewInteractableKey, const FText& NewInteractableName,
                                    const FText& NewInteractionAction, UActorInteractableComponent* NewOwningComponent)
{
	OwningComponent = NewOwningComponent;

	if (!OwningComponent)
	{
		AIP_LOG(Error, TEXT("[InitializeInteractionWidget] Interactable Widget has no owning Component. Please, report this bug."))
		return;
	}

	InteractableKey->SetText(NewInteractableKey);
	InteractableName->SetText(NewInteractableName);
	InteractionAction->SetText(NewInteractionAction);

	TimeRemainder = OwningComponent->GetLastInteractionTime();
	SetInteractionProgress(1.f);
}

void UActorInteractableWidget::CalculateRemainingTime(float InDeltaTime)
{
	if (!OwningComponent)
	{
		AIP_LOG(Error, TEXT("[CalculateRemainingTime] Interactable Widget has no owning Component. Please, report this bug."))
		return;
	}

	if (!OwningComponent->IsInUse()) return;

	const float InteractionTime = OwningComponent->GetInteractionTime();
	const float WorldTimeSeconds = GetWorld()->GetTimeSeconds();
	const float InteractionFinishTime = OwningComponent->GetLastInteractionTime() + InteractionTime;
	const float DeltaTime = InteractionFinishTime - WorldTimeSeconds;

	SetInteractionProgress(DeltaTime / InteractionTime);
}

void UActorInteractableWidget::SetInteractionProgress(const float NewInteractionProgress)
{
	InteractionProgress = FMath::Clamp(NewInteractionProgress, 0.f, 1.f);

	OnInteractionProgressChanged(InteractionProgress);
}

void UActorInteractableWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CalculateRemainingTime(InDeltaTime);
}