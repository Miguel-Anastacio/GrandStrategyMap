REM ============================================================
REM Run this script from the command line, providing the following parameters:
REM 1. UE Engine Path (e.g. "D:\Programs\Unreal\UE_5.4")
REM 2. Plugin .uplugin file path (e.g. "D:\MyPlugin\MyPlugin.uplugin")
REM 3. Base output path for built plugin (e.g. "D:\BuiltPlugins\")
REM This script performs an enhanced plugin build that mimics Epic's clean build environment. 
REM It ensures all cached/intermediate files are removed before building, which is critical for passing Epic's technical review process. 
REM It also includes a pre-step to build the MapGenerator static library, which is a dependency for the plugin.
REM It will also correct the output structure to match what Epic expects regarding third-party dependencies.
REM ============================================================

@echo off
setlocal enabledelayedexpansion

REM Enhanced Plugin Build Script - Mimics Epic's Clean Build Environment
REM This script ensures a clean build by removing all cached/intermediate files

REM Get parameters from command line
set "UE_PATH=%~1"
set "PLUGIN_PATH=%~2"
set "BASE_OUTPUT_PATH=%~3"

REM Check if all parameters were provided
if "%UE_PATH%"=="" (
    echo Usage: %0 "UE_Path" "Plugin_Path" "Base_Output_Path"
    echo Example: %0 "D:\Programs\Unreal\UE_5.4" "D:\MyPlugin\MyPlugin.uplugin" "D:\BuiltPlugins\"
    pause
    exit /b 1
)

if "%PLUGIN_PATH%"=="" (
    echo Error: Plugin path is required
    pause
    exit /b 1
)

if "%BASE_OUTPUT_PATH%"=="" (
    echo Error: Base output path is required
    pause
    exit /b 1
)

REM Extract UE version from path
for %%f in ("%UE_PATH%") do set "UE_VERSION=%%~nxf"

REM Extract plugin name and directory
for %%f in ("%PLUGIN_PATH%") do (
    set "PLUGIN_NAME=%%~nf"
    set "PLUGIN_DIR=%%~dpf"
)

REM Create the final output path
set "OUTPUT_PATH=%BASE_OUTPUT_PATH%%UE_VERSION%\%PLUGIN_NAME%"

echo ============================================================
echo Enhanced Plugin Build Script - Clean Build Mode
echo ============================================================
echo UE Path: %UE_PATH%
echo Plugin: %PLUGIN_PATH%
echo Plugin Name: %PLUGIN_NAME%
echo Plugin Dir: %PLUGIN_DIR%
echo UE Version: %UE_VERSION%
echo Output: %OUTPUT_PATH%
echo ============================================================
echo.

REM ============================================================
REM PRE-STEP: Build the MapGenerator static library
REM ============================================================
echo.
echo [PRE-STEP] Building MapGenerator static library...
echo --------------------------------------------------------

REM Define the MapGenerator library path
set "MAP_GEN_SETUP=%PLUGIN_DIR%Source\ThirdParty\MapGeneratorLibrary\MapGenerator\setup.bat"

REM Check if the setup script exists
if not exist "%MAP_GEN_SETUP%" (
    echo ERROR: MapGenerator setup script not found at: %MAP_GEN_SETUP%
    echo Please ensure the MapGenerator library is properly set up.
    pause
    exit /b 1
)

REM Run the setup script to build the library
echo Running: %MAP_GEN_SETUP%
call "%MAP_GEN_SETUP%"

REM Check if the library build was successful
if errorlevel 1 (
    echo.
    echo ============================================================
    echo ERROR: MapGenerator library build FAILED
    echo ============================================================
    echo.
    pause
    exit /b 1
)

echo.
echo [PRE-STEP] MapGenerator library built successfully!
echo.
timeout /t 2 /nobreak >nul
echo.

REM Validate paths exist
if not exist "%UE_PATH%\Engine\Build\BatchFiles\RunUAT.bat" (
    echo ERROR: RunUAT.bat not found at: %UE_PATH%\Engine\Build\BatchFiles\RunUAT.bat
    echo Please check your UE Engine path
    pause
    exit /b 1
)

if not exist "%PLUGIN_PATH%" (
    echo ERROR: Plugin file not found at: %PLUGIN_PATH%
    pause
    exit /b 1
)

REM ============================================================
REM STEP 1: Clean cached files in source directory
REM This prevents using stale intermediate files
REM ============================================================
echo.
echo [STEP 1] Cleaning cached files from source directory...
echo --------------------------------------------------------

REM Remove Binaries folder
if exist "%PLUGIN_DIR%Binaries\" (
    echo Removing Binaries folder...
    rmdir "%PLUGIN_DIR%Binaries" /s /q 2>nul
    if exist "%PLUGIN_DIR%Binaries\" (
        echo WARNING: Could not fully remove Binaries folder
    ) else (
        echo   SUCCESS: Binaries removed
    )
)

