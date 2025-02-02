using UnrealBuildTool; 

public class ShadersModule: ModuleRules 

{ 

	public ShadersModule(ReadOnlyTargetRules Target) : base(Target) 

	{
		PublicDependencyModuleNames.AddRange(new string[] { "UtilityModule" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateIncludePaths.AddRange(new string[] 
		{
			"ShadersModule/Private"
		});
		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.Add("TargetPlatform");
		}
		PublicDependencyModuleNames.Add("Core");
		PublicDependencyModuleNames.Add("Engine");
		PublicDependencyModuleNames.Add("MaterialShaderQualitySettings");
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Renderer",
			"RenderCore",
			"RHI",
			"Projects",
			"UtilityModule"
		});
		
		if (Target.bBuildEditor == true)
		{

			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"UnrealEd",
					"MaterialUtilities",
					"SlateCore",
					"Slate",
					"UnrealEd",
					"MaterialUtilities",
				}
			);
		}
	} 

}