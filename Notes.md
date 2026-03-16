DOING:


TODO:

**Missing/Poor features**:
Loading your own data files and lookup texture -> Complete docs, is this even possible?
Review limits and camera movement for flat maps (when user goes to edge of screen)

**BUGS**:

Address RuntimeMap Issues:
    LogInteractiveMap: Warning: Dynamic Texture tried to Update Texture but Data is Null
    LogInteractiveMap: Error: Pixel Map does not match lookup
    LogInteractiveMap: Error: Pixel Map size 229
    LogInteractiveMap: Error: Lookup size 223
---------------------------------------------------------------------------

**Test Execution**
Test packaged map for 5.6/5.4/5.5
Redo user test after changes
--------------------------------------------------------------------------

**Otimizations**
Check huge file size for mapobjecta
Use softobject references whenever possible for game runtime relevant code
Why is MapRoot a static mesh component? Check this
Review MapVisual Component whole setup is it even adequate?
---------------------------------------------------------------------------

**Testing Infra/Setup**
Set up testing levels by plugin doc section
For example:
All set up minus UI
All set up minus VP
All set up minus RuntimeMap
Setup for testing manual loading data
-----------------------------------------------------------------------------

**QOL Improvements**
Address package warnings 
Update PropertyDisplayGenerator plugin
Review logs to unreal and map gen logs (Copilot)
Maybe not so many save confirmations to user? Only one
Check names output by asset action should be based on origin asset name and not its type
----------------------------------------------------------------------------------

**Cleanup**
Cleanup example structs
Cleanup files of plugin
Cleanup asset actions code (low prio, not necessary for shipping)
-----------------------------------------------------------------------------------

**Features**
When batch editing change only the properties that were changed (is this feasible?)
Log ENUM in debug section of mapgen -> translated to log map gen and passed as a param to MapGen
-----------------------------------------------------------------------------