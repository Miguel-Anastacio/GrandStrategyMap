// Copyright Epic Games, Inc. All Rights Reserved.

#include "MapEditor.h"

#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/MapEditorMenu.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "Styling/SlateStyleRegistry.h"

DEFINE_LOG_CATEGORY(LogInteractiveMapEditor);

#define LOCTEXT_NAMESPACE "FInteractiveMapModule"

void FInteractiveMapEditorModule::StartupModule()
{
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Map Generator Editor module has been loaded"));
	RegisterMenus();
	RegisterTabs();
	RegisterSlateStyles();
}
void FInteractiveMapEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Map Generator Editor module has been unloaded"));

	if(MapEditorMenu)
	{
		delete MapEditorMenu;
		MapEditorMenu = nullptr;
	}
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TEXT("CustomEditorLayout"));
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

void FInteractiveMapEditorModule::LaunchMapEditor() const
{
	FGlobalTabmanager::Get()->TryInvokeTab(MapEditorMainTab);
}

void FInteractiveMapEditorModule::LaunchTerrainCreator() const
{
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Launching Terrain Creator Editor"));
}

TSharedPtr<FSlateStyleSet> FInteractiveMapEditorModule::CustomAssetsEditorSlateStyle;
void FInteractiveMapEditorModule::RegisterSlateStyles() const
{
	// Register the slate styles	
	const FString ProjectResourceDir = FPaths::ProjectPluginsDir()/TEXT("CustomEditableAssets/Resources");
	
	CustomAssetsEditorSlateStyle = MakeShareable(new FSlateStyleSet("CustomEditableAssetsEditor"));
	CustomAssetsEditorSlateStyle->Set("CustomAsset.Thumbnail", new FSlateImageBrush(FString(ProjectResourceDir/"Icon128.png"), FVector2D(128.0f, 128.0f)));
	CustomAssetsEditorSlateStyle->Set("ClassIcon.CustomActor", new FSlateImageBrush(FString(ProjectResourceDir/"Icon128.png"), FVector2D(32.0f, 32.0f)));
	CustomAssetsEditorSlateStyle->Set("ClassIcon.UAssetDefinition_CustomObject", new FSlateImageBrush(FString(ProjectResourceDir/"Icon128.png"), FVector2D(32.0f, 32.0f)));
	
	FSlateStyleRegistry::RegisterSlateStyle(*CustomAssetsEditorSlateStyle.Get());
}

const TSharedRef<FTabManager::FLayout> FInteractiveMapEditorModule::CreateCustomLayout() const
{
	return FTabManager::NewLayout(FName(TEXT("CustomEditorLayout")))
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				 ->SetSizeCoefficient(.65f)
				 ->AddTab(ViewportTab, ETabState::OpenedTab)
				 ->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(.2f)
				->AddTab(ParametersTab, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
		);
}

void FInteractiveMapEditorModule::RegisterTabs()
{
	if(!MapEditorMenu)
	{
		MapEditorMenu = new RMapEditorMenu();
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MapEditorMainTab, FOnSpawnTab::CreateRaw(this, &FInteractiveMapEditorModule::SpawnMainTab))
	   .SetDisplayName(NSLOCTEXT("YourModule", "CustomEditorLayout", "Custom Editor Layout"))
	   .SetTooltipText(NSLOCTEXT("YourModule", "CustomEditorLayoutTooltip", "Open the custom editor layout"))
	   .SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
	
}

TSharedRef<SDockTab> FInteractiveMapEditorModule::SpawnMainTab(const FSpawnTabArgs& Args) const
{
	const TSharedRef<FTabManager::FLayout>	CustomLayout = CreateCustomLayout();

	TSharedPtr<SWidget> RestoredLayoutWidget = FGlobalTabmanager::Get()->RestoreFrom(CustomLayout, Args.GetOwnerWindow());

	// Ensure the layout restoration is valid
	if (!RestoredLayoutWidget.IsValid())
	{
		RestoredLayoutWidget = SNew(STextBlock).Text(FText::FromString("Failed to restore layout."));
	}

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			RestoredLayoutWidget.ToSharedRef() // Embed the restored layout widget here
		];
	
}
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInteractiveMapEditorModule, MapEditor)