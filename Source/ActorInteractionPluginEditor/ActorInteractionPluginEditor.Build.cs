// All rights reserved Dominik Pavlicek 2022.

using System.IO;
using UnrealBuildTool;

public class ActorInteractionPluginEditor : ModuleRules
{
	public ActorInteractionPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UnrealEd", "Slate", "SlateCore", "Projects"});
	}
}
