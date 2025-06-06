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
				"ApplicationCore",
				"RHI",
				"RenderCore",
				"UtilityModule",
				"UtilityModuleEditor",
				"StructUtils",
				"SharedModule", 
				"Engine"
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
				"UnrealEd",
				"EditorStyle",
				"WorkspaceMenuStructure",
				"PropertyEditor",
				"MapGeneratorLibrary"
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
