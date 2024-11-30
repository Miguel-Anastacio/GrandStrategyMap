// Copyright Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class MapEditor : ModuleRules
{
	public MapEditor(ReadOnlyTargetRules Target) : base(Target)
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
				"VictoryBPLibrary"
             
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"InputCore",
                "Slate",	
				"SlateCore",
                "UMG",
				"Json",
				"JsonUtilities",
				"AssetTools",
				"AssetDefinition",
				"AdvancedPreviewScene",
				"ToolMenus",
				"UnrealEd"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "MapGenerator", "MapGeneratord.lib"));
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
