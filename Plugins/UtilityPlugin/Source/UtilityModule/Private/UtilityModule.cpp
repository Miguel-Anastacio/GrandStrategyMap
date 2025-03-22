// Copyright Epic Games, Inc. All Rights Reserved.

#include "UtilityModule.h"
DEFINE_LOG_CATEGORY(LogUtilityModule);

void FUtilityModule::StartupModule()
{
	UE_LOG(LogUtilityModule, Log, TEXT("Utility module has been loaded"));
}
void FUtilityModule::ShutdownModule()
{
	UE_LOG(LogUtilityModule, Log, TEXT("Utility module has been unloaded"));
}
	
IMPLEMENT_MODULE(FUtilityModule, UtilityModule)