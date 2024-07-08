﻿// All rights reserved Dominik Morse (Pavlicek) 2021

#pragma once

#include "ClassViewerFilter.h"


// Filter used in the class picker to only show non abstract children of class
class FActorInteractionClassViewerFilter : public IClassViewerFilter
{
public:
	// All children of these classes will be included unless filtered out by another setting.
	TSet<const UClass*> AllowedChildrenOfClasses;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		return !InClass->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}

private:
	// Disallowed class flags.
	EClassFlags DisallowedClassFlags = CLASS_Deprecated;
};