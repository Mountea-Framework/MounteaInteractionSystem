// All rights reserved Dominik Morse (Pavlicek) 2021

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "InteractableComponentAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class ACTORINTERACTIONPLUGINEDITOR_API UInteractableComponentAssetFactory : public UFactory
{
	GENERATED_BODY()
	
public:

	UInteractableComponentAssetFactory(const FObjectInitializer& ObjectInitializer);

	virtual bool ConfigureProperties() override;
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<class UActorInteractableComponentBase> ParentClass;
};
