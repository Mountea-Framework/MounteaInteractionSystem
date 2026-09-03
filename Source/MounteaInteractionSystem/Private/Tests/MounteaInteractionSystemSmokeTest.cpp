// All rights reserved Dominik Morse (Pavlicek) 2024.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Helpers/MounteaInteractionSystemSettings.h"

// Proves the automation harness itself runs for this module before any interaction-flow
// tests are added on top of it. Run from the Editor: Window -> Test Automation -> Mountea.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInteractionSystemSettingsSmokeTest,
	"Mountea.InteractionSystem.Smoke.SettingsLoad",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInteractionSystemSettingsSmokeTest::RunTest(const FString& Parameters)
{
	const UMounteaInteractionSystemSettings* settings = GetDefault<UMounteaInteractionSystemSettings>();
	TestNotNull(TEXT("UMounteaInteractionSystemSettings::GetDefault() should never return null"), settings);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
