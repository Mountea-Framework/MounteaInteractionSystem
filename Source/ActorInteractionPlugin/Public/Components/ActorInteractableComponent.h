// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ShapeComponent.h"
#include "Components/WidgetComponent.h"
#include "Helpers/InteractionHelpers.h"

#include "ActorInteractableComponent.generated.h"

class UActorInteractorComponent;
class UActorInteractableWidget;
class UBoxComponent;
class UShapeComponent;

#define LOCTEXT_NAMESPACE "ActorInteractionPlugin"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionCompleted, EInteractableType, FinishedInteractionType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionStarted, EInteractableType, RecievedInteractionType, float, RecievedInteractionTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionStopped);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractorFound, UActorInteractorComponent*, InteractingComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractorLost, UActorInteractorComponent*, InteractingComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableTraced, UActorInteractorComponent*, TracingInteractorComponent);

/**
 * Implement an Actor component for being interacted with.
 * 
 * An Interactable Component is a non-transient component that enables other Actors, who posses an Interactor Component, to interact.
 * Interaction is processed by Interactor Component provided with base data from the Interactable Component.
 * 
 * All logic, like hiding and setting values, should be done in Blueprint classes to keep this Widget Component as flexible as possible.
 * 
 * @note Networking is not implemented.
 * 
 * @see [InteractableComponent](https://sites.google.com/view/dominikpavlicek/home/documentation)
 */
UCLASS(ClassGroup=(Interaction), NotBlueprintable, HideCategories = (Navigation, Physics, Collision, Lighting, Rendering, Mobile, Animation, HLOD, UserInterface), meta=(BlueprintSpawnableComponent, DisplayName="Interactable Component"))
class ACTORINTERACTIONPLUGIN_API UActorInteractableComponent final : public UWidgetComponent
{
	GENERATED_BODY()
	
	UActorInteractableComponent();

public:

	#pragma region Getters_Setters

