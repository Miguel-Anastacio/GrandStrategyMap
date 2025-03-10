// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataBasedWidgetEditor.h"
void FDataBasedWidgetEditorModule::StartupModule()
{
	// FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	// PropertyModule.RegisterCustomClassLayout(
	// 	"WidgetMapDataAsset", // Name of the class that contains FStructClassSelector
	// 	FOnGetDetailCustomizationInstance::CreateStatic(&FCustomStructSelectorCustomization::MakeInstance)
	// );
}

void FDataBasedWidgetEditorModule::ShutdownModule()
{
	// if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	// {
	// 	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	// 	PropertyModule.UnregisterCustomClassLayout("WidgetMapDataAsset");
	// }
}
	
IMPLEMENT_MODULE(FDataBasedWidgetEditorModule, DataBasedWidgetEditor)