// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Interfaces/ActorInteractableInterface.h"
#include "ActorInteractableComponentBase.generated.h"


UCLASS(ClassGroup=(Interaction), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Interactable Component"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponentBase : public UWidgetComponent, public IActorInteractableInterface
{
	GENERATED_BODY()

public:

	UActorInteractableComponentBase();

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	virtual bool DoesAutoActive() const override;
	virtual void ToggleAutoActivate(const bool NewValue) override;

	virtual bool DoesAutoSetup() const override;
	virtual void ToggleAutoSetup(const bool NewValue) override;

	virtual bool ActivateInteractable(FString& ErrorMessage) override;
	virtual bool WakeUpInteractable(FString& ErrorMessage) override;
	virtual bool SuppressInteractable(FString& ErrorMessage) override;
	virtual void DeactivateInteractable() override;

	virtual EInteractableState GetState() const override;
	virtual void SetState(const EInteractableState& NewState) override;
	
	virtual TScriptInterface<IActorInteractorInterface> GetInteractor() const override;
	virtual void SetInteractor(const TScriptInterface<IActorInteractorInterface> NewInteractor) override;

	virtual int32 GetInteractableWeight() const override;
	virtual void SetInteractableWeight(const int32 NewWeight) override;

	virtual UObject* GetInteractableOwner() const override;
	virtual void SetInteractableOwner(const UObject* NewOwner) override;

	virtual ECollisionChannel GetCollisionChannel() const override;
	virtual void SetCollisionChannel(const ECollisionChannel& NewChannel) override;

	virtual TArray<UPrimitiveComponent*> GetCollisionComponents() const override;
	virtual void AddCollisionComponent(const UPrimitiveComponent* CollisionComp) override;
	virtual void AddCollisionComponents(const TArray<UPrimitiveComponent*> NewCollisionComponents) override;
	virtual void RemoveCollisionComponent(const UPrimitiveComponent* CollisionComp) override;
	virtual void RemoveCollisionComponents(const TArray<UPrimitiveComponent*> RemoveCollisionComponents) override;

	virtual TArray<UMeshComponent*> GetHighlightableComponents() const override;
	virtual void AddHighlightableComponent(const UMeshComponent* HighlightableComp) override;
	virtual void AddHighlightableComponents(const TArray<UMeshComponent*> HighlightableComponents) override;
	virtual void RemoveHighlightableComponent(const UMeshComponent* HighlightableComp) override;
	virtual void RemoveHighlightableComponents(const TArray<UMeshComponent*> HighlightableComponents) override;
	

	
	virtual UMeshComponent* FindMeshByTag(const FName Tag) const override;
	virtual UPrimitiveComponent* FindPrimitiveByTag(const FName Tag) const override;

	virtual TArray<FName> GetCollisionOverrides() const override;
	virtual void AddCollisionOverride(const FName Tag) override;
	virtual void AddCollisionOverrides(const TArray<FName> Tags) override;
	virtual void RemoveCollisionOverride(const FName Tag) override;
	virtual void RemoveCollisionOverrides(const TArray<FName> Tags) override;

	virtual TArray<FName> GetHighlightableOverrides() const override;
	virtual void AddHighlightableOverride(const FName Tag) override;
	virtual void AddHighlightableOverrides(const TArray<FName> Tags) override;
	virtual void RemoveHighlightableOverride(const FName Tag) override;
	virtual void RemoveHighlightableOverrides(const TArray<FName> Tags) override;

	/**
	 * Development Only.
	 * Toggles debug On/Off.
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Interaction", meta=(DevelopmentOnly))
	virtual void ToggleDebug() override
	{
		bToggleDebug = !bToggleDebug;
	}

protected:

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractorFound OnInteractorFound;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractorLost OnInteractorLost;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractorTraced OnInteractorTraced;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractorOverlapped OnInteractorOverlapped;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionCompleted OnInteractionCompleted;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionStarted OnInteractionStarted;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Interaction")
	FInteractionStopped OnInteractionStopped;

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableAutoSetupChanged OnInteractableAutoSetupChanged;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableWeightChanged OnInteractableWeightChanged;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableStateChanged OnInteractableStateChanged;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractableOwnerChanged OnInteractableOwnerChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FHighlightableComponentAdded OnHighlightableComponentAdded;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FHighlightableComponentRemoved OnHighlightableComponentRemoved;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionComponentAdded OnCollisionComponentAdded;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionComponentRemoved OnCollisionComponentRemoved;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FHighlightableOverrideAdded OnHighlightableOverrideAdded;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionOverrideAdded OnCollisionOverrideAdded;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FHighlightableOverrideRemoved OnHighlightableOverrideRemoved;
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FCollisionOverrideRemoved OnCollisionOverrideRemoved;

protected:

	UPROPERTY()
	uint8 bToggleDebug : 1;
	
	UPROPERTY()
	uint8 bInteractableAutoActivate : 1;
	
	UPROPERTY()
	uint8 bInteractableAutoSetup : 1;


	UPROPERTY()
	EInteractableState InteractableState;

	UPROPERTY()
	TEnumAsByte<ECollisionChannel> CollisionChannel;


	UPROPERTY()
	int32 InteractionWeight;

	UPROPERTY()
	UObject* InteractionOwner;

	UPROPERTY()
	TArray<FName> CollisionOverrides;
	
	UPROPERTY()
	TArray<FName> HighlightableOverrides;

	
	UPROPERTY()
	TArray<UMeshComponent*> HighlightableMeshComponents;

	UPROPERTY()
	TArray<UPrimitiveComponent*> CollisionComponents;

private:
	
	UPROPERTY()
	TScriptInterface<IActorInteractorInterface> Interactor;
};