	/**
	 * Returns type of the Interactable Actor Component.
	 * @return Interactable Type 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE EInteractableType GetInteractionType() const {return InteractionType; };
	
	/**
	 * Returns Collision Channel that is set for this Interactable. Interactor is looking for matching Collision Channel only.
	 * @return Collision Channel of this Interactable
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction|Getters")
	FORCEINLINE ECollisionChannel GetInteractionCollisionChannel() const
	{
		return InteractableCollisionChannel;
	};
		
	/**
	 * Returns Last Time in seconds that this Interactable was used.
	 * @return Last Interaction Time in seconds.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	FORCEINLINE float GetLastInteractionTime() const {return LastInteractionTime;};
		
	/**
	 * Returns Interaction Time required for Interaction to be completed.
	 * @return Interaction Time
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	FORCEINLINE float GetInteractionTime () const {return InteractionTime;};
	
	/**
	 * If true, all Mesh Components will be set to allow Custom Depth Rendering. If properly setup, such Components should be highlighted.
	 * @return Whether highlight is allowed or not
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	bool GetInteractionHighlight() const {return bInteractionHighlight; };
	
	/**
	 * Returns what kind inf Lifecycle Type is set for the Interactable Component.
	 * @return Interaction Lifecycle Type
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	EInteractableLifecycle GetInteractionLifecycle() const {return InteractableLifecycle; };

	/**
	 * Return how many Lifecycles are allowed. If maximum is achieved, interaction is no longer available. 
	 * @return Max Allowed Lifecycles
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	int32 GetMaxAllowedLifecycles() const {return InteractionCyclesAllowed; };

	/**
	 * Returns how many Lifecycles have already passed. If PassedLifecycles is equal MaxAllowedLifecycles, interaction is no longer allowed.
	 * @return How many Lifecycles have passed
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	int32 GetPassedLifecycles() const {return InteractionCounter; };

	/**
	 * Returns duration in seconds, how long does it take for Interactable Component to be interactable again after last time interaction happened.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	float GetInteractionCooldown() const {return InteractionCooldown; };

	/**
	 * Returns Interactable Action Title to be displayed in a Widget. Should describe the Object that player is interacting with or a General Title of the Action.
	 * @return Name of Interactable Action Title
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	FText GetInteractionActionTitle() const {return InteractionActionTitle; };

	/**
	 * Returns Interactable Action Text to be displayed in a Widget. Should describe what Action should the player perform to start the interaction.
	 * Suggestion is to use name of the Interactable Type (Press, Hold etc.)
	 * @return	Name of the Interactable Action
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	FText GetInteractionActionText() const {return InteractionActionText; };

	/**
	 * Returns Interactable Action Key to be displayed in a Widget. Should show the interaction Key that is set in bindings.
	 * @return Interactable Action Key
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	FText GetInteractionActionKey() const {return InteractionActionKey; };

	/**
	 * Returns current state of the Interactable Actor Component. Interactable State is using static state machine for quick and precise decision when and where interaction is allowed.
	 * @return State of the Interactable Actor Component
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	FORCEINLINE EInteractableState GetInteractionState() const { return InteractableState; };

	/**
	 * Returns whether Interactable is set by default to Auto Activate or not.
	 * @return Auto Activate value of the Component
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	FORCEINLINE bool GetInteractableAutoActivate() const {return bInteractableAutoActivate; };
	
	/**
	 * Sets type of the Interactable Component, whether it is Press or Hold.
	 * @param NewType Type to be used as the new Interactable Type.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionType(const EInteractableType NewType)
	{
		InteractionType = NewType;
	};

	/**
	 * Sets what Collision Channel is the Interactable Component using to be detected. Only matching Collision Channels are being detected by Interactor Component.
	 * @param NewChannel Channel to be used for Tracing by Interactor Component
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionCollisionChannel(const ECollisionChannel NewChannel)
	{
		InteractableCollisionChannel = NewChannel;

		UpdateCollisionChannels();
	};

	/**
	 * Sets Last time in seconds when the interaction has been performed.
	 * @param NewLastInteractionTime Time in seconds to be saved as Last Interaction Time
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetLastInteractionTime(const float NewLastInteractionTime)
	{
		LastInteractionTime = NewLastInteractionTime;
	}

	/**
	 * Sets Time in seconds, which defines how long does the interaction need to take to be finished.
	 * @param NewInteractionTime Time in seconds
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionTime(const float NewInteractionTime)
	{
		InteractionTime = NewInteractionTime;

		UpdateInteractableWidget();
	}

	/**
	 * Sets whether Interactable Component should highlight all Mesh Components from its Owner upon interaction detection.
	 * @note	LOCAL ONLY
	 * @param NewValue True or false
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionHighlight(const bool NewValue)
	{
		bInteractionHighlight = NewValue;
	};

	/**
	 * Sets Interactable Lifecycle type that will be used to define whether interaction is allowed only once or multiple times.
	 * @param NewLifecycleType Lifecycle Type to be set
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionLifecycleType(const EInteractableLifecycle NewLifecycleType)
	{
		InteractableLifecycle = NewLifecycleType;
	};
	
	/**
	 * Sets how many times interaction can be performed before it is restricted.
	 * @note	Value of zero is treated as unlimited.
	 * @param NewMaxAmount Value of the maximum interaction cycles
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionAllowedLifecycles(const int32 NewMaxAmount)
	{
		InteractionCyclesAllowed = NewMaxAmount;
	};

	/**
	 * Sets how many Lifecycles have been passed already. This function directly sets value.
	 * @note	For incrementing, use IncrementInteractionPassedLifecycles
	 * @param NewPassedCycles	Value of passed Lifecycles
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionPassedLifecycles(const int32 NewPassedCycles)
	{
		InteractionCounter = NewPassedCycles;
	};
	
	/**
	 * Increments value by given amount. For decrementing use negative value as input.
	 * By default, increment is by 1. Unless you want to change the amount, you can call this function without providing any input.
	 * @param IncrementBy	Value to incremented by
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void IncrementInteractionPassedLifecycles(const int32 IncrementBy = 1)
	{
		InteractionCounter += IncrementBy;
	};

	/**
	 * Sets the Cooldown time that is required to pass before interaction is allowed again.
	 * @note	Value of zero is treated as immediate and cooldown stage will be skipped.
	 * @param NewCooldown	Time in seconds how long cooldown state will last
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionCooldownTime(const float NewCooldown)
	{
		InteractionCooldown = NewCooldown;

		UpdateInteractableWidget();
	};

	/**
	 * Sets Interaction Action Title that will be displayed in a Widget.
	 * Should describe the Actor that is interactable with.
	 * @note	Empty value is by default treated as hidden.
	 * @param NewName	Value to be set as Actor Name.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionActionTitle(const FText& NewName)
	{
		InteractionActionTitle = NewName;

		UpdateInteractableWidget();
	};

	/**
	 * Sets Interaction Action Text that will be displayed in a Widget.
	 * Should describe what Action is required for the interaction.
	 * @note	Empty value is by default treated as hidden.
	 * @param NewName	Value to be set as Action Name 
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionActionText(const FText& NewName)
	{
		InteractionActionText = NewName;

		UpdateInteractableWidget();
	};

	/**
	 * Set Interaction Action Key Name that will be displayed in a Widget.
	 * @note	Should be the same Key that is set to you interaction binding.
	 * @param NewKey	Value to be set as Action Key
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionActionKey(const FText& NewKey)
	{
		InteractionActionKey = NewKey;

		UpdateInteractableWidget();
	};
	
	/**
	 * Sets new state of the Interactable Component.
	 * @note To enable interaction, set state to Inactive
	 * @param NewState	Value of the State to be set
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionState(const EInteractableState NewState);

	/**
	 * Sets progress in child Interactable Widget.
	 * Accepted values are 0, 1 and everything in between.
	 * 
	 * @note 0 means 0%
	 * @note 1 means 100%
	 * @param RemainingProgress	Progress value between 0 and 1
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetRemainingInteractionProgress(const float& RemainingProgress) const;

	/**
	 * Sets whether the Interaction Component should be activated by default (during BeginPlay) or whether it should wait to be activated.
	 * @param bValue	True = should be defined, False = should wait for activation
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractableAutoActivate(const bool bValue)
	{
		bInteractableAutoActivate = bValue;

		UpdateInteractableWidget();
	}

	/**
	 * Helper function to get User Widget class which is being displayed.
	 * @return	User Widget Class.
	 */
	UFUNCTION(BlueprintCallable,Category="Interaction|Getters")
	FORCEINLINE TSubclassOf<UActorInteractableWidget> GetInteractableWidgetClass() const
	{
		return InteractableWidgetClass;
	}

