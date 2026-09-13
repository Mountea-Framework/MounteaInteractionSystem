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
	OutColor = GetNodeTitleColor();

	const FName metaKey = GetMounteaMetaKey();

	if (metaKey == "MounteaGetter")
	{
		return FSlateIcon(FMounteaInteractionDeveloperStyle::GetStyleSetName(), "K2Node.Getter");
	}
	if (metaKey == "MounteaSetter")
	{
		return FSlateIcon(FMounteaInteractionDeveloperStyle::GetStyleSetName(), "K2Node.Setter");
	}
	if (metaKey == "MounteaValidate")
	{
		return FSlateIcon(FMounteaInteractionDeveloperStyle::GetStyleSetName(), "K2Node.Validate");
	}
	if (metaKey == "MounteaCommand")
	{
		return FSlateIcon(FMounteaInteractionDeveloperStyle::GetStyleSetName(), "K2Node.Command");
	}
	if (metaKey == "MounteaBinding")
	{
		return FSlateIcon(FMounteaInteractionDeveloperStyle::GetStyleSetName(), "K2Node.Binding");
	}

	return FSlateIcon(FMounteaInteractionDeveloperStyle::GetStyleSetName(), "K2Node.Default");
}

FText UK2Node_MounteaInteractionCallFunction::GetTooltipText() const
{
	const UFunction* func = GetTargetFunction();
	if (!func) return Super::GetTooltipText();

	FString tooltip = func->GetMetaData(FBlueprintMetadata::MD_Tooltip);
	if (tooltip.IsEmpty()) return Super::GetTooltipText();

	// Prefix with the meta category for discoverability
	const FName metaKey = GetMounteaMetaKey();
	if (!metaKey.IsNone())
	{
		tooltip = FString::Printf(TEXT("[%s] %s"), *metaKey.ToString(), *tooltip);
	}

	return FText::FromString(tooltip);
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
			const UFunction* func = *funcIt;
			if (!func || !func->HasAnyFunctionFlags(FUNC_Static | FUNC_BlueprintCallable)) continue;

			bool bHasMeta = false;
			for (const FName& metaKey : GetMounteaMetaKeys())
			{
				if (func->HasMetaData(metaKey)) { bHasMeta = true; break; }
			}
			if (!bHasMeta) continue;

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
