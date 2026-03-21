@echo off
REM ============================================================
REM Enhanced Project Package Script - Multi-Project
REM ============================================================
REM USAGE:
REM   PackageProjectAdv.bat [config.yaml]
REM   PackageProjectAdv.bat my_config.yaml
REM
REM CONFIG.YAML FORMAT:
REM   projects:
REM     - path: "C:/Dev/MyGame54/MyGame.uproject"
REM       name: "MyGame"
REM       unreal_engine_path: "C:/Programs/Unreal/UE_5.4"
REM     - path: "C:/Dev/MyGame55/MyGame.uproject"
REM       name: "MyGame"
REM       unreal_engine_path: "C:/Programs/Unreal/UE_5.5"
REM   output_directory: "C:/BuiltProjects"
REM   build_configuration: "Shipping"
REM   target_platform: "Win64"
REM   cook_all: false
REM ============================================================

setlocal enabledelayedexpansion

set "CONFIG_PATH=config_package_projects.yaml"
if not "%~1"=="" set "CONFIG_PATH=%~1"

if not exist "%CONFIG_PATH%" (
    echo Error: Config file not found at: %CONFIG_PATH%
    pause
    exit /b 1
)

echo ============================================================
echo Project Package Build Script
echo ============================================================
echo Config File: %CONFIG_PATH%
echo ============================================================
echo.

REM ============================================================
REM Parse YAML using PowerShell
REM Outputs prefixed lines:
REM   OUTPUT:   base output dir
REM   CONF:     build configuration
REM   PLATFORM: target platform
REM   COOKALL:  true/false
REM   PROJPATH: each project .uproject path
REM   PROJNAME: each project name (or AUTO)
REM   PROJUE:   each project's paired UE path
REM All three PROJ* lines are emitted together per project entry.
REM ============================================================
echo Parsing configuration file...
echo.

set "PROJ_COUNT=0"
set "BUILD_CONF=Shipping"
set "TARGET_PLATFORM=Win64"
set "COOK_ALL=false"

for /f "usebackq delims=" %%A in (`powershell -NoProfile -ExecutionPolicy Bypass -Command ^
    "$raw = Get-Content '%CONFIG_PATH%' -Raw;" ^
    "$outMatch  = [regex]::Match($raw, 'output_directory:\s*[\x27\x22]?([^\x27\x22\r\n]+)[\x27\x22]?');" ^
    "$confMatch = [regex]::Match($raw, 'build_configuration:\s*[\x27\x22]?([^\x27\x22\r\n]+)[\x27\x22]?');" ^
    "$platMatch = [regex]::Match($raw, 'target_platform:\s*[\x27\x22]?([^\x27\x22\r\n]+)[\x27\x22]?');" ^
    "$cookMatch = [regex]::Match($raw, 'cook_all:\s*([^\r\n]+)');" ^
    "if ($outMatch.Success)  { 'OUTPUT:'   + $outMatch.Groups[1].Value.Trim() };" ^
    "if ($confMatch.Success) { 'CONF:'     + $confMatch.Groups[1].Value.Trim() };" ^
    "if ($platMatch.Success) { 'PLATFORM:' + $platMatch.Groups[1].Value.Trim() };" ^
    "if ($cookMatch.Success) { 'COOKALL:'  + $cookMatch.Groups[1].Value.Trim() };" ^
    "$inProj = $false; $pendingPath = ''; $pendingName = ''; $pendingUE = '';" ^
    "function Flush { if ($pendingPath -ne '') { 'PROJPATH:' + $pendingPath; if ($pendingName -eq '') { 'PROJNAME:AUTO' } else { 'PROJNAME:' + $pendingName }; if ($pendingUE -ne '') { 'PROJUE:' + $pendingUE } else { 'PROJUE:MISSING' } } };" ^
    "foreach ($line in $raw -split '\r?\n') { if ($line -match '^\s*#') { continue };" ^
    "  if ($line -match 'projects\s*:') { $inProj = $true; continue };" ^
    "  if ($inProj -and $line -match '^\s*-\s*path\s*:\s*[\x27\x22]([^\x27\x22\r\n]+)[\x27\x22]') { Flush; $pendingPath = $matches[1].Trim(); $pendingName = ''; $pendingUE = ''; continue };" ^
    "  if ($inProj -and $line -match '^\s*name\s*:\s*[\x27\x22]?([^\x27\x22\r\n]*)[\x27\x22]?') { $pendingName = $matches[1].Trim(); continue };" ^
    "  if ($inProj -and $line -match 'unreal_engine_path\s*:\s*[\x27\x22]([^\x27\x22\r\n]+)[\x27\x22]') { $pendingUE = $matches[1].Trim(); continue };" ^
    "  if ($line -match '^[a-zA-Z]') { $inProj = $false; Flush; $pendingPath = '' } };" ^
    "Flush"`) do (
    set "LINE=%%A"
    if "!LINE:~0,7!"=="OUTPUT:"   set "BASE_OUTPUT_PATH=!LINE:~7!"
    if "!LINE:~0,5!"=="CONF:"     set "BUILD_CONF=!LINE:~5!"
    if "!LINE:~0,9!"=="PLATFORM:" set "TARGET_PLATFORM=!LINE:~9!"
    if "!LINE:~0,8!"=="COOKALL:"  set "COOK_ALL=!LINE:~8!"
    if "!LINE:~0,9!"=="PROJPATH:" (
        set /a PROJ_COUNT+=1
        set "PROJ_PATH_!PROJ_COUNT!=!LINE:~9!"
    )
    if "!LINE:~0,9!"=="PROJNAME:" set "PROJ_NAME_!PROJ_COUNT!=!LINE:~9!"
    if "!LINE:~0,7!"=="PROJUE:"   set "PROJ_UE_!PROJ_COUNT!=!LINE:~7!"
)

