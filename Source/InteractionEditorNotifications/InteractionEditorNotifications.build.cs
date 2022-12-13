using UnrealBuildTool;
 
public class InteractionEditorNotifications : ModuleRules
{
	public InteractionEditorNotifications(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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