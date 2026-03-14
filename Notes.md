DOING:

TODO:
Test packaged map for 5.6

Fix build for 5.5 and 5.4
[84/104] Compile [x64] Module.MapEditor.cpp
C:\Dev\BuiltPlugins\UE_5.5\InteractiveMap\HostProject\Plugins\InteractiveMap\Source\Utils\UtilityModuleEditor\Public\BlueprintLibrary\WidgetEditorFunctionLibrary.h(45): error C2039: 'WidgetVariableNameToGuidMap': is not a member of 'UWidgetBlueprint'
C:\Program Files\Epic Games\UE_5.5\Engine\Source\Editor\UMGEditor\Public\WidgetBlueprint.h(218): note: see declaration of 'UWidgetBlueprint'
C:\Dev\BuiltPlugins\UE_5.5\InteractiveMap\HostProject\Plugins\InteractiveMap\Source\Utils\UtilityModuleEditor\Public\BlueprintLibrary\WidgetEditorFunctionLibrary.h(25): note: This diagnostic occurred in the compiler generated function 'void UAtkWidgetEditorFunctionLibrary::ClearChildren(UWidgetBlueprint *,T *)'
C:\Dev\BuiltPlugins\UE_5.5\InteractiveMap\HostProject\Plugins\InteractiveMap\Source\Utils\UtilityModuleEditor\Public\BlueprintLibrary\WidgetEditorFunctionLibrary.h(80): error C2039: 'WidgetVariableNameToGuidMap': is not a member of 'UWidgetBlueprint'
C:\Program Files\Epic Games\UE_5.5\Engine\Source\Editor\UMGEditor\Public\WidgetBlueprint.h(218): note: see declaration of 'UWidgetBlueprint'
C:\Dev\BuiltPlugins\UE_5.5\InteractiveMap\HostProject\Plugins\InteractiveMap\Source\Utils\UtilityModuleEditor\Public\BlueprintLibrary\WidgetEditorFunctionLibrary.h(66): note: This diagnostic occurred in the compiler generated function 'void UAtkWidgetEditorFunctionLibrary::CreateRootWidget(UUserWidget *,const FName)'

Recreate all broken assets in Unreal engine 5.4 version.

Restructrure repo to hold several uprojects for test plugin in different versions

Move dev software to 5.4 version


Set up testing levels by plugin doc section
For example:
All set up minus UI
All set up minus VP
All set up minus RuntimeMap
Redo user test after changes


Setup for testing manual loading data
Loading your own data files and lookup texture -> Complete docs, is this even possible?

Address RuntimeMap Issues - Check if this is still relevant?
    Check logs in InteractiveMap runtime
        Pixel Map does not match lookup

    LogInteractiveMap: Warning: Dynamic Texture tried to Update Texture but Data is Null
    LogInteractiveMap: Error: Pixel Map does not match lookup
    LogInteractiveMap: Warning: Property: Populatio

    LogInteractiveMap: Warning: Property: Country
    LogInteractiveMap: Warning: Dynamic Texture tried to Update Texture but Data is Null ()
    LogInteractiveMap: Warning: Dynamic Texture tried to Update Texture but Data is Null

Cleanup example structs
Update PropertyDisplayGenerator plugin
Cleanup files of plugin
MATERIALS, CLEAN THEM UP
Address package warnings 
Review logs to unreal and map gen logs (Copilot)




Cleanup asset actions code (low prio, not necessary for shipping)
Maybe not so many save confirmations to user? Only one

Future Features
-- When batch editing change only the properties that were changed (is this feasible)