REM Remove Intermediate folder
if exist "%PLUGIN_DIR%Intermediate\" (
    echo Removing Intermediate folder...
    rmdir "%PLUGIN_DIR%Intermediate" /s /q 2>nul
    if exist "%PLUGIN_DIR%Intermediate\" (
        echo WARNING: Could not fully remove Intermediate folder
    ) else (
        echo   SUCCESS: Intermediate removed
    )
)

REM Remove Saved folder
if exist "%PLUGIN_DIR%Saved\" (
    echo Removing Saved folder...
    rmdir "%PLUGIN_DIR%Saved" /s /q 2>nul
    if exist "%PLUGIN_DIR%Saved\" (
        echo WARNING: Could not fully remove Saved folder
    ) else (
        echo   SUCCESS: Saved removed
    )
)

REM Remove .vs folder (Visual Studio cache)
if exist "%PLUGIN_DIR%.vs\" (
    echo Removing .vs folder...
    rmdir "%PLUGIN_DIR%.vs" /s /q 2>nul
    if exist "%PLUGIN_DIR%.vs\" (
        echo WARNING: Could not fully remove .vs folder
    ) else (
        echo   SUCCESS: .vs removed
    )
)

REM Remove DerivedDataCache if it exists
if exist "%PLUGIN_DIR%DerivedDataCache\" (
    echo Removing DerivedDataCache folder...
    rmdir "%PLUGIN_DIR%DerivedDataCache" /s /q 2>nul
    if exist "%PLUGIN_DIR%DerivedDataCache\" (
        echo WARNING: Could not fully remove DerivedDataCache folder
    ) else (
        echo   SUCCESS: DerivedDataCache removed
    )
)

REM Remove any .sln or .vcxproj files
echo Removing project files...
del "%PLUGIN_DIR%*.sln" /q 2>nul
del "%PLUGIN_DIR%*.vcxproj*" /q 2>nul
echo   SUCCESS: Project files cleaned

echo.
echo Cached files cleaned successfully!
echo Waiting 2 seconds to ensure file locks are released...
timeout /t 2 /nobreak >nul

REM ============================================================
REM STEP 2: Clean output directory
REM ============================================================
echo.
echo [STEP 2] Preparing output directory...
echo --------------------------------------------------------

if exist "%OUTPUT_PATH%" (
    echo Removing existing output directory...
    rmdir "%OUTPUT_PATH%" /s /q 2>nul
    if exist "%OUTPUT_PATH%" (
        echo WARNING: Could not fully remove output directory
    )
)

echo Creating fresh output directory: %OUTPUT_PATH%
mkdir "%OUTPUT_PATH%" 2>nul

REM ============================================================
REM STEP 3: Build the plugin using RunUAT
REM This uses the same command Epic uses for marketplace submissions
REM ============================================================
echo.
echo [STEP 3] Building plugin with RunUAT (Clean Build)...
echo --------------------------------------------------------
echo This may take several minutes...
echo.

@REM REM Capture build output to a log file for analysis
@REM set "BUILD_LOG=%OUTPUT_PATH%_BuildLog.txt"

REM Use -Clean flag to force a clean build (no cached files)
call "%UE_PATH%\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="%PLUGIN_PATH%" -Package="%OUTPUT_PATH%" -Rocket -TargetPlatforms=Win64 -Strict

REM Store the error level before any other commands
set BUILD_ERROR=%errorlevel%

@REM REM Check for dependency warnings in the output
@REM echo.
@REM echo Checking build output for warnings...
@REM findstr /C:"does not list plugin" "%BUILD_LOG%" >nul
@REM if %errorlevel% equ 0 (
@REM     echo.
@REM     echo ============================================================
@REM     echo WARNING: Plugin dependency issues detected!
@REM     echo ============================================================
@REM     echo.
@REM     findstr /C:"does not list plugin" "%BUILD_LOG%"
@REM     echo.
@REM     echo These warnings indicate missing plugin dependencies in your .uplugin file.
@REM     echo Epic's validation WILL FAIL if these are not fixed.
@REM     echo.
@REM     echo To fix: Add StructUtils to the "Plugins" array in your .uplugin file:
@REM     echo   "Plugins": [
@REM     echo       {
@REM     echo           "Name": "StructUtils",
@REM     echo           "Enabled": true
@REM     echo       }
@REM     echo   ]
@REM     echo.
@REM )

REM Check if the build was successful
if %BUILD_ERROR% neq 0 (
    echo.
    echo ============================================================
    echo ERROR: Plugin build FAILED with exit code %BUILD_ERROR%
    echo ============================================================
    echo.
    echo This is the same error Epic would see during submission review.
    echo Please check the build log for details: %BUILD_LOG%
    echo.
    echo Common issues:
    echo - Missing module dependencies in .Build.cs
    echo - Missing header includes
    echo - Platform-specific code issues
    echo - API changes between engine versions
    echo.
    pause
    exit /b 1
)

