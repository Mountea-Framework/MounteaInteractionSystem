// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ActorInteractableRotationComponent.generated.h"

/**
 * Rotation type.
 */
UENUM(BlueprintType)
enum class ERotationFacingType : uint8
{
	ERFT_AllAxes			UMETA(DisplayName="All Axes", Tooltip="Rotation on All axes to always face Interactor."),
	ERFT_ZOnly				UMETA(DisplayName="Z Only", Tooltip="Rotation on Z axes with no limitation."),
	ERFT_ZOnlyLimited		UMETA(DisplayName="Z Only - Limited", Tooltip="Rotation on Z axes with predefined angle limitation."),
	ERFT_None				UMETA(DisplayName="None", Tooltip="Rotation is disabled."),

	ERFT_Default			UMETA(Hidden)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFacingTypeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLookAtActorChanged, AActor*, Actor);

/**
 * Rotation Component. Enables World Space Widget to always face Interactor.
 */
UCLASS(ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent))
class ACTORINTERACTIONPLUGIN_API UActorInteractableRotationComponent final : public USceneComponent
{
	GENERATED_BODY()

public:
	UActorInteractableRotationComponent();

protected:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnFacingTypeChangedEvent();
	UFUNCTION(BlueprintImplementableEvent, Category="Rotation")
	void OnFacingTypeChangedEventBP();

	UFUNCTION()
	void OnLookAtActorChangedEvent(AActor* Actor);
	UFUNCTION(BlueprintImplementableEvent, Category="Rotation")
	void OnLookAtActorChangedEventBP(AActor* Actor);

	
	UFUNCTION()
	void RotationActivated(UActorComponent* ActorComponent, bool bArg);
	UFUNCTION()
	void RotationDeactivated(UActorComponent* ActorComponent);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Rotation")
	void EnableTick();
	bool CanRotate() const;
	void RotateComponent(float DeltaTime);
	
public:

	/**
	 * Sets new Facing Type.
	 * Calls OnFacingTypeChanged.
	 * @param NewType Value to be set as Facing Type.
	 */
	UFUNCTION(BlueprintCallable, Category="Rotation")
	void SetFacingType(const ERotationFacingType& NewType);

	UFUNCTION(BlueprintCallable, Category="Rotation")
	void SetLookAtActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Rotation")
	AActor* GetLookAtActor() const;
	
protected:

	/** Actor to look at.*/
	UPROPERTY(VisibleAnywhere, Category="Rotation", meta=(DisplayThumbnail=false))
	AActor* LookAtActor = nullptr;
	/** How is this Component facing the Interactor.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rotation")
	ERotationFacingType FacingType;
	/**	How much is the rotation angle limited.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rotation", meta=(UIMin=0, ClampMin=0, UIMax=360, ClampMax=360, EditCondition="FacingType==ERotationFacingType::ERFT_ZOnlyLimited"))
	float AngleLimit;
	/**	How frequently is this Component being updated.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rotation", meta=(UIMin=0.1, ClampMin=0.1, EditCondition="FacingType!=ERotationFacingType::ERFT_None", Units=s))
	float Interval;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rotation", meta=(UIMin=0.1, ClampMin=0.1, EditCondition="FacingType!=ERotationFacingType::ERFT_None"))
	float InterpolationSpeed;
	
	/** Event called once FacingType has changed.*/
	UPROPERTY(BlueprintAssignable, Category="Rotation")
	FOnFacingTypeChanged OnFacingTypeChanged;
	/** Event called once Look At Actor has changed.*/
	UPROPERTY(BlueprintAssignable, Category="Rotation")
	FOnLookAtActorChanged OnLookAtActorChanged;

private:

	UPROPERTY(VisibleAnywhere, Category="Rotation")
	FRotator CachedRotation;
	UPROPERTY(VisibleAnywhere, Category="Rotation")
	FRotator LastRotation;
};
