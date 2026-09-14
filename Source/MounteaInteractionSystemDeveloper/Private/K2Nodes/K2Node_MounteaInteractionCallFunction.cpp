// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "K2Nodes/K2Node_MounteaInteractionCallFunction.h"
#include "Styling/MounteaInteractionDeveloperStyle.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "UObject/UObjectIterator.h"
#include "Engine/BlueprintGeneratedClass.h"

#define LOCTEXT_NAMESPACE "K2Node_MounteaInteractionCallFunction"

namespace MounteaInteractionK2
{
	// Per-meta-key node colors - matching Inventory System color convention
	static const FLinearColor ColorGetter    = FLinearColor(0.05f, 0.35f, 0.75f); // blue
	static const FLinearColor ColorSetter    = FLinearColor(0.85f, 0.45f, 0.05f); // orange
	static const FLinearColor ColorValidate  = FLinearColor(0.75f, 0.10f, 0.10f); // red
	static const FLinearColor ColorCommand   = FLinearColor(0.10f, 0.60f, 0.10f); // green
	static const FLinearColor ColorBinding   = FLinearColor(0.50f, 0.10f, 0.70f); // purple
	static const FLinearColor ColorDefault   = FLinearColor(0.15f, 0.15f, 0.15f); // dark grey
}

const TArray<FName>& UK2Node_MounteaInteractionCallFunction::GetMounteaMetaKeys()
{
	static TArray<FName> keys = {
		FName("MounteaGetter"),
		FName("MounteaSetter"),
		FName("MounteaValidate"),
		FName("MounteaCommand"),
		FName("MounteaBinding"),
	};
	return keys;
}

FName UK2Node_MounteaInteractionCallFunction::GetMounteaMetaKey() const
{
	const UFunction* func = GetTargetFunction();
	if (!func) return NAME_None;

	for (const FName& key : GetMounteaMetaKeys())
	{
		if (func->HasMetaData(key)) return key;
	}
	return NAME_None;
}

FLinearColor UK2Node_MounteaInteractionCallFunction::GetNodeTitleColor() const
{
	const FName metaKey = GetMounteaMetaKey();

	if (metaKey == "MounteaGetter")   return MounteaInteractionK2::ColorGetter;
	if (metaKey == "MounteaSetter")   return MounteaInteractionK2::ColorSetter;
	if (metaKey == "MounteaValidate") return MounteaInteractionK2::ColorValidate;
	if (metaKey == "MounteaCommand")  return MounteaInteractionK2::ColorCommand;
	if (metaKey == "MounteaBinding")  return MounteaInteractionK2::ColorBinding;

	return MounteaInteractionK2::ColorDefault;
}

FSlateIcon UK2Node_MounteaInteractionCallFunction::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = FLinearColor(0.823f, 0.823f, 0.823f);

	const FName metaKey = GetMounteaMetaKey();

	if (metaKey == "MounteaGetter")
	{
		return FSlateIcon(FMounteaInteractionDeveloperStyle::GetStyleSetName(), "MISStyleSet.K2Node_GetterIcon.small");
	}
	if (metaKey == "MounteaSetter")
	{
		return FSlateIcon(FMounteaInteractionDeveloperStyle::GetStyleSetName(), "MISStyleSet.K2Node_SetterIcon.small");
	}
	if (metaKey == "MounteaValidate")
	{
		return FSlateIcon(FMounteaInteractionDeveloperStyle::GetStyleSetName(), "MISStyleSet.K2Node_ValidateIcon.small");
	}
	if (metaKey == "MounteaCommand")
	{
		return FSlateIcon(FMounteaInteractionDeveloperStyle::GetStyleSetName(), "MISStyleSet.K2Node_CommandIcon.small");
	}
	if (metaKey == "MounteaBinding")
	{
		return FSlateIcon(FMounteaInteractionDeveloperStyle::GetStyleSetName(), "MISStyleSet.K2Node_BindingIcon.small");
	}

	// No Mountea meta tag matched (shouldn't normally happen - GetMenuActions only ever spawns this
	// node class for tagged functions) - fall back to the engine's own default function icon rather
	// than requesting a brush key nothing registers.
	return Super::GetIconAndTint(OutColor);
}