	/**
	 * Helper function which will set the Interactable Widget class to be used.
	 * @param Class	Subclass of Interactable Widget class.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractableWidgetClass(const TSubclassOf<UActorInteractableWidget> Class)
	{
		InteractableWidgetClass = Class;

		OnWidgetClassChanged();
	}

	/**
	 * Returns the Interactor Component which is performing interaction action with this Interactable.
	 * @return	Interactor or nullptr
	 */
	UFUNCTION(BlueprintCallable,Category="Interaction|Getters")
	FORCEINLINE UActorInteractorComponent* GetInteractorComponent() const {return InteractingInteractorComponent; };

	/**
	 * Helper function to set Interactor Component which is doing interaction with this Interactable.
	 * To clear the value simply pass down a nullptr.
	 * Overriding with the same value is not allowed.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractorComponent(UActorInteractorComponent* NewComponent)
	{
		if (NewComponent != InteractingInteractorComponent)
		{
			InteractingInteractorComponent = NewComponent;
		};
	}

	/**
	 * Returns a list of Collision Shapes for this Interactable Component.
	 * If empty, Interaction will be disabled!
	 * @return	Array of Shape Components
	 */
	UFUNCTION(BlueprintCallable,Category="Interaction|Getters")
	TArray<UShapeComponent*> GetCollisionShapes() const {return CollisionShapes; };

