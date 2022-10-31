// All rights reserved Dominik Pavlicek 2022.

using System.IO;
using UnrealBuildTool;

public class ActorInteractionPluginEditor : ModuleRules
{
	public ActorInteractionPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UnrealEd"});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ActorInteractionPlugin",
				"UnrealEd",
				"Projects",
				
				"Slate",
				"SlateCore",
				
				"AssetTools",
				
				"BlueprintGraph",
				"Kismet"
			}
		);
		
		
	}
}
