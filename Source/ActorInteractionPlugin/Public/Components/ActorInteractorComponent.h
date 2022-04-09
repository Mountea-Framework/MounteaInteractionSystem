// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Helpers/InteractionHelpers.h"

#include "ActorInteractorComponent.generated.h"

class UActorInteractableComponent;

/**
 * Interaction Tracing Data used for easier communication.
 */
struct FInteractionTraceData
{
	FVector StartLocation;
	FVector EndLocation;
	FRotator TraceRotation;
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	ECollisionChannel CollisionChannel;

	FInteractionTraceData(const FVector& Start, const FVector& End, const FRotator Rotation, const FHitResult& Hit, const FCollisionQueryParams& Params, const ECollisionChannel& Channel)
	{
		StartLocation = Start;
		EndLocation = End;
		TraceRotation = Rotation;
		HitResult = Hit;
		CollisionParams = Params;
		CollisionChannel = Channel;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableFound, class UActorInteractableComponent*, FoundActorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableLost, class UActorInteractableComponent*, LostActorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionKeyPressed, float, TimeKeyPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionKeyReleased, float, TimeKeyReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractorTypeChanged, float, TimeChanged);

/**
 * Implement an Actor component for interaction.
 *
 * An Interactor Component is a non-transient component that enables its owning Actor to interact with Interactable Actors.
 * Interaction is processed by Interactor Component provided with base data from the Interactable Component.
 * Dependency injection is achieved by using IInteractorComponentInterface instead of direct class usage.
 * 
 * @note	Interactor Actor Component is using "ActorEyesViewPoint" by default. To change this value, go to Actor -> Base Eye Height.
 * - Higher the value, higher the tracing start point will be located.
 *
 * @warning Networking is not implemented.
 *
 * @see [InteractorComponent](https://sites.google.com/view/dominikpavlicek/home/documentation)
 */
UCLASS(ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent, DisplayName = "Interactor Component"))
class ACTORINTERACTIONPLUGIN_API UActorInteractorComponent final : public UActorComponent
{
	GENERATED_BODY()
	
	UActorInteractorComponent();

protected:
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void TickInteraction(const float DeltaTime);
	void TickPrecise(FInteractionTraceData& TraceData) const;
	void TickLoose(FInteractionTraceData& TraceData) const;

	void UpdateTicking();
	void UpdatePrecision();

	UFUNCTION()
	void OnRep_InteractorState();

	UFUNCTION()
	void OnRep_IgnoredActors();

#pragma region Getters_Setters
	
public:

