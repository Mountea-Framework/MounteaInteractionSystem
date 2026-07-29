// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "Modules/ModuleManager.h"

class FMounteaInteractionPinFactory;

DECLARE_LOG_CATEGORY_EXTERN(MounteaInteractionSystemDeveloper, All, All);

class FMounteaInteractionSystemDeveloper : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#if WITH_EDITOR
private:

	TSharedPtr<FMounteaInteractionPinFactory> PinFactory;
#endif
};
