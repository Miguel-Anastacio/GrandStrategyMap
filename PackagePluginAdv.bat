@echo off
REM ============================================================
REM Enhanced Plugin Build Script - Multi-Version Support
REM ============================================================
REM USAGE:
REM   PackagePluginAdv.bat [--skip-map-gen]
REM
REM CONFIG.YAML FORMAT:
REM   plugin_path: "C:/Path/to/plugin.uplugin"
REM   unreal_engine_paths:
REM     - "C:/Programs/Unreal/UE_5.4"
REM     - "C:/Programs/Unreal/UE_5.5"
REM   output_directory: "C:/BuiltPlugins"
REM   test_project_paths:
REM     - "C:/Dev/TestProject54/Plugins/MyPlugin"
REM     - "C:/Dev/TestProject55/Plugins/MyPlugin"
REM ============================================================

setlocal enabledelayedexpansion

set "SKIP_MAP_GEN=false"
if "%~1"=="--skip-map-gen" set "SKIP_MAP_GEN=true"

set "CONFIG_PATH=config.yaml"

if not exist "%CONFIG_PATH%" (
    echo Error: Config file not found at: %CONFIG_PATH%
    pause
    exit /b 1
)

echo ============================================================
echo Plugin Package Build Script - Multi-Version Mode
echo ============================================================
echo Config File: %CONFIG_PATH%
echo ============================================================
echo.

REM ============================================================
REM Parse YAML configuration using inline PowerShell
REM Parses: plugin_path, output_directory, unreal_engine_paths, test_project_paths
REM Outputs prefixed lines: PLUGIN: OUTPUT: PATH: TEST:
REM ============================================================
echo Parsing configuration file...
echo.

set "UE_COUNT=0"
set "TEST_COUNT=0"

for /f "usebackq delims=" %%A in (`powershell -NoProfile -ExecutionPolicy Bypass -Command "$content = Get-Content '%CONFIG_PATH%' -Raw; $pluginMatch = [regex]::Match($content, 'plugin_path:\s*[\x27\x22]?([^\x27\x22\r\n]+)[\x27\x22]?'); $outputMatch = [regex]::Match($content, 'output_directory:\s*[\x27\x22]?([^\x27\x22\r\n]+)[\x27\x22]?'); if ($pluginMatch.Success) { 'PLUGIN:' + $pluginMatch.Groups[1].Value.Trim() }; if ($outputMatch.Success) { 'OUTPUT:' + $outputMatch.Groups[1].Value.Trim() }; $inUEPaths = $false; $inTestPaths = $false; foreach ($line in $content -split '\r?\n') { if ($line -match 'unreal_engine_paths\x3a') { $inUEPaths = $true; $inTestPaths = $false; continue }; if ($line -match 'test_project_paths\x3a') { $inTestPaths = $true; $inUEPaths = $false; continue }; if ($line -match '^\s*[a-zA-Z]') { $inUEPaths = $false; $inTestPaths = $false }; if ($inUEPaths -and $line -match '^\s*-\s*[\x27\x22]([^\x27\x22\r\n]+)[\x27\x22]') { 'PATH:' + $matches[1].Trim() }; if ($inTestPaths -and $line -match '^\s*-\s*[\x27\x22]([^\x27\x22\r\n]+)[\x27\x22]') { 'TEST:' + $matches[1].Trim() } }"`) do (
    set "LINE=%%A"
    if "!LINE:~0,7!"=="PLUGIN:" set "PLUGIN_PATH=!LINE:~7!"
    if "!LINE:~0,7!"=="OUTPUT:" set "BASE_OUTPUT_PATH=!LINE:~7!"
    if "!LINE:~0,5!"=="PATH:" (
        set /a UE_COUNT+=1
        set "UE_PATH_!UE_COUNT!=!LINE:~5!"
    )
    if "!LINE:~0,5!"=="TEST:" (
        set /a TEST_COUNT+=1
        set "TEST_PATH_!TEST_COUNT!=!LINE:~5!"
    )
)

