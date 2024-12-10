// Copyright Epic Games, Inc. All Rights Reserved.

#include "MapEditor.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/MapEditorMenu.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "UObject/SavePackage.h"
DEFINE_LOG_CATEGORY(LogInteractiveMapEditor);

#define LOCTEXT_NAMESPACE "FInteractiveMapModule"

void FInteractiveMapEditorModule::StartupModule()
{
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Map Generator Editor module has been loaded"));
	RegisterMenus();
	RegisterTabs();
}
void FInteractiveMapEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Map Generator Editor module has been unloaded"));

	if(_MapEditorMenu)
	{
		delete _MapEditorMenu;
		_MapEditorMenu = NULL;
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
	if(!_MapEditorMenu)
	{
		_MapEditorMenu = new MapEditorMenu();
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

bool FInteractiveMapEditorModule::SaveObject(UObject* Object)
{
	UPackage* Package = Object->GetPackage();
	const FString PackageName = Package->GetName();
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

	FSavePackageArgs SaveArgs;
	
	// This is specified just for example
	{
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
	}
	
	const bool bSucceeded = UPackage::SavePackage(Package, nullptr, *PackageFileName, SaveArgs);

	if (!bSucceeded)
	{
		UE_LOG(LogTemp, Error, TEXT("Package '%s' wasn't saved!"), *PackageName)
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Package '%s' was successfully saved"), *PackageName)
	return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInteractiveMapEditorModule, MapEditor)