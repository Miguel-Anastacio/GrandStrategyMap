// Copyright Epic Games, Inc. All Rights Reserved.

#include "UtilityModuleEditor.h"
DEFINE_LOG_CATEGORY(LogUtilityModuleEditor);

// #define LOCTEXT_NAMESPACE "FInteractiveMapModule"

void FUtilityModuleEditor::StartupModule()
{
	UE_LOG(LogUtilityModuleEditor, Log, TEXT("Utility module has been loaded"));
}
void FUtilityModuleEditor::ShutdownModule()
{
	UE_LOG(LogUtilityModuleEditor, Log, TEXT("Utility module has been unloaded"));
}


IMPLEMENT_MODULE(FUtilityModuleEditor, UtilityModuleEditor)