if not defined PLUGIN_PATH (
    echo Error: Could not parse plugin_path from config file
    pause
    exit /b 1
)
if not exist "%PLUGIN_PATH%" (
    echo Error: Plugin file not found at: %PLUGIN_PATH%
    pause
    exit /b 1
)
if not defined BASE_OUTPUT_PATH (
    echo Error: Could not parse output_directory from config file
    pause
    exit /b 1
)
if %UE_COUNT%==0 (
    echo Error: No unreal_engine_paths found in config file
    pause
    exit /b 1
)

for %%f in ("%PLUGIN_PATH%") do (
    set "PLUGIN_NAME=%%~nf"
    set "PLUGIN_DIR=%%~dpf"
)

echo Plugin Path:  %PLUGIN_PATH%
echo Plugin Name:  %PLUGIN_NAME%
echo Plugin Dir:   %PLUGIN_DIR%
echo Output Dir:   %BASE_OUTPUT_PATH%
echo.
echo Found %UE_COUNT% Unreal Engine version(s):
for /l %%i in (1, 1, %UE_COUNT%) do echo   [%%i] !UE_PATH_%%i!
echo.
echo Found %TEST_COUNT% test project path(s):
for /l %%i in (1, 1, %TEST_COUNT%) do echo   [%%i] !TEST_PATH_%%i!
echo.
echo ============================================================
echo.

REM ============================================================
REM PRE-STEP: Build the MapGenerator static library
REM ============================================================
echo [PRE-STEP] Checking for MapGenerator static library...
echo --------------------------------------------------------
echo.

if /i "%SKIP_MAP_GEN%"=="true" (
    echo MapGenerator build skipped (--skip-map-gen flag passed)
    echo.
) else (
    set "MAP_GEN_SETUP=%PLUGIN_DIR%Source\ThirdParty\MapGeneratorLibrary\MapGenerator\setup.bat"
    if exist "!MAP_GEN_SETUP!" (
        echo Running: !MAP_GEN_SETUP!
        call "!MAP_GEN_SETUP!"
        if errorlevel 1 (
            echo ERROR: MapGenerator library build FAILED
            pause
            exit /b 1
        )
        echo [PRE-STEP] MapGenerator library built successfully!
        echo.
    ) else (
        echo MapGenerator setup script not found, skipping.
        echo.
    )
)

REM ============================================================
REM Main build loop
REM ============================================================
echo Starting build process for all versions...
echo ============================================================
echo.

set "TOTAL_SUCCESS=0"
set "TOTAL_FAILED=0"

for /l %%i in (1, 1, %UE_COUNT%) do (
    set "CURRENT_TEST_PATH="
    if %%i leq %TEST_COUNT% set "CURRENT_TEST_PATH=!TEST_PATH_%%i!"
    call :build_version "!UE_PATH_%%i!" %%i "!CURRENT_TEST_PATH!"
)

goto :summary

REM ============================================================
REM Subroutine: build_version
REM %1 = UE path, %2 = index, %3 = test project path (optional)
REM ============================================================
:build_version
set "UE_PATH=%~1"
set "BUILD_INDEX=%~2"
set "TEST_PROJECT_PATH=%~3"

echo.
echo ============================================================
echo [BUILD %BUILD_INDEX% of %UE_COUNT%] Processing: %UE_PATH%
echo ============================================================
echo.

for %%f in ("%UE_PATH%") do set "UE_VERSION=%%~nxf"

REM Plugin output goes into: C:/Dev/TestScript/UE_5.4/PluginName/
set "VERSION_OUTPUT_DIR=%BASE_OUTPUT_PATH%\%UE_VERSION%"
set "OUTPUT_PATH=%VERSION_OUTPUT_DIR%\%PLUGIN_NAME%"

echo UE Path:     %UE_PATH%
echo UE Version:  %UE_VERSION%
echo Output:      %OUTPUT_PATH%
if not "%TEST_PROJECT_PATH%"=="" (
    echo Test Deploy: %TEST_PROJECT_PATH%
)
echo.

if not exist "%UE_PATH%\Engine\Build\BatchFiles\RunUAT.bat" (
    echo ERROR: RunUAT.bat not found at: %UE_PATH%\Engine\Build\BatchFiles\RunUAT.bat
    echo This Unreal Engine version may not be installed correctly.
    echo.
    set /a TOTAL_FAILED+=1
    exit /b 0
)