REM Validate
if not defined BASE_OUTPUT_PATH (
    echo Error: Could not parse output_directory from config
    pause & exit /b 1
)
if %PROJ_COUNT%==0 (
    echo Error: No projects found in config
    pause & exit /b 1
)

REM Resolve AUTO names from filename
for /l %%i in (1, 1, %PROJ_COUNT%) do (
    if "!PROJ_NAME_%%i!"=="AUTO" (
        for %%f in ("!PROJ_PATH_%%i!") do set "PROJ_NAME_%%i=%%~nf"
    )
    if not defined PROJ_NAME_%%i (
        for %%f in ("!PROJ_PATH_%%i!") do set "PROJ_NAME_%%i=%%~nf"
    )
)

REM Print summary
echo Projects to build (%PROJ_COUNT%):
for /l %%i in (1, 1, %PROJ_COUNT%) do (
    for %%f in ("!PROJ_UE_%%i!") do set "VER_%%i=%%~nxf"
    echo   [%%i] !PROJ_NAME_%%i! ^(!VER_%%i!^) -- !PROJ_PATH_%%i!
)
echo.
echo Build Config : %BUILD_CONF%
echo Platform     : %TARGET_PLATFORM%
echo Cook All     : %COOK_ALL%
echo Output Dir   : %BASE_OUTPUT_PATH%
echo.
echo ============================================================
echo.

set "TOTAL_SUCCESS=0"
set "TOTAL_FAILED=0"

for /l %%i in (1, 1, %PROJ_COUNT%) do (
    call :package_project "!PROJ_PATH_%%i!" "!PROJ_NAME_%%i!" "!PROJ_UE_%%i!" %%i
)

goto :summary

REM ============================================================
REM Subroutine: package_project
REM %1 = .uproject path
REM %2 = project display name
REM %3 = UE install path
REM %4 = project index
REM ============================================================
:package_project
set "UPROJECT_PATH=%~1"
set "PROJECT_NAME=%~2"
set "UE_PATH=%~3"
set "PROJ_IDX=%~4"

for %%f in ("%UE_PATH%") do set "UE_VERSION=%%~nxf"

echo.
echo ============================================================
echo [BUILD %PROJ_IDX% of %PROJ_COUNT%] %PROJECT_NAME% -- %UE_VERSION%
echo ============================================================
echo.

if not exist "%UPROJECT_PATH%" (
    echo ERROR: .uproject not found at: %UPROJECT_PATH%
    echo.
    set /a TOTAL_FAILED+=1
    exit /b 0
)

if "%UE_PATH%"=="MISSING" (
    echo ERROR: No unreal_engine_path specified for this project in config
    echo.
    set /a TOTAL_FAILED+=1
    exit /b 0
)

