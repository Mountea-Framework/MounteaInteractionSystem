// All rights reserved Dominik Morse (Pavlicek) 2021

using System.IO;
using UnrealBuildTool;

public class ActorInteractionPluginEditor : ModuleRules
{
	public ActorInteractionPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;
		
		PrecompileForTargets = PrecompileTargetsType.None;
		bPrecompile = false;
		bUsePrecompiled = false;

		PublicDependencyModuleNames.AddRange
			(
				new string[]
				{
					"Core",
					"CoreUObject",
					
					"BlueprintGraph",
					
					"Engine",
					
					"KismetCompiler",
					"PropertyEditor",
					"UnrealEd",
					"Kismet"
				}
			);
		
		PrivateDependencyModuleNames.AddRange
		(
			new string[]
			{
				"ActorInteractionPlugin",
				"UnrealEd",
				"Projects",
				
				"Slate",
				"SlateCore",
				
				"AssetTools",
				
				"BlueprintGraph",
				"Kismet", 
				
				"WebBrowser",
				"HTTP", 
				"Json", 
				"JsonUtilities",
				
				"EditorStyle",
				"DeveloperSettings",
				
				"MainFrame",
				"ToolMenus",
				"InputCore",
				
				"UMG",
				
				"GameplayTags"
			}
		);
		
		
	}
}