REM [STEP 1] Clean source directory
echo [STEP 1] Cleaning cached files from source directory...
echo --------------------------------------------------------
if exist "%PLUGIN_DIR%Binaries\"         rmdir "%PLUGIN_DIR%Binaries"         /s /q 2>nul
if exist "%PLUGIN_DIR%Intermediate\"     rmdir "%PLUGIN_DIR%Intermediate"     /s /q 2>nul
if exist "%PLUGIN_DIR%Saved\"            rmdir "%PLUGIN_DIR%Saved"            /s /q 2>nul
if exist "%PLUGIN_DIR%.vs\"              rmdir "%PLUGIN_DIR%.vs"              /s /q 2>nul
if exist "%PLUGIN_DIR%DerivedDataCache\" rmdir "%PLUGIN_DIR%DerivedDataCache" /s /q 2>nul
del "%PLUGIN_DIR%*.sln" /q 2>nul
del "%PLUGIN_DIR%*.vcxproj*" /q 2>nul
echo Cleaned!
echo.
timeout /t 1 /nobreak >nul

REM [STEP 2] Clean output directory
echo [STEP 2] Preparing output directory...
echo --------------------------------------------------------
if exist "%OUTPUT_PATH%" rmdir "%OUTPUT_PATH%" /s /q 2>nul
mkdir "%OUTPUT_PATH%" 2>nul
echo Created: %OUTPUT_PATH%
echo.

REM [STEP 3] Build plugin
echo [STEP 3] Building plugin with RunUAT...
echo --------------------------------------------------------
echo This may take several minutes...
echo.

call "%UE_PATH%\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="%PLUGIN_PATH%" -Package="%OUTPUT_PATH%" -Rocket -TargetPlatforms=Win64 -Strict

if errorlevel 1 (
    echo.
    echo ERROR: Plugin build FAILED for %UE_VERSION%
    echo.
    set /a TOTAL_FAILED+=1
    exit /b 0
)

echo.
echo [STEP 3] Build completed successfully!
echo.

REM [STEP 4] Verify build output
echo [STEP 4] Verifying build output...
echo --------------------------------------------------------
if not exist "%OUTPUT_PATH%\Binaries\" echo WARNING: No Binaries folder found
dir "%OUTPUT_PATH%" /b
echo.

REM [STEP 5] Clean up output source directory
echo [STEP 5] Cleaning up output source directory...
echo --------------------------------------------------------
for /r "%OUTPUT_PATH%\Source\" %%f in (.git) do (
    if exist "%%f" (
        attrib -r -h -s "%%f" 2>nul
        del "%%f" /q 2>nul
    )
)

for /d %%d in ("%OUTPUT_PATH%\Source\*") do (
    if /i not "%%~nxd"=="ThirdParty" (
        for /d %%s in ("%%d\*") do move "%%s" "%OUTPUT_PATH%\Source\%%~nxs" >nul 2>&1
        if exist "%%d\*.*" move "%%d\*.*" "%OUTPUT_PATH%\Source\" >nul 2>&1
        rmdir "%%d" 2>nul
    )
)

del "%OUTPUT_PATH%\Source\*.md" /s /q 2>nul
del "%OUTPUT_PATH%\Source\.gitignore" /s /q 2>nul
del "%OUTPUT_PATH%\Source\.gitattributes" /s /q 2>nul
del "%OUTPUT_PATH%\Source\.git" /s /q 2>nul
del "%OUTPUT_PATH%\Source\LICENSE*" /s /q 2>nul
echo Cleaned!
echo.

REM [STEP 6] Create zip — saved to VERSION folder, not inside plugin folder
REM Result: C:/Dev/TestScript/UE_5.4/PluginName_UE_5.4.zip
echo [STEP 6] Creating submission zip file...
echo --------------------------------------------------------
set "ZIP_NAME=%PLUGIN_NAME%_%UE_VERSION%.zip"
set "ZIP_PATH=%VERSION_OUTPUT_DIR%\%ZIP_NAME%"
set "TEMP_ZIP_DIR=%TEMP%\%PLUGIN_NAME%_zip_%UE_VERSION%"