set "RUN_UAT=%UE_PATH%\Engine\Build\BatchFiles\RunUAT.bat"
if not exist "%RUN_UAT%" (
    echo ERROR: RunUAT.bat not found. Is %UE_VERSION% installed correctly?
    echo   Expected: %RUN_UAT%
    echo.
    set /a TOTAL_FAILED+=1
    exit /b 0
)

REM Output: BASE/ProjectName/UE_5.X/
set "OUTPUT_PATH=%BASE_OUTPUT_PATH%\%PROJECT_NAME%\%UE_VERSION%"

echo .uproject : %UPROJECT_PATH%
echo Engine    : %UE_PATH%
echo Output    : %OUTPUT_PATH%
echo.

REM [STEP 1] Prepare output directory
echo [STEP 1] Preparing output directory...
echo --------------------------------------------------------
if exist "%OUTPUT_PATH%" (
    echo Removing previous build...
    rmdir "%OUTPUT_PATH%" /s /q 2>nul
)
mkdir "%OUTPUT_PATH%" 2>nul
echo Created: %OUTPUT_PATH%
echo.

REM [STEP 2] Package
set "COOK_ARGS="
if /i "%COOK_ALL%"=="true" set "COOK_ARGS=-CookAll"

echo [STEP 2] Packaging with RunUAT...
echo --------------------------------------------------------
echo Config   : %BUILD_CONF%
echo Platform : %TARGET_PLATFORM%
echo.
echo This may take a while...
echo.

call "%RUN_UAT%" BuildCookRun ^
    -project="%UPROJECT_PATH%" ^
    -target=%PROJECT_NAME% ^
    -noP4 ^
    -platform=%TARGET_PLATFORM% ^
    -clientconfig=%BUILD_CONF% ^
    -serverconfig=%BUILD_CONF% ^
    -cook ^
    -build ^
    -stage ^
    -pak ^
    -archive ^
    -archivedirectory="%OUTPUT_PATH%" ^
    -nocompileeditor ^
    %COOK_ARGS%

if errorlevel 1 (
    echo.
    echo ERROR: Package FAILED for %PROJECT_NAME% ^| %UE_VERSION%
    echo.
    set /a TOTAL_FAILED+=1
    exit /b 0
)

echo.
echo [STEP 2] Package completed successfully!
echo.

REM [STEP 3] Verify output
echo [STEP 3] Verifying output...
echo --------------------------------------------------------
dir "%OUTPUT_PATH%" /b
echo.

REM [STEP 4] Create zip
echo [STEP 4] Creating zip...
echo --------------------------------------------------------
set "ZIP_NAME=%PROJECT_NAME%_%UE_VERSION%_%BUILD_CONF%.zip"
set "ZIP_PATH=%BASE_OUTPUT_PATH%\%PROJECT_NAME%\%ZIP_NAME%"

echo Creating: %ZIP_PATH%
powershell -command "Compress-Archive -Path '%OUTPUT_PATH%\*' -DestinationPath '%ZIP_PATH%' -Force"

if exist "%ZIP_PATH%" (
    echo   SUCCESS: %ZIP_NAME%
) else (
    echo   WARNING: Zip creation failed (build output still available at %OUTPUT_PATH%)
)
echo.

echo [SUCCESS] %PROJECT_NAME% ^| %UE_VERSION%
echo.
set /a TOTAL_SUCCESS+=1
exit /b 0

REM ============================================================
:summary
REM ============================================================
echo.
echo ============================================================
echo FINAL BUILD SUMMARY
echo ============================================================
echo.
echo Total Builds : %PROJ_COUNT%
echo Successful   : %TOTAL_SUCCESS%
echo Failed       : %TOTAL_FAILED%
echo.
echo Output structure:
echo   %BASE_OUTPUT_PATH%\
for /l %%i in (1, 1, %PROJ_COUNT%) do (
    for %%f in ("!PROJ_UE_%%i!") do (
        echo     !PROJ_NAME_%%i!\
        echo       %%~nxf\                               (packaged build)
        echo       !PROJ_NAME_%%i!_%%~nxf_%BUILD_CONF%.zip
    )
)
echo.

if %TOTAL_FAILED% equ 0 (
    echo [SUCCESS] All builds completed!
) else (
    echo [WARNING] %TOTAL_FAILED% build(s) failed. See details above.
)

echo.
echo ============================================================
pause
