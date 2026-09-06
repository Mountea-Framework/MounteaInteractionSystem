// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Subsystems/MounteaInteractionWorldSubsystem.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Interfaces/MounteaInteractionWidget.h"
#include "Helpers/MounteaInteractionSystemSettings.h"
#include "Settings/MounteaInteractorUIConfig.h"
#include "Helpers/MounteaInteractionSystemLog.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionWorldSubsystem"

// Constant "float above origin" height for the pooled attention widget — origin-to-head-height
// varies too much across possible interactable actors (props vs. characters) to derive
// automatically; a fixed offset is the simplest correct default for a first pass.
static constexpr float AttentionWidgetZOffset = 100.f;

bool UMounteaInteractionWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
				return false;
	// Don't create in editor preview worlds
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		return World->IsGameWorld();
	}
	return false;
}

void UMounteaInteractionWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	int32 PoolSize = 5;
	TSubclassOf<UUserWidget> AttentionWidgetClass = nullptr;

	if (const UMounteaInteractionSystemSettings* Settings = GetDefault<UMounteaInteractionSystemSettings>())
	{
		if (const UMounteaInteractorUIConfig* UIConfig = Settings->GetInteractorUIConfig())
		{
			PoolSize = FMath::Max(1, UIConfig->AttentionWidgetPoolSize);
			AttentionWidgetClass = UIConfig->DefaultAttentionWidgetClass.LoadSynchronous();
		}
	}

	SpawnPoolEntries(PoolSize, AttentionWidgetClass);

	LOG_INFO(TEXT("[MounteaInteractionWorldSubsystem] Initialized with pool size %d"), PoolSize);
}

void UMounteaInteractionWorldSubsystem::Deinitialize()
{
	for (FAttentionWidgetPoolEntry& Entry : AttentionWidgetPool)
	{
		if (IsValid(Entry.WidgetActor))
		{
			Entry.WidgetActor->Destroy();
		}
	}
	AttentionWidgetPool.Empty();

	Super::Deinitialize();
}