FText UK2Node_MounteaInteractionCallFunction::GetTooltipText() const
{
	const FText defaultText = Super::GetTooltipText();
	const FName metaKey = GetMounteaMetaKey();

	if (metaKey == "MounteaGetter")
	{
		return FText::Format(
			INVTEXT("{0}\n\n📤 Getter: A getter function is designed to retrieve and return a specific value or property from an object without modifying it.\nGetters are essential for accessing data in a controlled manner, ensuring that external code can view data but not alter it directly."),
			defaultText);
	}
	if (metaKey == "MounteaSetter")
	{
		return FText::Format(
			INVTEXT("{0}\n\n📥 Setter: A setter function is responsible for updating or modifying a particular value or data in an object.\nIt directly affects the state of the target by assigning a new value to one of its properties.\nSetters may include internal validation or checks to ensure that the value being assigned meets certain criteria before applying it."),
			defaultText);
	}
	if (metaKey == "MounteaValidate")
	{
		return FText::Format(
			INVTEXT("{0}\n\n❔ Validator: Validator functions are used to verify the correctness or validity of data.\nThese functions perform checks to ensure that the data adheres to certain rules or constraints, such as format, range, or consistency.\nA successful validation confirms that the data is reliable and suitable for use."),
			defaultText);
	}
	if (metaKey == "MounteaCommand")
	{
		return FText::Format(
			INVTEXT("{0}\n\n⚡ Command: A command function triggers an action or state transition on the target rather than reading or writing a single value.\nCommands may have side effects (starting a timer, changing state, broadcasting delegates) and are not guaranteed to be idempotent - calling one twice can have a different effect than calling it once."),
			defaultText);
	}
	if (metaKey == "MounteaBinding")
	{
		return FText::Format(
			INVTEXT("{0}\n\nBinding: Binding functions subscribe or unsubscribe Blueprint events to native Mountea delegates.\nThe binding is possible even without direct class access which is required by regular events."),
			defaultText);
	}

	return defaultText;
}

FText UK2Node_MounteaInteractionCallFunction::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "Mountea|Interaction");
}

void UK2Node_MounteaInteractionCallFunction::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* actionKey = GetClass();
	if (!ActionRegistrar.IsOpenForRegistration(actionKey)) return;

	// Discover all UFunction objects in the MounteaInteractionSystem module that carry a Mountea meta tag
	static const FName mounteaModuleName("MounteaInteractionSystem");

	for (TObjectIterator<UClass> classIt; classIt; ++classIt)
	{
		const UClass* targetClass = *classIt;
		if (!IsValid(targetClass)) continue;

		// Limit to the runtime module - avoids picking up editor/developer functions
		const UPackage* pkg = targetClass->GetOutermost();
		if (!IsValid(pkg)) continue;

		const FString pkgName = pkg->GetName();
		if (!pkgName.Contains(mounteaModuleName.ToString())) continue;

		for (TFieldIterator<UFunction> funcIt(targetClass, EFieldIteratorFlags::ExcludeSuper); funcIt; ++funcIt)
		{
			UFunction* func = *funcIt;
			if (!func || !func->HasAnyFunctionFlags(FUNC_Static | FUNC_BlueprintCallable)) continue;

			bool bHasMeta = false;
			for (const FName& metaKey : GetMounteaMetaKeys())
			{
				if (func->HasMetaData(metaKey)) { bHasMeta = true; break; }
			}
			if (!bHasMeta) continue;

			// Without this, UEdGraphSchema_K2::CanUserKismetCallFunction() still considers this
			// function callable, so FBlueprintActionDatabase's own per-class scan (independent of
			// this GetMenuActions override) ALSO registers the engine's plain default K2Node_CallFunction
			// for the same UFunction — the palette/context menu then shows two entries with the same
			// name, and it's a coin flip which one gets placed. Marking it BlueprintInternalUseOnly
			// suppresses that duplicate, leaving only this custom colored/iconed node.
			func->SetMetaData(FBlueprintMetadata::MD_BlueprintInternalUseOnly, TEXT("true"));

			UBlueprintNodeSpawner* spawner = UBlueprintNodeSpawner::Create(GetClass());
			if (!spawner) continue;

			spawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda(
				[func](UEdGraphNode* NewNode, bool bIsTemplateNode)
				{
					if (UK2Node_MounteaInteractionCallFunction* callNode = Cast<UK2Node_MounteaInteractionCallFunction>(NewNode))
					{
						callNode->SetFromFunction(func);
					}
				});

			ActionRegistrar.AddBlueprintAction(actionKey, spawner);
		}
	}
}

#undef LOCTEXT_NAMESPACE
