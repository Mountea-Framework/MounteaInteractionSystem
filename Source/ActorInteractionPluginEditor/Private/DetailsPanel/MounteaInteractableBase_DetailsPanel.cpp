// All rights reserved Dominik Morse (Pavlicek) 2024

#include "MounteaInteractableBase_DetailsPanel.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Components/Interactable/ActorInteractableComponentBase.h"
#include "Widgets/Layout/SScaleBox.h"

#define LOCTEXT_NAMESPACE "InteractableComponentsPanel"

void MounteaInteractableBase_DetailsPanel::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

	// Only support one object being customized
	if (ObjectsBeingCustomized.Num() != 1) return;

	const TWeakObjectPtr<UActorInteractableComponentBase> weakComponent = Cast<UActorInteractableComponentBase>(ObjectsBeingCustomized[0].Get());
	if (!weakComponent.IsValid()) return;

	EditingComponent = weakComponent.Get();
	if (!EditingComponent) return;

	// Only edit if editing from Actor Editor
	/*if (DetailBuilder.GetBaseClass()->IsChildOf(AActor::StaticClass()) == false)
	{ return; };*/

	IDetailCategoryBuilder& ItrCategoryBuild = DetailBuilder.EditCategory(TEXT("MounteaInteraction"), FText::GetEmpty(), ECategoryPriority::Important);
	ItrCategoryBuild.AddCustomRow(LOCTEXT("InteractableComponentsPanel_Defaults", "Load Defaults"), false)
	.WholeRowWidget
	[
		SNew(SBox)
		.HAlign(HAlign_Fill)
		[
			SNew(SScaleBox)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			.Stretch(EStretch::ScaleToFit)
			[
				SAssignNew(DefaultsButton, SButton)
				.HAlign(HAlign_Fill)
				.Text(LOCTEXT("InteractableComponentsPanel_Defaults_Text", "Load Defaults"))
				.ToolTipText(LOCTEXT("InteractableComponentsPanel_Defaults_Tooltip", "Overrides data with default values from Project Settings.\nProject Settings must be defined!"))
				.OnClicked(this, &MounteaInteractableBase_DetailsPanel::OnDefaultsClicked)
				.OnHovered(this, &MounteaInteractableBase_DetailsPanel::OnDefaultsHovered)
				.OnUnhovered(this, &MounteaInteractableBase_DetailsPanel::OnDefaultsHovered)
				
			]
		]
	];
}

FReply MounteaInteractableBase_DetailsPanel::OnDefaultsClicked() const
{
	if (EditingComponent)
	{
		EditingComponent->SetDefaultValues();

		if (SavedLayoutBuilder) SavedLayoutBuilder->ForceRefreshDetails();

		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void MounteaInteractableBase_DetailsPanel::OnDefaultsHovered()
{
	
}

#undef LOCTEXT_NAMESPACE