bool UMounteaInteractionWorldSubsystem::AssignAttentionWidget(AActor* FocusedActor)
{
	if (!IsValid(FocusedActor))
				return false;

	// Already assigned?
	if (FindEntryForActor(FocusedActor))
				return true;

	// Find idle entry
	for (FAttentionWidgetPoolEntry& Entry : AttentionWidgetPool)
	{
		if (!Entry.IsIdle())
				continue;

		Entry.AssignedActor = FocusedActor;

		if (IsValid(Entry.WidgetActor))
		{
			// Attach (not a one-shot SetActorLocation, and not a Tick) so the widget keeps
			// following FocusedActor — including under root motion/physics movement — at zero
			// ongoing cost. This is per-client local work: each client's own local Subsystem
			// instance attaches its own pool actor in its own world, nothing here replicates.
			Entry.WidgetActor->AttachToActor(FocusedActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Entry.WidgetActor->SetActorHiddenInGame(false);
		}

		// Notify widget of mode
		if (IsValid(Entry.WidgetComponent))
		{
			if (UUserWidget* Widget = Entry.WidgetComponent->GetUserWidgetObject())
			{
				if (Widget->GetClass()->ImplementsInterface(UActorInteractionWidget::StaticClass()))
				{
					IActorInteractionWidget::Execute_SetWidgetMode(Widget, EInteractionWidgetMode::EWM_Attention);
				}
			}
		}

		return true;
	}

	LOG_WARNING(TEXT("[MounteaInteractionWorldSubsystem] Pool exhausted — cannot assign attention widget to %s"), *GetNameSafe(FocusedActor));
	return false;
}

void UMounteaInteractionWorldSubsystem::ReleaseAttentionWidget(AActor* Actor)
{
	if (!IsValid(Actor))
				return;

	if (FAttentionWidgetPoolEntry* Entry = FindEntryForActor(Actor))
	{
		Entry->AssignedActor.Reset();

		if (IsValid(Entry->WidgetActor))
		{
			Entry->WidgetActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			Entry->WidgetActor->SetActorHiddenInGame(true);
		}
	}
}

void UMounteaInteractionWorldSubsystem::UpdateAttentionWidgetMode(AActor* Actor, EInteractionWidgetMode NewMode)
{
	if (!IsValid(Actor))
				return;

	if (const FAttentionWidgetPoolEntry* Entry = FindEntryForActor(Actor))
	{
		if (IsValid(Entry->WidgetComponent))
		{
			if (UUserWidget* Widget = Entry->WidgetComponent->GetUserWidgetObject())
			{
				if (Widget->GetClass()->ImplementsInterface(UActorInteractionWidget::StaticClass()))
				{
					IActorInteractionWidget::Execute_SetWidgetMode(Widget, NewMode);
				}
			}
		}
	}
}

UUserWidget* UMounteaInteractionWorldSubsystem::GetWidgetForActor(const AActor* Actor) const
{
	if (!IsValid(Actor))
				return nullptr;

	if (const FAttentionWidgetPoolEntry* Entry = FindEntryForActor(Actor))
	{
		if (IsValid(Entry->WidgetComponent))
		{
			return Entry->WidgetComponent->GetUserWidgetObject();
		}
	}
	return nullptr;
}

int32 UMounteaInteractionWorldSubsystem::GetIdlePoolCount() const
{
	int32 Count = 0;
	for (const FAttentionWidgetPoolEntry& Entry : AttentionWidgetPool)
	{
		if (Entry.IsIdle()) ++Count;
	}
	return Count;
}

void UMounteaInteractionWorldSubsystem::SpawnPoolEntries(int32 Count, TSubclassOf<UUserWidget> WidgetClass)
{
	UWorld* World = GetWorld();
	if (!World)
				return;

	AttentionWidgetPool.Reserve(Count);

	for (int32 i = 0; i < Count; ++i)
	{
		FAttentionWidgetPoolEntry Entry;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Name = *FString::Printf(TEXT("MIS_AttentionWidgetActor_%d"), i);

		AActor* PoolActor = World->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (!IsValid(PoolActor))
				continue;

		PoolActor->SetActorHiddenInGame(true);

		UWidgetComponent* WidgetComp = NewObject<UWidgetComponent>(PoolActor, UWidgetComponent::StaticClass());
		WidgetComp->SetWidgetSpace(EWidgetSpace::World);
		WidgetComp->SetDrawAtDesiredSize(true);

		// "Float above head" offset lives here (once, at spawn) rather than being re-applied on
		// every AssignAttentionWidget call — the pool actor itself snaps exactly to the focused
		// actor's origin; this component's own relative transform is what lifts it above that.
		WidgetComp->SetRelativeLocation(FVector(0.f, 0.f, AttentionWidgetZOffset));

		if (WidgetClass)
		{
			WidgetComp->SetWidgetClass(WidgetClass);
		}

		// SetRootComponent() before RegisterComponent(): PoolActor is a bare AActor with no
		// existing root, and SetActorLocation()/AttachToActor() on the actor are no-ops without
		// one. Must happen before the component registers so it's recognized as the root from
		// the start rather than floating unattached at the world origin.
		PoolActor->SetRootComponent(WidgetComp);
		WidgetComp->RegisterComponent();
		PoolActor->AddInstanceComponent(WidgetComp);

		Entry.WidgetActor = PoolActor;
		Entry.WidgetComponent = WidgetComp;

		AttentionWidgetPool.Add(MoveTemp(Entry));
	}
}

FAttentionWidgetPoolEntry* UMounteaInteractionWorldSubsystem::FindEntryForActor(const AActor* Actor)
{
	for (FAttentionWidgetPoolEntry& Entry : AttentionWidgetPool)
	{
		if (Entry.AssignedActor.Get() == Actor)
				return &Entry;
	}
	return nullptr;
}

const FAttentionWidgetPoolEntry* UMounteaInteractionWorldSubsystem::FindEntryForActor(const AActor* Actor) const
{
	for (const FAttentionWidgetPoolEntry& Entry : AttentionWidgetPool)
	{
		if (Entry.AssignedActor.Get() == Actor)
				return &Entry;
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