if exist "%TEMP_ZIP_DIR%" rmdir "%TEMP_ZIP_DIR%" /s /q 2>nul
mkdir "%TEMP_ZIP_DIR%"

REM Copy plugin folder contents into a named subfolder inside the zip
REM so zip extracts as PluginName/ rather than dumping files at root
mkdir "%TEMP_ZIP_DIR%\%PLUGIN_NAME%"

for /d %%d in ("%OUTPUT_PATH%\*") do (
    set "FOLDER_NAME=%%~nxd"
    if /i not "!FOLDER_NAME!"=="Binaries" (
        if /i not "!FOLDER_NAME!"=="Intermediate" (
            echo   Copying: !FOLDER_NAME!
            robocopy "%%d" "%TEMP_ZIP_DIR%\%PLUGIN_NAME%\!FOLDER_NAME!" /e /nfl /ndl /njh /njs
        ) else (
            echo   Skipping: !FOLDER_NAME!
        )
    ) else (
        echo   Skipping: !FOLDER_NAME!
    )
)

for %%f in ("%OUTPUT_PATH%\*.*") do (
    copy "%%f" "%TEMP_ZIP_DIR%\%PLUGIN_NAME%\" >nul 2>&1
)

echo Creating zip: %ZIP_PATH%
powershell -command "Compress-Archive -Path '%TEMP_ZIP_DIR%\*' -DestinationPath '%ZIP_PATH%' -Force"
rmdir "%TEMP_ZIP_DIR%" /s /q 2>nul

if exist "%ZIP_PATH%" (
    echo   SUCCESS: Zip created at %ZIP_PATH%
) else (
    echo   ERROR: Failed to create zip
    set /a TOTAL_FAILED+=1
    exit /b 0
)
echo.

REM [STEP 7] Deploy to test project (if configured)
echo [STEP 7] Deploying to test project...
echo --------------------------------------------------------
if "%TEST_PROJECT_PATH%"=="" (
    echo No test project configured for %UE_VERSION%, skipping.
    echo.
    set /a TOTAL_SUCCESS+=1
    exit /b 0
)

echo Deploying to: %TEST_PROJECT_PATH%
echo Removing old plugin files...
if exist "%TEST_PROJECT_PATH%" rmdir "%TEST_PROJECT_PATH%" /s /q 2>nul
mkdir "%TEST_PROJECT_PATH%" 2>nul

echo Copying new plugin files (without Binaries/Intermediate)...
for /d %%d in ("%OUTPUT_PATH%\*") do (
    set "FOLDER_NAME=%%~nxd"
    if /i not "!FOLDER_NAME!"=="Binaries" (
        if /i not "!FOLDER_NAME!"=="Intermediate" (
            robocopy "%%d" "%TEST_PROJECT_PATH%\!FOLDER_NAME!" /e /nfl /ndl /njh /njs
        )
    )
)
for %%f in ("%OUTPUT_PATH%\*.*") do (
    copy "%%f" "%TEST_PROJECT_PATH%\" >nul 2>&1
)

echo   SUCCESS: Plugin deployed to test project
echo   NOTE: Open the project in %UE_VERSION% to recompile binaries
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
echo Total Versions: %UE_COUNT%
echo Successful:     %TOTAL_SUCCESS%
echo Failed:         %TOTAL_FAILED%
echo.
echo Output structure:
echo   %BASE_OUTPUT_PATH%\
for /l %%i in (1, 1, %UE_COUNT%) do (
    for %%f in ("!UE_PATH_%%i!") do (
        echo     %%~nxf\
        echo       %PLUGIN_NAME%\        (built plugin)
        echo       %PLUGIN_NAME%_%%~nxf.zip  (submission zip)
    )
)
echo.

if %TOTAL_FAILED% equ 0 (
    echo [SUCCESS] All versions built successfully!
) else (
    echo [WARNING] Some versions failed. See details above.
)

echo.
echo ============================================================
pause
