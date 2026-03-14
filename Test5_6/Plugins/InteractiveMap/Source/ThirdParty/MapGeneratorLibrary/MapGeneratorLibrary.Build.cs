using System;
using System.IO;
using UnrealBuildTool;
 
public class MapGeneratorLibrary : ModuleRules
{
	public MapGeneratorLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		PublicIncludePaths.Add(ModuleDirectory);  // This points to the MapGeneratorLibrary folder
		// Add any include paths for the plugin
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "MapGenerator"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "MapGenerator/source"));
 
		// Add any import libraries or static libraries
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "MapGenerator", "MapGenerator.lib"));
	}
}