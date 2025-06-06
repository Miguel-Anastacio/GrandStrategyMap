@echo off
setlocal

REM === Configuration ===
set PROJECT_NAME=MapGenerator
set BUILD_DIR=build
set BUILD_TYPE=Release
set GENERATOR=Ninja
set PLATFORM=Windows
set LIB_OUTPUT_DIR=%BUILD_DIR%\Binaries\%PLATFORM%\%BUILD_TYPE%\%PROJECT_NAME%
set LIB_FILE=%LIB_OUTPUT_DIR%\%PROJECT_NAME%.lib


REM === Relative path to Unreal's ThirdParty destination ===
set UE_THIRDPARTY_LIB_DIR=..\MapGenerator

REM === Check for Ninja ===
where ninja >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Ninja is not installed or not in PATH.
    echo Download it from: https://github.com/ninja-build/ninja/releases
    exit /b 1
)

REM === Check for MSVC Compiler ===
where cl >nul 2>&1
if errorlevel 1 (
    echo [ERROR] MSVC compiler not found.
    echo Please run this from a "Developer Command Prompt for Visual Studio".
    exit /b 1
)

echo [INFO] Cleaning previous build...
if exist %BUILD_DIR% (
    rmdir /s /q %BUILD_DIR%
)

echo [INFO] Generating CMake project with %GENERATOR%...
cmake -S . -B %BUILD_DIR% -G "%GENERATOR%" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if errorlevel 1 (
    echo [ERROR] CMake generation failed.
    exit /b 1
)

echo [INFO] Building %PROJECT_NAME% (%BUILD_TYPE%)...
cmake --build %BUILD_DIR% --config %BUILD_TYPE%
if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)

REM === Copy the built .lib file ===
if exist "%LIB_FILE%" (
    echo [INFO] Copying %LIB_FILE% to Unreal ThirdParty directory...
    copy /Y "%LIB_FILE%" "%UE_THIRDPARTY_LIB_DIR%\%PROJECT_NAME%.lib"
    echo [SUCCESS] Copied to: %UE_THIRDPARTY_LIB_DIR%\%PROJECT_NAME%.lib
) else (
    echo [ERROR] Library file not found at expected location:
    echo %LIB_FILE%
    exit /b 1
)

endlocal
pause