	/**
	 * Returns whether Searched Component is within Collision Shapes.
	 * @return	True if within Collision Shapes, false if Searched is nullptr or if not withing the Array.
	 */
	UFUNCTION(BlueprintCallable,Category="Interaction|Getters")
	bool FindCollisionShape(UShapeComponent* const SearchedComponent) const
	{
		if (SearchedComponent == nullptr)
		{
			return false;
		}
		
		return CollisionShapes.Contains(SearchedComponent);
	};
	
	/**
	 * Returns Shape Component at given Index (if exists). Otherwise returns nullptr.
	 * @return	Shape Component from Array, nullptr if invalid index.
	 */
	UFUNCTION(BlueprintCallable,Category="Interaction|Getters")
	UShapeComponent* FindCollisionShapeAtIndex(const int32 Index) const
	{
		if(CollisionShapes.IsValidIndex(Index))
		{
			return CollisionShapes[Index];
		}

		return nullptr;
	};
	
	/**
	 * Helper function to set Collision Shapes which Interactors are colliding with.
	 * Overrides existing Collision Shapes and sets the new ones.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetCollisionShapes(const TArray<UShapeComponent*> NewCollisionShapes)
	{
		RemoveCollisionShapes(NewCollisionShapes);

		AddCollisionShapes(NewCollisionShapes);
	}

	/**
	 * Helper function to update Collision Shapes which Interactors are colliding with.
	 * Adds new ones to existing Collision Shapes.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void AddCollisionShapes(TArray<UShapeComponent*> const NewCollisionShapes)
	{
		if(NewCollisionShapes.Num())
		{
			for (const auto Itr : NewCollisionShapes)
			{
				AddCollisionShape(Itr);
			}
		}
	}
	
	/**
	 * Helper function to update Collision Shapes which Interactors is colliding with.
	 * Adds new one to existing Collision Shapes.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void AddCollisionShape(UShapeComponent* NewCollisionShape)
	{
		// Do not try to add already included one
		if (NewCollisionShape && !CollisionShapes.Contains(NewCollisionShape))
		{
			CollisionShapes.Add(NewCollisionShape);

			UpdateCollisionChannel(NewCollisionShape);
			BindCollisionEvents(NewCollisionShape);
		}
	}

	/**
	 * Helper function to remove a specific Collision Shape from Collision Shapes.
	 * Removes the Collision Shape from the Array.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void RemoveCollisionShape(UShapeComponent* RemoveCollisionShape)
	{
		if(RemoveCollisionShape)
		{
			if(CollisionShapes.Contains(RemoveCollisionShape))
			{
				UnbindCollisionEvents(RemoveCollisionShape);
				
				CollisionShapes.Remove(RemoveCollisionShape);	
			}
		}
	}
	
	/**
	 * Helper function to remove a list of specific Collision Shapes from Collision Shapes.
	 * Removes the Collision Shapes from the Array.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void RemoveCollisionShapes(TArray<UShapeComponent*> const RemoveCollisionShapes)
	{
		if(RemoveCollisionShapes.Num())
		{
			for (const auto Itr : RemoveCollisionShapes)
			{
				RemoveCollisionShape(Itr);
			}
		}
	}

	/**
	 * Returns selected StencilID which is used to highlight Primitive Components.
	 * @return Stencil ID
	 */
	UFUNCTION(BlueprintCallable,Category="Interaction|Getters")
	int32 GetInteractionHighlightStencilID () const {return StencilID; };

