// Copyright Epic Games, Inc. All Rights Reserved.

#include "MapEditor.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/MapEditorPreset.h"
#include "Editor/SMapCreatorViewport.h"
#include "PropertyCustomizationHelpers.h"
#include "Editor/SMapEditorMenu.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
DEFINE_LOG_CATEGORY(LogInteractiveMapEditor);

#define LOCTEXT_NAMESPACE "FInteractiveMapModule"

void FInteractiveMapEditorModule::StartupModule()
{
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Map Generator Editor module has been loaded"));
	RegisterMenus();

	// FGlobalTabmanager::Get()->RegisterNomadTabSpawner("MyCustomTab", FOnSpawnTab::CreateRaw(this, &FInteractiveMapEditorModule::OnSpawnPluginTab))
	//    .SetDisplayName(FText::FromString("My Custom Tool"))
	//    .SetMenuType(ETabSpawnerMenuType::Hidden);

	RegisterTabs();

}
void FInteractiveMapEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Map Generator Editor module has been unloaded"));

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
				// FTabManager::NewSplitter()
				// ->SetOrientation(Orient_Vertical)
				// ->SetSizeCoefficient(.35f)
				// ->Split
				// (
				// 	FTabManager::NewStack()
				// 	->SetSizeCoefficient(.8f)
				// 	->AddTab(TextureSelectTab, ETabState::OpenedTab)
				// 	->SetHideTabWell(true)
				// )
				// ->Split
				// (
				// 	FTabManager::NewStack()
				// 	->SetSizeCoefficient(.2f)
				// 	->AddTab(ParametersTab, ETabState::OpenedTab)
				// 	->SetHideTabWell(true)
				// )
			)
		);
}

void FInteractiveMapEditorModule::RegisterTabs()
{
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ViewportTab, FOnSpawnTab::CreateRaw(this, &FInteractiveMapEditorModule::SpawnViewport))
		.SetDisplayName(NSLOCTEXT("YourModule", "ViewportTab", "Viewport"))
		.SetTooltipText(NSLOCTEXT("YourModule", "ViewportTabTooltip", "Open the Viewport tab"));

	// FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TextureSelectTab, FOnSpawnTab::CreateRaw(this, &FInteractiveMapEditorModule::SpawnTextureSelectTab))
	// .SetDisplayName(NSLOCTEXT("YourModule", "TextureSelectTab", "Texture Select"))
	// .SetTooltipText(NSLOCTEXT("YourModule", "ViewportTabTooltip", "Open the Texture Select tab"));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ParametersTab, FOnSpawnTab::CreateRaw(this, &FInteractiveMapEditorModule::SpawnDetailsTab))
	.SetDisplayName(NSLOCTEXT("YourModule", "DetailsTab", "Details"))
	.SetTooltipText(NSLOCTEXT("YourModule", "DetailsTabTooltip", "Open the Details tab"));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MapEditorMainTab, FOnSpawnTab::CreateRaw(this, &FInteractiveMapEditorModule::SpawnMainTab))
	   .SetDisplayName(NSLOCTEXT("YourModule", "CustomEditorLayout", "Custom Editor Layout"))
	   .SetTooltipText(NSLOCTEXT("YourModule", "CustomEditorLayoutTooltip", "Open the custom editor layout"))
	   .SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
	
}

TSharedRef<SDockTab> FInteractiveMapEditorModule::SpawnDetailsTab(const FSpawnTabArgs& Args) const
{
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	const TSharedRef<IDetailsView> DetailsView = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(DetailsViewArgs);
	DetailsView->SetObjects(TArray<UObject*>{MapEditorPreset});
	
	return SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		.TabRole(ETabRole::PanelTab)
		[
			DetailsView
		];
}

TSharedRef<SDockTab> FInteractiveMapEditorModule::SpawnTextureSelectTab(const FSpawnTabArgs& Args) const
{
	return SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		[
			// SNew(STextBlock).Text(FText::FromString("Texture select Content"))
			SNew(SObjectPropertyEntryBox)
		];
}

TSharedRef<SDockTab> FInteractiveMapEditorModule::SpawnViewport(const FSpawnTabArgs& Args)
{
	MapEditorPreset = NewObject<UMapEditorPreset>();
	MapEditorMenu = MakeShared<SMapEditorMenu>();
	MapEditorPreset->OnObjectChanged.AddRaw(this, &FInteractiveMapEditorModule::GenerateMap);
	return SNew(SDockTab)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SAssignNew(MapEditorMenu, SMapEditorMenu)
		]
	];
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

void FInteractiveMapEditorModule::GenerateMap()
{
	if(MapEditorMenu && MapEditorPreset)
		MapEditorMenu->OnTextureChanged(MapEditorPreset->MapEditorDetails.HeightMapTexture);
}

// void FInteractiveMapEditorModule::OnSpawnMapEditorTab() const
// {
// 	return SNew(SDockTab)
// 	  .TabRole(ETabRole::NomadTab)
// 	  [
// 		  SNew(SMapEditorMenu) // Your custom Slate widget
// 	  ];
// }

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInteractiveMapEditorModule, MapEditor)