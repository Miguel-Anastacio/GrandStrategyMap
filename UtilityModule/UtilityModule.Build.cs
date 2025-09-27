// Copyright Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class UtilityModule : ModuleRules
{
	public UtilityModule(ReadOnlyTargetRules Target) : base(Target)
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
				"RenderCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UtilityModuleEditor"
					// ... add private dependencies that you statically link with here ...	
				}
			);
		}

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"InputCore",
				"Json",
				"JsonUtilities",
				"Slate",
				"SlateCore",
				"UMG",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		if (Target.Version.MajorVersion <= 5 && Target.Version.MinorVersion <= 4)
		{
			PublicDependencyModuleNames.Add("StructUtils");
		}

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