	/**
	 * Helper function to set a new Stencil ID value.
	 * This value will be used to Highlight Primitive Components using Post-Process volume material.
	 * @note Value is clamped between 0 and 255 for Engine limits.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetInteractionHighlightStencilID(const int32 NewStencilID)
	{
		StencilID = FMath::Clamp(NewStencilID, 0, 255);
	}

	/**
	 * Returns a list of Highlightable Meshes (Static or Skeletal) if there are any.
	 * @return List of Highlightables
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Getters")
	TArray<UPrimitiveComponent*> GetHighlightableMeshes() const {return ListOfHighlightables; };

	/**
	 * Returns whether Searched Component is within the List of Highlightable Meshes.
	 * @return	True if within the List of Highlightable Meshes, false if Searched is nullptr or if not withing the Array.
	 */
	UFUNCTION(BlueprintCallable,Category="Interaction|Getters")
	bool FindHighlightableMesh(UPrimitiveComponent* const SearchedComponent) const
	{
		if (SearchedComponent == nullptr)
		{
			return false;
		}
		
		return ListOfHighlightables.Contains(SearchedComponent);
	};

	/**
	 * Returns Primitive Component at given Index (if exists). Otherwise returns nullptr.
	 * @return	Primitive Component from Array, nullptr if invalid index.
	 */
	UFUNCTION(BlueprintCallable,Category="Interaction|Getters")
	UPrimitiveComponent* FindHighlightableMeshAtIndex(const int32 Index) const
	{
		if(ListOfHighlightables.IsValidIndex(Index))
		{
			return ListOfHighlightables[Index];
		}

		return nullptr;
	};

	/**
	 * Helper function to set Highlightable Meshes which are highlighted if Interaction is happening.
	 * Overrides existing Highlightable Meshes and sets the new ones.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void SetHighlightableMeshes(const TArray<UPrimitiveComponent*> NewHighlightableMeshes)
	{
		RemoveHighlightableMeshes(NewHighlightableMeshes);

		AddHighlightableMeshes(NewHighlightableMeshes);
	}

	/**
	 * Helper function to add a Highlightable Meshes which are highlighted if Interaction is happening.
	 * Adds new ones to existing Highlightable Meshes.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void AddHighlightableMeshes(TArray<UPrimitiveComponent*> const NewHighlightableMeshes)
	{
		if(NewHighlightableMeshes.Num())
		{
			for (const auto Itr : NewHighlightableMeshes)
			{
				AddHighlightableMesh(Itr);
			}
		}
	}

	/**
	 * Helper function to add a Highlightable Mesh which is highlighted if Interaction is happening.
	 * Adds new one to existing Highlightable Meshes.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void AddHighlightableMesh(UPrimitiveComponent* NewHighlightableMesh)
	{
		// Do not try to add already included one
		if (NewHighlightableMesh && !ListOfHighlightables.Contains(NewHighlightableMesh))
		{
			ListOfHighlightables.Add(NewHighlightableMesh);
		}
	}

	/**
	 * Helper function to remove a specific Highlightable Mesh from Highlightable Meshes.
	 * Removes the Highlightable Mesh from the Array.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void RemoveHighlightableMesh(UPrimitiveComponent* RemoveHighlightableMesh)
	{
		if(RemoveHighlightableMesh)
		{
			if(ListOfHighlightables.Contains(RemoveHighlightableMesh))
			{				
				ListOfHighlightables.Remove(RemoveHighlightableMesh);	
			}
		}
	}

	/**
	 * Helper function to remove a specific Highlightable Meshes from Highlightable Meshes.
	 * Removes the Highlightable Meshes from the Array.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Setters")
	void RemoveHighlightableMeshes(TArray<UPrimitiveComponent*> const RemoveHighlightableMeshes)
	{
		if(RemoveHighlightableMeshes.Num())
		{
			for (const auto Itr : RemoveHighlightableMeshes)
			{
				RemoveHighlightableMesh(Itr);
			}
		}
	}
	
	/**
	 * Helper function which forces Collision Channels for all Collision shapes to reset to Plugin defaults.
	 */
	UFUNCTION(BlueprintCallable, Category="Inteaction|Initialize")
	void RefreshCollisionChannels() const
	{
		UpdateCollisionChannels();
	}

