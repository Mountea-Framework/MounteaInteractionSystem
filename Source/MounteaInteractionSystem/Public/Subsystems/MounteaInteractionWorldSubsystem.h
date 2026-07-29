// All rights reserved Dominik Morse (Pavlicek) 2024.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Helpers/MounteaInteractionHelpers.h"
#include "MounteaInteractionWorldSubsystem.generated.h"

class UWidgetComponent;
class UUserWidget;
class IActorInteractionWidget;

/**
 * Pooled attention widget entry.
 * Each entry is a spawned actor that hosts a single UWidgetComponent for the attention indicator.
 * Idle entries are hidden and repositioned when assigned to a focused actor.
 */
USTRUCT()
struct FAttentionWidgetPoolEntry
{
	GENERATED_BODY()

	/** The spawned actor that owns the widget component. */
	UPROPERTY()
	TObjectPtr<AActor> WidgetActor = nullptr;

	/** The widget component on WidgetActor. */
	UPROPERTY()
	TObjectPtr<UWidgetComponent> WidgetComponent = nullptr;

	/** Actor currently assigned to this slot, or null if idle. */
	UPROPERTY()
	TWeakObjectPtr<AActor> AssignedActor;

	bool IsIdle() const { return !AssignedActor.IsValid(); }
};

/**
 * WorldSubsystem that manages a pool of attention widget actors.
 *
 * Only the focused actor gets an attention widget assigned from the pool.
 * When focus shifts or the actor enters InteractionRange, the widget returns to pool.
 * Pool size is read from UMounteaInteractorUIConfig::AttentionWidgetPoolSize at Initialize().
 */
UCLASS()
class MOUNTEAINTERACTIONSYSTEM_API UMounteaInteractionWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/**
	 * Assigns an idle pool entry to the given actor, positioning the widget above it.
	 * Returns false if the pool is exhausted (all entries occupied).
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Subsystem")
	bool AssignAttentionWidget(AActor* FocusedActor);

	/**
	 * Returns the attention widget for Actor to the pool and hides it.
	 * No-op if Actor has no assigned entry.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Subsystem")
	void ReleaseAttentionWidget(AActor* Actor);

	/**
	 * Updates the EInteractionWidgetMode on the widget assigned to Actor.
	 * Use to transition between Attention and Interaction modes without reassigning the actor.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Subsystem")
	void UpdateAttentionWidgetMode(AActor* Actor, EInteractionWidgetMode NewMode);

	/** Returns the widget instance assigned to Actor, or null if none. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Interaction|Subsystem")
	UUserWidget* GetWidgetForActor(const AActor* Actor) const;

	/** Number of idle (available) pool entries. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Subsystem")
	int32 GetIdlePoolCount() const;

	/** Total pool size (idle + occupied). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Interaction|Subsystem")
	int32 GetTotalPoolSize() const { return AttentionWidgetPool.Num(); }

private:

	void SpawnPoolEntries(int32 Count, TSubclassOf<UUserWidget> WidgetClass);
	FAttentionWidgetPoolEntry* FindEntryForActor(const AActor* Actor);
	const FAttentionWidgetPoolEntry* FindEntryForActor(const AActor* Actor) const;

	UPROPERTY()
	TArray<FAttentionWidgetPoolEntry> AttentionWidgetPool;
};
