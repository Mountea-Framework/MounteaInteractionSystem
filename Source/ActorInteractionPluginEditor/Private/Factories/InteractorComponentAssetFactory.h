// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "InteractorComponentAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class ACTORINTERACTIONPLUGINEDITOR_API UInteractorComponentAssetFactory : public UFactory
{
	GENERATED_BODY()
	
public:

	UInteractorComponentAssetFactory(const FObjectInitializer& ObjectInitializer);

	virtual bool ConfigureProperties() override;
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<class UActorInteractorComponentBase> ParentClass;
};