	#pragma endregion Getters_Setters

	#pragma region Validation
	UFUNCTION(BlueprintCallable, Category="Interaction|Validation")
	FORCEINLINE	bool IsInUse() const
	{
		if (GetInteractionState() != EInteractableState::EIS_Active) return false;
		if (GetWorld() == nullptr) return false; 
		// if (InteractionWidget == nullptr) return false;
		if (GetInteractorComponent() == nullptr) return false;

		return true;
	}

	UFUNCTION(BlueprintCallable, Category="Interaction|Validation")
	FORCEINLINE bool CanInteract() const
	{
		if (GetInteractionState() != EInteractableState::EIS_Standby) return false;
		if (GetWorld() == nullptr) return false; 
		// if (InteractionWidget == nullptr) return false;
		if (GetInteractorComponent() == nullptr) return false;

		return true;
	}	

	#pragma endregion Validation

	/**
	 * Initialization function responsible for creating User Widget instance from Widget User class.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Initialize")
	void InitializeInteractionComponent();
	
	UFUNCTION()
	void StartInteraction(float TimeKeyPressed);

	UFUNCTION()
	void StopInteraction(float TimeKeyReleased);

	UFUNCTION()
	void FinishInteraction(float TimeInteractionFinished);

	UFUNCTION()
	void CancelInteraction(UActorInteractableComponent* Component);

	/**
	 * A simple function that should be called when Interactable is requested to be activated and start receiving inputs from Interactor Component.
	 * @param ErrorMessage	[OUT]	Error message that will be filled if results is false. Otherwise is empty. Is always cleared upon retrieval.
	 * @return	Returns true if all required checks are OK, otherwise returns false with ErrorMessage which contains explanation.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Activate")
	bool ActivateInteractable(FString& ErrorMessage);

	/**
	 * A simple function that should be called when Interactable is requested to be deactivated and stop receiving inputs from Interactor Component.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|Activate")
	void DeactivateInteractable();

	/**
	 * Helper function that should update Interactable User Widget every time that text might have changed.
	 */
	UFUNCTION(BlueprintCallable, Category="Interaction|UI")
	void UpdateInteractableWidget() const;

public:

	#pragma region Events
	
	/**
	 * Delegate called after Interaction is finished.
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction|Events")
	FOnInteractionCompleted OnInteractionCompleted;

	/**
	 * Delegate called after Interaction has started.
	 * Must be implemented in Blueprint.
	 * @see https://sites.google.com/view/dominikpavlicek/home/documentation
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction|Events")
	FOnInteractionStarted OnInteractionStarted;

	/**
	 * Must be implemented in Blueprint.
	 * @see https://sites.google.com/view/dominikpavlicek/home/documentation
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction|Events")
	FOnInteractionStopped OnInteractionStopped;

	/**
	 * Delegate called after Interactor Component is found, either by Overlapping or by tracing.
	 * @param InteractingComponent	Value of Interactor Component which has been found
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction|Events")
	FOnInteractorFound OnInteractorFound;

	/**
	 * Delegate called after Interactor Component is lost, either way by stopped Overlapping or by stopping tracing.
	 * @param Interactor	Value of Interactor Component that has been lost
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction|Events")
	FOnInteractorLost OnInteractorLost;

	/**
	 * Delegate called after Interactor Component has traced this Interactable
	 * @param Interactor	Value of Interactor Component that has traced this Interactable
	 */
	UPROPERTY(BlueprintAssignable, Category="Interaction|Events")
	FOnInteractableTraced OnInteractableTraced;

	#pragma endregion Events

	#pragma region Properties
	
protected:
	
	// Timer Handle responsible for calling callback function after Interaction Time passes
	FTimerHandle TimerHandle_InteractionTime = FTimerHandle();

	// Timer Handle responsible for calling callback function after Cooldown Time passes
	FTimerHandle TimerHandle_CooldownTime = FTimerHandle();;

