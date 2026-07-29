// All rights reserved Dominik Morse (Pavlicek) 2024

using UnrealBuildTool;

public class MounteaInteractionSystemDeveloper : ModuleRules
{
	public MounteaInteractionSystemDeveloper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"InputCore"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Projects",
				"Engine",
				"Slate",
				"SlateCore",
				"GameplayTags",

				"MounteaInteractionSystem"
			}
		);

		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"EditorStyle",
					"BlueprintGraph",
					"Kismet",
					"ApplicationCore",
					"UnrealEd",
					"GraphEditor",
					"KismetCompiler",
					"PropertyEditor",
					"ToolMenus",
					"AssetTools",
					"ContentBrowser"
				}
			);
		}
	}
}
