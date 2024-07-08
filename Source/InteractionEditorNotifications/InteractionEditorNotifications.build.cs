// All rights reserved Dominik Morse (Pavlicek) 2021

using System.IO;
using UnrealBuildTool;
 
public class InteractionEditorNotifications : ModuleRules
{
	public InteractionEditorNotifications(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrecompileForTargets = PrecompileTargetsType.None;
		bPrecompile = false;
		bUsePrecompiled = false;

		PublicDependencyModuleNames.AddRange
			(new string[]
				{
					"Core", 
					"CoreUObject", 
					"Engine",
					"Slate", 
					"SlateCore", 
					"Projects"
				}
			);
	}
}