	UPROPERTY(VisibleAnywhere, Category="Interaction|Debug")
	UActorInteractableWidget* InteractionWidget = nullptr;

protected:
	
	// TODO: replicate
	/** Defines whether Activation must be done or is turned on by default.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings")
	uint8 bInteractableAutoActivate : 1;

	// TODO: replicate
	/** Type of interaction this Interactable Actor required to finish the interaction process.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings")
	EInteractableType InteractionType = EInteractableType::EIT_Hold;

	// TODO: replicate
	/**
	 * Defines which Collision Channel is used for Interaction Collision Tracing.
	 *
	 * @note Check Collision tab in Interaction -> Settings -> Interaction Radius Box Collision -> Collision and set the following if is not:
	 * - channel: Block
	 * - Pawn: Overlap
	 * - Others: Ignore or Overlap
	 * 
	 * This channel will be set to Block trace and generating a hit event.
	 * Can use custom Collision Channel.
	 * Default Channel: ECC_Visibility.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings|Collision")
	TEnumAsByte<ECollisionChannel> InteractableCollisionChannel = ECC_Visibility;

	// TODO: replicate
	/**
	 * List of collision shapes the Interactor is tracing against.
	 * Keep in mind that those collision shapes will get their collision preset overwritten by this Interactable component!
	 *
	 * List is checked whether Collision Shapes do have the same Owning Component as Interactable Component.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction|Settings", Replicated)
	TArray<UShapeComponent*> CollisionShapes;
	
	// TODO: replicate
	/** Time it takes to finish interaction.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings|Time", meta=(Units = "s", UIMin = 0, ClampMin = 0, EditCondition="InteractionType != EInteractableType::EIT_Press"))
	float InteractionTime = 1.5f;

	/**
	 * Defines whether Interactable should be highlighted with defined Material when Interaction is possible.
	 *
	 * @note For this option to work following steps must be done:
	 * - In Level must be PostProcessVolume
	 * - PostProcessVolume must have set PostProcessMaterials
	 * - - Custom Depth-Stencil Pass must be enabled
	 * - - - Projects Settings -> Engine -> Rendering -> PostProcessing
	 * - - - - Set to value: Enabled with Stencil
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings|Highlight")
	uint8 bInteractionHighlight : 1;

	// TODO: Getter Setter
	/**
	 * Defined what Stencil ID should be used to highlight the Primitive Mesh Components.
	 * In order to smoothly integrate with other logic, keep this ID unique!
	 * Default: 133
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="Interaction|Settings|Highlight", meta=(EditCondition="bInteractionHighlight == true", UIMin=0, ClampMin=0, UIMax=255, ClampMax=255))
	int32 StencilID = 133;

	// TODO: Getter Setter
	/**
	 * List of Primitive Components (Static and Skeletal Meshes) that will be highlighted when selected.
	 * If left empty, Owning Component will be used instead.
	 *
	 * List is checked whether Highlightables do have the same Owning Component as Interactable Component. 
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,  Category="Interaction|Settings|Highlight")
	TArray<UPrimitiveComponent*> ListOfHighlightables;

	// TODO: replicate
	/** Defined whether Interactable should be used only once or multiple times.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings|Lifecycle")
	EInteractableLifecycle InteractableLifecycle = EInteractableLifecycle::EIL_Cycled;

	// TODO: replicate
	/**
	 * How long it takes (in seconds) until next interaction can be performed.
	 * @note	0 means no Interaction Cooldown is applied.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings|Lifecycle", meta=(Units = "s", UIMin = 0, ClampMin = 0, EditCondition="InteractableLifecycle == EInteractableLifecycle::EIL_Cycled"))
	float InteractionCooldown = 5.f;

	// TODO: replicate
	/**
	 * How many times interaction can be performed.
	 * @note 0 means infinite times.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings|Lifecycle", meta=(UIMin = 0, ClampMin = 0, EditCondition="InteractableLifecycle == EInteractableLifecycle::EIL_Cycled"))
	int32 InteractionCyclesAllowed = 0;
	
	/** User Widget class that will be forced to use.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings|Display")
	TSubclassOf<UActorInteractableWidget> InteractableWidgetClass;
	
	/** Value of the Interaction Action Title.
	 * Could be "A Book", "Open Door" or anything short and descriptive.
	 * @note If empty, nothing will be displayed by default.
 	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings|Display")
	FText InteractionActionTitle = LOCTEXT("InteractionActionTitle", "Object");

	/** Value of the Interaction Action Text (Body).
	 * Could be "Hold to Open" or anything short and descriptive.
	 * @note If empty, nothing will be displayed by default.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings|Display")
	FText InteractionActionText = LOCTEXT("InteractionActionText", "Hold");

	/** Value of the Interaction Key.
	 * Could be "E" or any other Key.
	 * @note If empty, nothing will be displayed by default.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Interaction|Settings|Display")
	FText InteractionActionKey = LOCTEXT("InteractionActionKey", "E");

	/**
	 * Editor only flag.
	 * If true, will display debug lines and boxes.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|Debug")
	uint8 bDebug : 1;

protected:
	
	// TODO: replicate
	/** Last Time Interactable was used.*/
	UPROPERTY()
	float LastInteractionTime = 0.f;

