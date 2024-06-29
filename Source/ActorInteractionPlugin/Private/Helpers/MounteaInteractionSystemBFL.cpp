// Copyright Dominik Morse (Pavlicek) 2024. All Rights Reserved.


#include "Helpers/MounteaInteractionSystemBFL.h"

UMeshComponent* UMounteaInteractionSystemBFL::FindMeshByTag(const FName Tag, const AActor* Source)
{
	if (!Source) return nullptr;

	TArray<UMeshComponent*> MeshComponents;
	Source->GetComponents(MeshComponents);

	for (const auto& Itr : MeshComponents)
	{
		if (Itr && Itr->ComponentHasTag(Tag))
		{
			return Itr;
		}
	}

	return nullptr;
}

UMeshComponent* UMounteaInteractionSystemBFL::FindMeshByName(const FName Name, const AActor* Source)
{
	if (!Source) return nullptr;

	TArray<UMeshComponent*> MeshComponents;
	Source->GetComponents(MeshComponents);

	for (const auto& Itr : MeshComponents)
	{
		if (Itr && Itr->GetName().Equals(Name.ToString()))
		{
			return Itr;
		}
	}

	return nullptr;
}

UPrimitiveComponent* UMounteaInteractionSystemBFL::FindPrimitiveByTag(const FName Tag, const AActor* Source)
{
	if (!Source) return nullptr;

	TArray<UPrimitiveComponent*> MeshComponents;
	Source->GetComponents(MeshComponents);

	for (const auto& Itr : MeshComponents)
	{
		if (Itr && Itr->ComponentHasTag(Tag))
		{
			return Itr;
		}
	}

	return nullptr;
}

UPrimitiveComponent* UMounteaInteractionSystemBFL::FindPrimitiveByName(const FName Name, const AActor* Source)
{
	if (!Source) return nullptr;

	TArray<UPrimitiveComponent*> MeshComponents;
	Source->GetComponents(MeshComponents);

	for (const auto& Itr : MeshComponents)
	{
		if (Itr && Itr->GetName().Equals(Name.ToString()))
		{
			return Itr;
		}
	}

	return nullptr;
}

UActorInteractionPluginSettings* UMounteaInteractionSystemBFL::GetInteractionSystemSettings()
{
	return GetMutableDefault<UActorInteractionPluginSettings>();
}