	/**
	 * Defines whether Ticking is allowed for this Interactor Actor Component.
	 * @return True if:
	 * - Component is loaded properly
	 * - Component has Owner (is attached to an Actor)
	 * - Component State is
	 * - - StandBy OR
	 * - - Active
	 * - Interactor Type is equal Active
	 * - - InteractorTickInterval is larger than 0
	 * - - - AND InteractorTickInterval has passed
	 * - - OR InteractorTickInterval is 0
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Validation")
	FORCEINLINE bool CanTick() const
	{
		return
		GetWorld() != nullptr &&
		GetOwner() != nullptr &&
		(
			GetInteractionState() == EInteractorState::EIS_StandBy ||
			GetInteractionState() == EInteractorState::EIS_Active
		)
		&&
		GetInteractorType() == EInteractorType::EIT_Active &&
		(
			GetInteractorTickInterval() <= KINDA_SMALL_NUMBER ||
			GetInteractorTickInterval() > KINDA_SMALL_NUMBER &&
			(
				GetWorld()->TimeSince(LastTickTime) > GetInteractorTickInterval()
			)
		);
	};

	/**
	 *Returns what Interactable Actor Component is interacting with. If none, returns nullptr.
	 * @return	Interactable Component that is being interacted with
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE UActorInteractableComponent* GetInteractingWith() const { return InteractingWith; };

	/**
	 * Sets what Interactable Actor Component is interacting with. If none, pass nullptr.
	 * @param NewInteractingWith	Value to be set as Interacting with, could be nullptr
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractingWith(UActorInteractableComponent* NewInteractingWith);

	/**
	 * Returns Interaction Precision to work with.
	 * @return Interaction Precision 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE EInteractorPrecision GetInteractionPrecision() const { return InteractorPrecision; };

	/**
	 * Sets Interaction Precision to work with.
	 * @param NewPrecision	Value to be used as new Interaction Precision
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionPrecision(const EInteractorPrecision NewPrecision);

	/**
	 * Returns Interaction Precision Box Half Extend (in centimeters) which is used as a tracing shape.
	 * @return Value of Box Half Extend in centimeters
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	float GetPrecisionBoxHalfExtend() const {return InteractorPrecisionBoxHalfExtend; };

	/**
	 * Sets Interaction Precision Box half extend which will be used as a tracing shape.
	 * @param NewBoxHalfExtend Value in centimeters
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionBoxHalfExtend(const float NewBoxHalfExtend);

	/**
	 * Returns whether Custom Trace Start is allowed or not.
	 * @return True or false
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE bool GetUseCustomTraceStart() const {return bUseCustomStartTransform; };

	/**
	 * Sets whether Custom Trace Start is allowed or not.
	 * @param NewValue Value of True or False
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetUseCustomTraceStart(const bool NewValue);

	/**
	 * Returns Transform in World Space where does Interaction Tracing start if Custom Trace Start is allowed. Otherwise returns empty Transform.
	 * @return Transform in World Space where Tracing starts at
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE FTransform GetCustomTraceStart() const {return CustomTraceTransform; };

	/**
	 * Sets Transform in World Space where does Interaction Tracing start if Custom Trace Start is allowed. Otherwise sets empty Transform.
	 * @param NewTraceStart	Transform in World Space where Tracing should start at
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetCustomTraceStart(const FTransform NewTraceStart);

	/**
	 * Returns Collision Channel that is used by this Interactor Actor Component.
	 * This Collision Channel is used for looking for Interactable Components with matching Collision Channel.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE ECollisionChannel GetInteractorTracingChannel() const {return InteractorTracingChannel; };

	/**
	 * Sets Collision Channel that will be used by this Interactor Actor Component to Trace against.
	 * Only Interactable Actors with matching Collision Channel are used.
	 * @param NewChannel	Value of Collision Channel to Trace against
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractorTracingChannel(ECollisionChannel NewChannel);

	/**
	 * Returns Interaction Tick Interval in seconds.
	 * @return Value in seconds how long does it take between Tick
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE float GetInteractorTickInterval() const { return InteractionTickInterval; };

	/**
	 * Sets Interaction Tick Interval in seconds.
	 * @note	If zero, Interaction Tick Interval will be every Tick.
	 * @note	Values below zero are clamped at zero
	 * @param NewRefreshRate	Value in seconds
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractorTickInterval(const float NewRefreshRate);

	/**
	 * Returns Interaction Range in centimeters. Larger values mean further Tracing can trace.
	 * @return	Value in centimeters how far Tracing goes when looking for Interactable Components
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE float GetInteractionRange() const { return InteractionRange; };

	/**
	 * Sets Interaction Range in centimeters.
	 * @note	Values below zero are clamped at zero
	 * @param NewRange	Value in centimeters how far Tracing should go when looking for Interactable Components
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionRange(const float NewRange);

	/**
	 * Returns Interaction State.
	 * @return Value of Interactor State
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE EInteractorState GetInteractionState() const { return InteractorState; };

	/**
	 * Sets Interaction State.
	 * @param NewState	Value of Interactor State to be used
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionState(const EInteractorState NewState);

	/**
	 * Returns Interactor Type.
	 * @return Returns type of the Interactor Component
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE EInteractorType GetInteractorType() const { return InteractorType; };

	/**
	 * Sets Interactor Type.
	 * @param NewInteractorType	Value of the Interactor Type to be used
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractorType(const EInteractorType NewInteractorType);
	
	/**
	 * Returns whether Interactor is set by default to Auto Activate or not.
	 * @return Auto Activate value of the Component
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE bool GetInteractorAutoActivate() const {return bInteractorAutoActivate; };

	/**
	 * Sets whether the Interaction Component should be activated by default (during BeginPlay) or whether it should wait to be activated.
	 * @param bValue	True = should be defined, False = should wait for activation
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractorAutoActivate(const bool bValue);

	/**
	 * Returns a list of Actors who are being ignored when tracing.
	 * @return	List of ignored Actors
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	TArray<AActor*> GetIgnoredActors() const {return IgnoredActors; };

	/**
	 * Sets a new list of ignored Actors and overriding which is currently in use.
	 * @param NewIgnoredActors A list of Actors to be ignored.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetIgnoredActors(TArray<AActor*>& NewIgnoredActors)
	{
		IgnoredActors = NewIgnoredActors;
	}

	/**
	 * Adds a single Actor to the list of ignored Actors.
	 * @param IgnoredActor	Actor to be ignored.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void AddIgnoredActor(AActor* IgnoredActor)
	{
		IgnoredActors.Add(IgnoredActor);
	}
	
	/**
	 * Adds a list of Actors to the list of ignored Actors.
	 * @param ActorsToAdd	Array of Actors to be added to the ignored Actors list
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void AddIgnoredActors(const TArray<AActor*>& ActorsToAdd)
	{
		IgnoredActors.Append(ActorsToAdd);
	}

	/**
	 * Removes a single Actor from the list of ignored Actors.
	 * @param UnignoredActor	Actor who is no longer being ignored. 
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void RemoveIgnoredActor(AActor* UnignoredActor)
	{
		IgnoredActors.Remove(UnignoredActor);
	}

	/**
	 * Removes a list of Actors from the list of ignored Actors.
	 * @param UnignoredActors	List of Actors who ar eno longer being ignored.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void RemoveIgnoredActors(const TArray<AActor*>& UnignoredActors)
	{
		for (auto& Itr : UnignoredActors)
		{
			if (IgnoredActors.Contains(Itr))
			{
				IgnoredActors.Remove(Itr);
			}
		}
	}

#pragma endregion Getters_Setters

#pragma region Events
public:
	
	/**
	 * Delegate called after Interactable Component is found. 
	 */
	UPROPERTY(BlueprintCallable, Category="Interaction|Events")
	FOnInteractableFound OnInteractableFound;

