DOING:
Test packaged map for 5.6/5.4/5.5

Set up testing levels by plugin doc section
For example:
All set up minus UI
All set up minus VP
All set up minus RuntimeMap
Setup for testing manual loading data

# TODO:

**Missing/Poor features**:
---------------------------------------------------------------------------

**Otimizations**
---------------------------------------------------------------------------

**Test Execution**
Redo user test
--------------------------------------------------------------------------

**Testing Infra/Setup**
Set up testing levels by plugin doc section
For example:
All set up minus UI
All set up minus VP
All set up minus RuntimeMap
Setup for testing manual loading data
-----------------------------------------------------------------------------

**QOL Improvements**
Why is MapRoot a static mesh component? Check this
Review MapVisual Component whole setup, is it even adequate?
Address package warnings 
Update PropertyDisplayGenerator plugin
Review logs to unreal and map gen logs (Copilot)
Maybe not so many save confirmations to user when creating map widgets? Only one
Check names output by asset action should be based on origin asset name and not its type
----------------------------------------------------------------------------------

**Cleanup**
Cleanup files of plugin
Cleanup asset actions code (low prio, not necessary for shipping)
Cleanup example structs (low prio, not necessary)
-----------------------------------------------------------------------------------

**Features**
When batch editing change only the properties that were changed (is this feasible?)
Log ENUM in debug section of mapgen -> translated to log map gen and passed as a param to MapGen
-----------------------------------------------------------------------------