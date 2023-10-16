// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Widgets/ActorInteractableWidget.h"
#include "Components/ActorInteractableComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Helpers/ActorInteractionPluginLog.h"

bool UActorInteractableWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return bSuccess;
	
	bSuccess =
		InteractableName != nullptr
		&& InteractionAction != nullptr
		&& InteractableKey != nullptr
		&& InteractionTexture != nullptr; 

	if (!bSuccess)
	{
		AIP_LOG(Error, TEXT("[Initialize] Initialize has returned false. Please report this bug."))
	}
	
	return bSuccess;
}

void UActorInteractableWidget::InitializeInteractionWidget(const FText& NewInteractableKey, const FText& NewInteractableName, const FText& NewInteractionAction, UDEPRECATED_ActorInteractableComponent* NewOwningComponent, UTexture2D* NewInteractionTexture)
{
	AIP_LOG(Error, TEXT("[InitializeInteractionWidget] This function is Deprecated. Use `UpdateWidget` instead."))
}

void UActorInteractableWidget::ToggleVisibility_Implementation()
{
	ESlateVisibility NewVisibility = ESlateVisibility::Hidden;

	switch (CachedVisibility)
	{
		case ESlateVisibility::HitTestInvisible: break;
		case ESlateVisibility::SelfHitTestInvisible: break;
		case ESlateVisibility::Visible:
			NewVisibility = ESlateVisibility::Hidden;
			break;
		case ESlateVisibility::Collapsed:
		case ESlateVisibility::Hidden:
			NewVisibility = ESlateVisibility::Visible;
			break;
		default:
			NewVisibility = ESlateVisibility::Hidden;
			break;
	}
	
	CachedVisibility = GetVisibility();
	SetVisibility(NewVisibility);
}

void UActorInteractableWidget::CalculateRemainingTime(float InDeltaTime)
{
	AIP_LOG(Error, TEXT("[InitializeInteractionWidget] This function is Deprecated. Use `SetProgress` or `GetProgress` instead."))
}

void UActorInteractableWidget::SetInteractionProgress(const float NewInteractionProgress)
{
	AIP_LOG(Warning, TEXT("[InitializeInteractionWidget] This function is Deprecated. Use `SetProgress` instead."))
	
	InteractionProgress = FMath::Clamp(NewInteractionProgress, 0.f, 1.f);

	OnInteractionProgressChanged(InteractionProgress);
}