echo.
echo ============================================================
echo [STEP 3] Build completed successfully!
echo ============================================================

REM ============================================================
REM STEP 4: Verify the build compiled correctly
REM ============================================================
echo.
echo [STEP 4] Verifying build output...
echo --------------------------------------------------------

REM Check if Binaries folder was created (indicates successful compilation)
if not exist "%OUTPUT_PATH%\Binaries\" (
    echo WARNING: No Binaries folder found - plugin may not have compiled
    echo This would cause Epic's submission to fail
    pause
)

REM List what was built
echo.
echo Build output contents:
dir "%OUTPUT_PATH%" /b
echo.

REM ============================================================
REM STEP 5: Clean up source directory in output
REM ============================================================
echo.
echo [STEP 5] Cleaning up output source directory...
echo --------------------------------------------------------

REM Remove .git files
echo Removing .git files...
for /r "%OUTPUT_PATH%\Source\" %%f in (.git) do (
    if exist "%%f" (
        attrib -r -h -s "%%f" 2>nul
        del "%%f" /q 2>nul
    )
)

REM Move subfolders
echo Moving first-level subfolders...
for /d %%d in ("%OUTPUT_PATH%\Source\*") do (
    for /d %%s in ("%%d\*") do (
        move "%%s" "%OUTPUT_PATH%\Source\%%~nxs" >nul 2>&1
    )
    if exist "%%d\*.*" move "%%d\*.*" "%OUTPUT_PATH%\Source\" >nul 2>&1
    rmdir "%%d" 2>nul
)

REM Clean up unwanted files
echo Cleaning up unwanted files...
del "%OUTPUT_PATH%\Source\*.md" /s /q 2>nul
del "%OUTPUT_PATH%\Source\.gitignore" /s /q 2>nul
del "%OUTPUT_PATH%\Source\.gitattributes" /s /q 2>nul
del "%OUTPUT_PATH%\Source\.git" /s /q 2>nul
del "%OUTPUT_PATH%\Source\LICENSE*" /s /q 2>nul

for /f "delims=" %%d in ('dir "%OUTPUT_PATH%\Source\" /ad /b /s 2^>nul ^| findstr "\.git$"') do (
    rmdir "%%d" /s /q 2>nul
)

REM ============================================================
REM STEP 6: Create zip file (without Binaries/Intermediate)
REM ============================================================
echo.
echo [STEP 6] Creating submission zip file...
echo --------------------------------------------------------

set "ZIP_NAME=%PLUGIN_NAME%_%UE_VERSION%.zip"
set "ZIP_PATH=%OUTPUT_PATH%\%ZIP_NAME%"
set "TEMP_ZIP_DIR=%TEMP%\%PLUGIN_NAME%_zip_temp"

echo Preparing zip contents...
if exist "%TEMP_ZIP_DIR%" rmdir "%TEMP_ZIP_DIR%" /s /q 2>nul
mkdir "%TEMP_ZIP_DIR%"

REM Copy everything except Binaries and Intermediate
echo Copying plugin files for zip...
for /d %%d in ("%OUTPUT_PATH%\*") do (
    set "FOLDER_NAME=%%~nxd"
    if /i not "!FOLDER_NAME!"=="Binaries" (
        if /i not "!FOLDER_NAME!"=="Intermediate" (
            echo   Copying: !FOLDER_NAME!
            robocopy "%%d" "%TEMP_ZIP_DIR%\!FOLDER_NAME!" /e /nfl /ndl /njh /njs
        ) else (
            echo   Skipping: !FOLDER_NAME!
        )
    ) else (
        echo   Skipping: !FOLDER_NAME!
    )
)

REM Copy loose files
for %%f in ("%OUTPUT_PATH%\*.*") do (
    set "FILE_NAME=%%~nxf"
    if /i not "!FILE_NAME!"=="%ZIP_NAME%" (
        copy "%%f" "%TEMP_ZIP_DIR%\" >nul 2>&1
    )
)

echo Creating zip: %ZIP_PATH%
powershell -command "Compress-Archive -Path '%TEMP_ZIP_DIR%\*' -DestinationPath '%ZIP_PATH%' -Force"

echo Cleaning up temporary files...
rmdir "%TEMP_ZIP_DIR%" /s /q 2>nul

REM ============================================================
REM FINAL STATUS
REM ============================================================
echo.
echo ============================================================
echo BUILD COMPLETE!
echo ============================================================
echo.
echo Output location: %OUTPUT_PATH%
echo.

if exist "%ZIP_PATH%" (
    echo [SUCCESS] Submission zip created: %ZIP_NAME%
    echo.
    echo This build was performed with:
    echo   - Clean intermediate files
    echo   - Fresh compilation
    echo   - Same tools Epic uses for review
    echo.
    echo If this build succeeded, your plugin should pass Epic's
    echo technical review compilation step.
) else (
    echo [ERROR] Failed to create zip file
)

echo.
echo ============================================================
pause