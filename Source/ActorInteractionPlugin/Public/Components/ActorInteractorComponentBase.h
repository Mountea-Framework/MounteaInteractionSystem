// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/ActorInteractorInterface.h"
#include "ActorInteractorComponentBase.generated.h"


UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactor Component"))
class ACTORINTERACTIONPLUGIN_API UActorInteractorComponentBase : public UActorComponent, public IActorInteractorInterface
{
	GENERATED_BODY()

public:

	UActorInteractorComponentBase();

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnInteractableFoundEvent(const TScriptInterface<IActorInteractableInterface>& FoundInteractable);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnInteractableLostEvent(const TScriptInterface<IActorInteractableInterface>& LostInteractable);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnInteractionKeyPressedEvent(const float TimeKeyPressed);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Interaction")
	void OnInteractionKeyReleasedEvent(const float TimeKeyReleased);

	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void StartInteraction() override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void StopInteraction() override;

	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool ActivateInteractor(FString& ErrorMessage) override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void DeactivateInteractor() override;

	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void TickInteraction(const float DeltaTime) override;

	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual ECollisionChannel GetResponseChannel() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetResponseChannel(const ECollisionChannel NewResponseChannel) override;


	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual EInteractorState GetState() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetState(const EInteractorState NewState) override;


	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual bool DoesAutoActivate() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction", meta=(DisplayName="Set Auto Activate"))
	virtual void SetDoesAutoActivate(const bool bNewAutoActivate) override;


	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual FKey GetInteractionKey() const override;
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void SetInteractionKey(const FKey NewInteractorKey) override;

protected:

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractableFound OnInteractableFound;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractableLost OnInteractableLost;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionKeyPressed OnInteractionKeyPressed;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionKeyReleased OnInteractionKeyReleased;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction|Required", meta=(DisplayName="Auto Activate", NoResetToDefault))
	uint8 bDoesAutoActivate : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction|Required", meta=(NoResetToDefault))
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction|Required", meta=(NoResetToDefault))
	FKey InteractionKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction|Required", meta=(NoResetToDefault))
	EInteractorState InteractorState;
};