	// TODO: replicate
	/** How many times was interaction performed.*/
	UPROPERTY()
	int32 InteractionCounter = 0;

	// TODO: replicate
	/** State of Interactable.*/
	UPROPERTY()
	EInteractableState InteractableState = EInteractableState::EIS_Disabled;
	
	// TODO: replicate
	/** Interactor interacting with this Component*/
	UPROPERTY()
	UActorInteractorComponent* InteractingInteractorComponent = nullptr;
	
	#pragma endregion Properties
	
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnActorBeginsOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnActorStopsOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void InteractorFound(UActorInteractorComponent* FoundInteractor);

	UFUNCTION()
	void InteractorLost(UActorInteractorComponent* LostInteractor);

	UFUNCTION()
	void OnInteractorTraced(UActorInteractorComponent* TracingInteractor);

	UFUNCTION()
	void InteractorChanged(float TimeHappened);

	UFUNCTION(NetMulticast, Unreliable)
	void Client_SetMeshComponentsHighlight(const bool bShowHighlight);

	void SetMeshComponentsHighlight(const bool bShowHighlight);
	
	void OnWidgetClassChanged();

private:

	#pragma region Helpers
	
	void StopInteractionLink(UActorInteractorComponent* OtherComponent);
	void FinishInteraction_TimerFunction();
	void CooldownElapsed_TimerFunction();
	void UpdateCollisionChannels() const;
	void UpdateCollisionChannel(UShapeComponent* const CollisionShape) const;
	FORCEINLINE bool HasAuthority() const
	{
		return GetOwner() && GetOwner()->HasAuthority();
	}

	void BindCollisionEvents(UShapeComponent* const NewCollisionShape)
	{
		if(NewCollisionShape)
		{
			NewCollisionShape->OnComponentBeginOverlap.AddUniqueDynamic(this, &UActorInteractableComponent::OnActorBeginsOverlapping);
			NewCollisionShape->OnComponentEndOverlap.AddUniqueDynamic(this, &UActorInteractableComponent::OnActorStopsOverlapping);
		}
	}
	void UnbindCollisionEvents(UShapeComponent* const RemovedCollisionShape)
	{
		RemovedCollisionShape->OnComponentBeginOverlap.RemoveDynamic(this, &UActorInteractableComponent::OnActorBeginsOverlapping);
		RemovedCollisionShape->OnComponentEndOverlap.RemoveDynamic(this, &UActorInteractableComponent::OnActorStopsOverlapping);
	}

	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	#endif

	#pragma endregion Helper
};

#undef LOCTEXT_NAMESPACE
