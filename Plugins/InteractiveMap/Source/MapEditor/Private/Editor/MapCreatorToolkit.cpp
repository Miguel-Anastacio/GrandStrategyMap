#include "Editor/MapCreatorToolkit.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Docking/SDockTab.h"
#include "Internationalization/Text.h"
#include "Framework/Docking/TabManager.h"
#include "Editor/SMapCreatorViewport.h"
#include "Editor/MapEditorPreset.h"

FName ViewportTab = FName(TEXT("ViewportTab"));
FName StatsTab = FName(TEXT("StatsTab"));
FName DetailsTab = FName(TEXT("DetailsTab"));

void FMapCreatorToolKit::Initialize(const TSharedPtr<IToolkitHost>& InitToolkitHost, UMapEditorPreset* EditorPreset)
{
	MapEditorStylePreset = EditorPreset;
	
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout(FName(TEXT("CustomEditorLayout")))
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
			FTabManager::NewSplitter()
			->SetOrientation(Orient_Vertical)
			->SetSizeCoefficient(.35f)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(.8f)
				->AddTab(DetailsTab, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(.2f)
				->AddTab(StatsTab, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
		)
	);
	
	InitAssetEditor(EToolkitMode::Standalone, InitToolkitHost, GetToolkitFName(), Layout, true, true, EditorPreset);
}

void FMapCreatorToolKit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("CustomConfigEditor"));

	// VIEWPORT
	InTabManager->RegisterTabSpawner(ViewportTab, FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SMapCreatorViewport)
				.EditingObject(MapEditorStylePreset.Get())
			]
		];
	}))
	.SetDisplayName(INVTEXT("Viewport"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	// STATS PLACEHOLDER
	InTabManager->RegisterTabSpawner(StatsTab, FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		.TabRole(PanelTab)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.Padding(10)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Visibility(EVisibility::HitTestInvisible)
				.TextStyle(FAppStyle::Get(), "Graph.CornerText")
				.Text(FText::FromString("STATS PLACEHOLDER"))
			]
		];
	}))
	.SetDisplayName(INVTEXT("Stats"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	// DETAILS VIEW
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	const TSharedRef<IDetailsView> DetailsView = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(DetailsViewArgs);
	DetailsView->SetObjects(TArray<UObject*>{MapEditorStylePreset.Get()});
	
	InTabManager->RegisterTabSpawner(DetailsTab, FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		.TabRole(PanelTab)
		[
			DetailsView
		];
	}))
	.SetDisplayName(INVTEXT("Details"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FMapCreatorToolKit::UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManagerRef)
{
	MapEditorStylePreset = nullptr;

	TabManagerRef->UnregisterAllTabSpawners();
	
	FAssetEditorToolkit::UnregisterTabSpawners(TabManagerRef);
}

