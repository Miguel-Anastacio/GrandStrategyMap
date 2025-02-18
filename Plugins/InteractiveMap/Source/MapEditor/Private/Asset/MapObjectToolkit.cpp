#include "Asset/MapObjectToolkit.h"
#include "MapObject.h"
#include "Asset/SMapObjectViewport.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Docking/SDockTab.h"
#include "Internationalization/Text.h"
#include "Framework/Docking/TabManager.h"
#include "MapEditor.h"
#include "Asset/SCustomInstancedStructList.h"
#include "SlateWidgets/InstancedStructList.h"

FName MapViewportTab = FName(TEXT("MapViewportTab"));
FName MapStatsTab = FName(TEXT("MapStatsTab"));
FName DataSourceTab = FName(TEXT("DataSourceTab"));
FName DataListTab = FName(TEXT("DataListTab"));

void FMapObjectToolkit::InitEditor(const TSharedPtr<IToolkitHost >& InitToolkitHost, UMapObject* Object)
{
	CustomObject = Object;
	
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout(FName(TEXT("Layout")))
	->AddArea
	(
		FTabManager::NewPrimaryArea()
		->SetOrientation(Orient_Horizontal)
		->Split
		(
			FTabManager::NewStack()
			->SetSizeCoefficient(.5f)
			->AddTab(MapViewportTab, ETabState::OpenedTab)
			->SetHideTabWell(true)
		)
		->Split
		(
			FTabManager::NewSplitter()
			->SetOrientation(Orient_Vertical)
			->SetSizeCoefficient(.45f)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.35f)
				->AddTab(DataSourceTab, ETabState::OpenedTab)
				->AddTab(DataListTab, ETabState::OpenedTab)
				->SetHideTabWell(false)
			)
			// ->Split
			// (
			// 	FTabManager::NewStack()
			// 	->SetSizeCoefficient(.65f)
			// 	->AddTab(DataListTab, ETabState::OpenedTab)
			// 	->SetHideTabWell(true)
			// )
		)	
	);
	
	InitAssetEditor(EToolkitMode::Standalone, InitToolkitHost, GetToolkitFName(), Layout, true, true, CustomObject.Get());
}

void FMapObjectToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("CustomConfigEditor"));

	// VIEWPORT
	InTabManager->RegisterTabSpawner(MapViewportTab, FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SAssignNew(MapViewport, SMapObjectViewport)
				.EditingObject(CustomObject.Get())
				.Toolkit(TWeakPtr<FMapObjectToolkit>(SharedThis(this)))
			]
		];
	}))
	.SetDisplayName(INVTEXT("Viewport"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	// STATS PLACEHOLDER
	InTabManager->RegisterTabSpawner(MapStatsTab, FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
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
	DetailsView->SetObjects(TArray<UObject*>{CustomObject.Get()});
	
	InTabManager->RegisterTabSpawner(DataSourceTab, FOnSpawnTab::CreateLambda([this, DetailsView](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		.TabRole(PanelTab)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			.Padding(5)
			[
				DetailsView
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			.Padding(5)
			[
				SNew(SButton)
				.Text(FText::FromString("Load Data file"))
				.OnClicked_Lambda([this]() -> FReply
				{
					OnLoadFile();
					return FReply::Handled();
				})
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			.Padding(5)
			[
				SNew(SButton)
				.Text(FText::FromString("Save to file"))
				.OnClicked_Lambda([this]() -> FReply
				{
					this->CustomObject->SaveData();
					return FReply::Handled();
				})
			]
		];
	}))
	.SetDisplayName(INVTEXT("Data Source"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	// Setup List refernces
	for(const auto& Data : CustomObject.Get()->GetMapDataValue())
	{
		MyListItems.Emplace(MakeShared<FInstancedStruct>(Data));
	}
	StructTypes.Emplace(CustomObject.Get()->StructType);
	StructTypes.Emplace(CustomObject.Get()->OceanStructType);
	PropertyNamesNotEditable.Emplace("ID");
	PropertiesWithDropDown = CustomObject->GetNamesOfVisualPropertiesInMapData();

	InTabManager->RegisterTabSpawner(DataListTab, FOnSpawnTab::CreateLambda([this, InTabManager](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		.TabRole(PanelTab)
		[
			SAssignNew(EditableStructListDisplay, SCustomInstancedStructList)
				.ListSource(&MyListItems)
				.StructTypes(&StructTypes)
				.NotEditableProperties(&PropertyNamesNotEditable)
				.PropertiesWithDropdown(&PropertiesWithDropDown)
				.MapObject(CustomObject)
				.OnItemChanged_Lambda([this](const FInstancedStruct& Item)
				{
					CustomObject.Get()->UpdateDataInEditor(Item);
				})
				.OnSelectionChanged_Lambda([this](const int32 ID)
				{
					MapViewport->UpdatePreviewActor(ID);
				})
				
		];
	}))
	.SetDisplayName(INVTEXT("DataList"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	
}

void FMapObjectToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManagerRef)
{
	CustomObject = nullptr;

	TabManagerRef->UnregisterAllTabSpawners();
	
	FAssetEditorToolkit::UnregisterTabSpawners(TabManagerRef);
}

void FMapObjectToolkit::UpdateTreeSelection(int32 Index) const
{
	if(EditableStructListDisplay)
	{
		EditableStructListDisplay->SetSelection(Index);
	}
}

void FMapObjectToolkit::OnLoadFile() const
{
	if(!CustomObject.IsValid())
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Map Object is NULL"));
		return;
	}
	
	CustomObject->LoadDataFromFile();
	// if(TreeDisplay)
	// {
	// 	TreeDisplay->RebuildTreeFromMap(CustomObject.Get());
	// }
}


