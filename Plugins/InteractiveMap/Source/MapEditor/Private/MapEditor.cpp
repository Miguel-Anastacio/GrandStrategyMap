// Copyright Epic Games, Inc. All Rights Reserved.

#include "MapEditor.h"
#include "ToolMenus.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
DEFINE_LOG_CATEGORY(LogInteractiveMapEditor);

#define LOCTEXT_NAMESPACE "FInteractiveMapModule"
void FInteractiveMapEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	RegisterMenus();
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Map Generator Editor module has been loaded"));

}
void FInteractiveMapEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Map Generator Editor module has been unloaded"));
}

void FInteractiveMapEditorModule::RegisterMenus() const
{
	// Ensure ToolMenus are initialized
	if (!UToolMenus::IsToolMenuUIEnabled())
	{
		return;
	}

	// Get or create the Tools menu
	UToolMenus* ToolMenus = UToolMenus::Get();
	UToolMenu* ToolsMenu = ToolMenus->ExtendMenu("LevelEditor.MainMenu.Tools");

	// Add a new section to the Tools menu
	FToolMenuSection& MyPluginSection = ToolsMenu->AddSection(
		"MyPluginSection", // Unique name for the section
		FText::FromString("My Plugin Tools") // Section heading
	);

	// Add menu entries to the section
	MyPluginSection.AddMenuEntry(
		"MapEditor",
		FText::FromString("Map Editor"),
		FText::FromString("Launch Map Editor"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FInteractiveMapEditorModule::LaunchMapEditor))
	);

	// MyPluginSection.AddSeparator("MySeparator"); // Add a separator

	MyPluginSection.AddMenuEntry(
		"TerrainCreator",
		FText::FromString("Terrain Generator Editor"),
		FText::FromString("Launch Terrain Generator"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FInteractiveMapEditorModule::LaunchTerrainCreator))
	);

	// Refresh the menus
	ToolMenus->RefreshAllWidgets();
}

void FInteractiveMapEditorModule::AddMenuExtensions(FMenuBuilder& Builder) const
{
	// FToolMenuSection& AssetSection = Builder->FindOrAddSection("FileAsset");
			
	// AssetSection.AddSeparator("FileAssetSeparator").InsertPosition = FToolMenuInsert(NAME_None, EToolMenuInsertType::First);
	
	Builder.AddMenuEntry(
		FText::FromString("Map Editor"),
		FText::FromString("Launch Map Editor"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FInteractiveMapEditorModule::LaunchMapEditor))
	);

	Builder.AddMenuEntry(
		FText::FromString("Terrain Generator Editor"),
		FText::FromString("Launch Terrain Generator"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FInteractiveMapEditorModule::LaunchTerrainCreator))
	);
}

void FInteractiveMapEditorModule::LaunchMapEditor() const
{
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Launching Map Editor"));
}

void FInteractiveMapEditorModule::LaunchTerrainCreator() const
{
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Launching Terrain Creator Editor"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInteractiveMapEditorModule, MapEditor)