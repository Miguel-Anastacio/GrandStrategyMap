// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClickableMap.h"
#include "Modules/ModuleManager.h"

//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, ClickableMap, "ClickableMap" );

void FShaderBitsModule::StartupModule()
{
	//#if (ENGINE_MINOR_VERSION >= 21)
	FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders")); // add this!
	AddShaderSourceDirectoryMapping("/Project", ShaderDirectory);
	//#endif

}

void FShaderBitsModule::ShutdownModule() {}

//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, ShaderBits, "ShaderBits" );
IMPLEMENT_PRIMARY_GAME_MODULE(FShaderBitsModule, ClickableMap, "ClickableMap"); // edit this!