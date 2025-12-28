// All rights reserved Dominik Morse (Pavlicek) 2024

using System.IO;
using UnrealBuildTool;

public class MounteaInteractionSystem : ModuleRules
{
	public MounteaInteractionSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange
		(
			new string[]
			{
				"Core",
				"UMG",
				"InputCore",
				"Engine"
			}
		);
		
		PrivateDependencyModuleNames.AddRange
		(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UMG",
				"Slate",
				"SlateCore",
				"Projects",
				"InputCore",
				"GameplayTags",
				"EnhancedInput",
				"NetCore",
				"EnhancedInput",
				"ApplicationCore",
				"CommonInput",

				"MounteaInteractionSystemEditorNotifications", "StructUtilsEditor",

#if UE_4_26_OR_LATER
				"DeveloperSettings",
#endif
			}
		);
	}
}