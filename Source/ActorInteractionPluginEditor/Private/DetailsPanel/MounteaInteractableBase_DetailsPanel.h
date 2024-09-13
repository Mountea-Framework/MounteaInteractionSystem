// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "IDetailCustomization.h"

class UActorInteractableComponentBase;

class MounteaInteractableBase_DetailsPanel : public IDetailCustomization
{
	typedef MounteaInteractableBase_DetailsPanel Self;

public:
	// Makes a new instance of this detail layout class for a specific detail view requesting it
	static TSharedRef<IDetailCustomization> MakeInstance() { return MakeShared<Self>(); }
	
	// IDetailCustomization interface
	/** Called when details should be customized */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	FReply OnDefaultsClicked() const;
	void OnDefaultsHovered();

private:

	IDetailLayoutBuilder* SavedLayoutBuilder = nullptr;

	UActorInteractableComponentBase* EditingComponent = nullptr;

	TSharedPtr<SButton> DefaultsButton;
};