	/**
	 * Delegate called after Interactable Component is lost. 
	 */
	UPROPERTY(BlueprintCallable, Category="Interaction|Events")
	FOnInteractableLost OnInteractableLost;
	
	/**
	 * Delegate called after Interaction Key is pressed.
	 * @param TimeKeyPressed	Value in seconds. Must be set for Interaction to work properly.
	 */
	UPROPERTY(BlueprintCallable, Category="Interaction|Events")
	FOnInteractionKeyPressed OnInteractionKeyPressed;

	/**
	 * Delegate called after Interaction Key is released.
	 * @param TimeKeyReleased	Value in seconds. Must be set for Interaction to work properly.
	 */
	UPROPERTY(BlueprintCallable, Category="Interaction|Events")
	FOnInteractionKeyReleased OnInteractionKeyReleased;

	/**
	 * Delegate called after Interactor Type is changed.
	 * Responsible for updating all necessary value for Interactor to work properly.
	 */
	UPROPERTY(BlueprintCallable, Category="Interaction|Events")
	FOnInteractorTypeChanged OnInteractorTypeChanged;

#pragma endregion Events

#pragma region Properties
public:
	
	virtual void StartInteraction();// override;
	virtual void StopInteraction();// override;

	/**
	 * A simple function that should be called when Interactor is requested to be activated and start searching for Interactable Components.
	 * @param ErrorMessage	[OUT]	Error message that will be filled if results is false. Otherwise is empty. Is always cleared upon retrieval.
	 * @return	Returns true if all required checks are OK, otherwise returns false with ErrorMessage which contains explanation.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|State")
	bool ActivateInteractor(FString& ErrorMessage);
	
	/**
	 * A simple function that should be called when Interactor is requested to be deactivated and stop searching for Interactable Components.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|State")
	void DeactivateInteractor();

protected:
	
	/** Defines whether Activation must be done or is turned on by default.*/
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category="Interaction|Settings", meta=(DispplayName="Auto Activate"))
	uint8 bInteractorAutoActivate : 1;
	
	/**
	 * Defines type of the Interactor Component.
	 * Could be either Active or Passive.
	 * @note	Active one is looking for Interactable objects each Tick (based on InteractionTickInterval).
	 * @note	Passive on is not looking for any Interactable objects and is activated only when overlapping such objects.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Settings")
	EInteractorType InteractorType = EInteractorType::EIT_Active;
	
	/**
	 * Defines how precise the interaction is.
	 * - Low precision is using ShapeTrace and might cause issues if there are many interactable items close each other.
	 * - High precision is using LineTrace and might cause frustration when interacting.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Settings", meta=(EditCondition="InteractorType == EInteractorType::EIT_Active"))
	EInteractorPrecision InteractorPrecision = EInteractorPrecision::EIP_Low;
	
	/**
	 * Defines half extend of Low precision Interaction Tracing method.
	 * - Higher the value, less precise interaction is.
	 * - Lower the value, more precise interaction is.
	 * @note	Extremely low values are treated as Zero.
	 * @note	Zero value will result in using High precision instead.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Settings", meta=(Units = "cm", EditCondition="InteractorType == EInteractorType::EIT_Active && InteractorPrecision == EInteractorPrecision::EIP_Low"))
	float InteractorPrecisionBoxHalfExtend = 5.0f;
	
	/**
	 * Defines which Collision Channel is used for Interaction Tracing.
	 * 
	 * This channel will used to Trace against. This channel defines which Interactable Actor Components will be found. Only those Components with the same Collision Channel will be found.
	 * Can use custom Collision Channel.
	 * @note	Default Channel: ECC_Visibility.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Settings", meta=(EditCondition="InteractorType == EInteractorType::EIT_Active"))
	TEnumAsByte<ECollisionChannel> InteractorTracingChannel = ECollisionChannel::ECC_Visibility;
	
	/**
	 * Defines whether Tracing starts at ActorEyesViewPoint (default) or at a given Location.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Settings", meta=(EditCondition="InteractorType == EInteractorType::EIT_Active"))
	uint8 bUseCustomStartTransform : 1;
	
	/**
	 * A list of Actors that won't be taken in count when tracing.
	 * If left empty, only Owner Actor is ignored.
	 * If using multiple Actors (a gun, for instance), all those child/attached Actors should be ignored.
	 */
	UPROPERTY(Replicated, EditInstanceOnly, Category="Interaction|Settings", ReplicatedUsing=OnRep_IgnoredActors)
	TArray<AActor*> IgnoredActors;
	
	/**
	 * Transform in World Space.
	 * @note	Suggested use:
	 * - Update from Code/Blueprint to follow Socket, like Weapon Barrel
	 * 
	 * Defines where does the Tracing start and what direction it follows.
	 * Will be ignored if bUseCustomStartTransform is false.
	 */
	UPROPERTY(Replicated, VisibleAnywhere, Category="Interaction|Settings", AdvancedDisplay, meta=(DisplayName="Trace Start (World Space Transform)"))
	FTransform CustomTraceTransform;
	
	/**
	 * Optimization feature.
	 * The frequency in seconds at which the Interaction function will be executed. If less than or equal 0 then it will tick every frame.
	 * 
	 * @note	Lower the value, less frequent ticking is and less performance is required.
	 * @note	Higher the value, more frequent ticking is and more performance is required.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Settings|Optimization", meta=(Units = "s", UIMin=0, ClampMin=0, EditCondition="InteractorType == EInteractorType::EIT_Active", DisplayName="Tick Interval (sec)"))
	float InteractionTickInterval = 3.f;
	
	/**
	 * Defines the lenght of interaction vision.
	 * @note	Higher the value, further items can be reached.
	 */
	UPROPERTY(Replicated, EditAnywhere, Category="Interaction|Settings|Optimization", meta=(Units = "cm", UIMin=0, ClampMin=0, EditCondition="InteractorType == EInteractorType::EIT_Active", DisplayName="Interaction Range (cm)"))
	float InteractionRange = 250.f;

	/**
	 * Editor only flag.
	 * If true, will display debug lines and boxes.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Debug")
	uint8 bDebug : 1;

private:
	
	UPROPERTY(Replicated)
	UActorInteractableComponent* InteractingWith = nullptr;
	
	UPROPERTY(Replicated)
	EInteractorState InteractorState;
	
	UPROPERTY(Replicated)
	float LastInteractionTickTime;
	
	UPROPERTY(Replicated)
	float LastTickTime;	

#pragma endregion Properties

private:
	
	#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	#endif

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};
