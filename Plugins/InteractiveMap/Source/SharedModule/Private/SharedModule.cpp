// Copyright Epic Games, Inc. All Rights Reserved.

#include "SharedModule/Public/SharedModule.h"
// DEFINE_LOG_CATEGORY(LogUtilityModule);

// #define LOCTEXT_NAMESPACE "FInteractiveMapModule"

void FSharedModule::StartupModule()
{
	// UE_LOG(LogUtilityModule, Log, TEXT("Utility module has been loaded"));
}
void FSharedModule::ShutdownModule()
{
	// UE_LOG(LogUtilityModule, Log, TEXT("Utility module has been unloaded"));
}

IMPLEMENT_MODULE(FSharedModule, SharedModule)