// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "K2Node_MounteaInteractionCallFunction.generated.h"

/**
 * Custom K2Node subclass for all Mountea Interaction static helper functions.
 *
 * Discovers functions in the MounteaInteractionSystem module that carry one of:
 *   meta=(MounteaGetter)   — pure read-only query      → blue header
 *   meta=(MounteaSetter)   — state mutation             → orange header
 *   meta=(MounteaValidate) — bool validation            → red header
 *   meta=(MounteaCommand)  — action trigger             → green header
 *   meta=(MounteaBinding)  — delegate bind/unbind       → purple header
 *
 * Node colors and icons are sourced from FMounteaInteractionDeveloperStyle.
 */
UCLASS()
class MOUNTEAINTERACTIONSYSTEMDEVELOPER_API UK2Node_MounteaInteractionCallFunction : public UK2Node_CallFunction
{
	GENERATED_BODY()

public:

	//~ Begin UEdGraphNode Interface
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetMenuCategory() const override;
	//~ End UEdGraphNode Interface

	//~ Begin UK2Node Interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	//~ End UK2Node Interface

private:

	/** Returns the meta tag key found on the bound function (MounteaGetter, MounteaSetter, etc.). Empty if none. */
	FName GetMounteaMetaKey() const;

	static const TArray<FName>& GetMounteaMetaKeys();
};
