// Copyright Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class InteractiveMap : ModuleRules
{
	public InteractiveMap(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"RHI",
				"RenderCore",
				"UtilityModule",
				"SharedModule",
				"StructUtils",
				"UMG", 
				"ShadersModule",
				"DataBasedWidget"
             
				// ... add other public dependencies that you statically link with here ...
			}
			);
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UMGEditor",
					"UnrealEd"
					// ... add private dependencies that you statically link with here ...	
				}
			);
		}

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "EnhancedInput",
                "Slate",	
				"SlateCore",
                "UMG",
				"Json",
				"JsonUtilities",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
