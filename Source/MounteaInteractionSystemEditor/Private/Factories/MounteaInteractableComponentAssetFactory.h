// All rights reserved Dominik Morse (Pavlicek) 2021

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaInteractableComponentAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAINTERACTIONSYSTEMEDITOR_API UMounteaInteractableComponentAssetFactory : public UFactory
{
	GENERATED_BODY()
	
public:

	UMounteaInteractableComponentAssetFactory(const FObjectInitializer& ObjectInitializer);

	virtual bool ConfigureProperties() override;
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<class UMounteaInteractableComponentBase> ParentClass;
};
