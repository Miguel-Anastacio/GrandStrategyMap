using System;
using System.IO;
using UnrealBuildTool;
 
public class MapGeneratorLibrary : ModuleRules
{
	public MapGeneratorLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
 
		// Add any macros that need to be set
		PublicDefinitions.Add("WITH_MYTHIRDPARTYLIBRARY=1");
 
		// Add any include paths for the plugin
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "MapGenerator"));
 
		// Add any import libraries or static libraries
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "MapGenerator", "MapGenerator.lib"));
	}
}