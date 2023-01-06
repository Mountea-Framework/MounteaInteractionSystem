using UnrealBuildTool;
 
public class InteractionEditorNotifications : ModuleRules
{
	public InteractionEditorNotifications(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;
		
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