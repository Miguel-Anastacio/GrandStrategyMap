// Copyright 2025 An@stacioDev All rights reserved.
#include "MapEditorDataAppMode.h"

#include "DataEntryEditorTabFactory.h"
#include "MapDataEditorTabFactory.h"
#include "MapObject.h"
#include "MapObjectDetailsTabFactory.h"
#include "Asset/SCustomInstancedStructList.h"
#include "Editor/InstancedStructWrapper.h"
#include "Editor/MapViewportTabFactory.h"
#include "Editor/NameDefines.h"
#include "Editor/MapEditorApp.h"
#include "Editor/MapEditorTexturePreviewFactory.h"

FMapEditorDataAppMode::FMapEditorDataAppMode(TSharedPtr<FMapEditorApp> app)
	: FApplicationMode(MapDataEditorModeName), App(app)
{
	EntryWrapper = NewObject<UInstancedStructWrapper>();
	
	TabSet.RegisterFactory(MakeShareable(new FMapObjectDetailsTabFactory(app)));
	TabSet.RegisterFactory(MakeShareable(new FMapViewportTabFactory(app)));
	TabSet.RegisterFactory(MakeShareable(new FMapEditorTexturePreviewFactory(app)));
	
	Init();
	const TSharedPtr<FWorkflowTabFactory> Factory = MakeShareable(new FMapDataEditorTabFactory(app));
	DataEditorListTabID = Factory->GetIdentifier();
	TabSet.RegisterFactory(Factory);

	const TSharedPtr<FWorkflowTabFactory> FactoryDatEntry = MakeShareable(new FMapDataEntryEditorTabFactory(app));
	DataEditorEntryTabID = FactoryDatEntry->GetIdentifier();
	TabSet.RegisterFactory(FactoryDatEntry);
	
	TabLayout = FTabManager::NewLayout("MapEditorDataAppMode_Layout_v1.5")
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewSplitter()
					->SetOrientation(Orient_Vertical)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab(MapEditorViewportTabName, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.25f)
						->AddTab(MapEditorPreviewTabName, ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewSplitter()
					->SetOrientation(Orient_Vertical)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.75f)
						->AddTab(MapObjectDetailsTabName, ETabState::OpenedTab)
						->AddTab(MapDataEditorTabName, ETabState::OpenedTab)
						->SetHideTabWell(false)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.25f)
						->AddTab(MapDataEntryEditorTabName, ETabState::OpenedTab)
						->SetHideTabWell(false)
					)
				)
		)
	);
}

void FMapEditorDataAppMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FMapEditorApp> app = App.Pin();
	EntryWrapper->SetAppMode(SharedThis(this));
	
	const TSharedPtr<FMapDataEditorTabFactory> Factory = StaticCastSharedPtr<FMapDataEditorTabFactory>(TabSet.GetFactory(DataEditorListTabID));
	Factory->SetAppMode(SharedThis(this));

	const TSharedPtr<FMapDataEntryEditorTabFactory> FactoryDataEntry = StaticCastSharedPtr<FMapDataEntryEditorTabFactory>(TabSet.GetFactory(DataEditorEntryTabID));
	FactoryDataEntry->SetAppMode(SharedThis(this));
	
	app->PushTabFactories(TabSet);
	Init();
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FMapEditorDataAppMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FMapEditorDataAppMode::PostActivateMode()
{
	TSharedPtr<FMapEditorApp> app = App.Pin();
	app->RestoreTexturePreview();
	Init();
	const TArray<int32> Index = app->GetWorkingAsset()->GetTilesSelected();
	if(!Index.IsEmpty())
	{
		app->UpdateEntrySelected(Index.Last());
	}
	else
	{
		app->ClearSelection();
	}
	
	FApplicationMode::PostActivateMode();
}

void FMapEditorDataAppMode::Init()
{
	ListFilterByType = App.Pin().Get()->GetFilterForDataList();
	RefreshDataList();
	
	TWeakObjectPtr<UMapObject> CustomObject = App.Pin().Get()->GetWorkingAsset();
	StructTypes.Emplace(CustomObject.Get()->StructType);
	StructTypes.Emplace(CustomObject.Get()->OceanStructType);
}

void FMapEditorDataAppMode::UpdateMap(const FInstancedStruct& Data) 
{
	App.Pin()->UpdateMapData(Data);
	RefreshDataList();
}

void FMapEditorDataAppMode::SetFilter(const UScriptStruct* Type, bool RefreshList)
{
	ListFilterByType = Type;
	if(RefreshList)
		RefreshDataList();
}

void FMapEditorDataAppMode::Refresh()
{
	RefreshDataList();

	const TArray<int32> tilesSelected = App.Pin()->GetWorkingAsset()->GetTilesSelected();
	if(tilesSelected.IsEmpty())
	{
		EditableStructListDisplay->ClearSelection();
	}
	else
	{
		const int32 selectedTile= tilesSelected.Last();
		const FInstancedStruct* data = App.Pin()->GetWorkingAsset()->GetTileData(selectedTile);
		if(data)
		{
			UpdateDataEntryEditor(*data, selectedTile);
		}
	}

}

void FMapEditorDataAppMode::UpdateDataEntryEditor(const FInstancedStruct& Data, int32 ID) const
{
	EntryWrapper->SetStructInstance(Data);
	EntryWrapper->ID = ID;
}

void FMapEditorDataAppMode::RefreshDataList()
{
	MyListItems.Empty();
	
	TWeakObjectPtr<UMapObject> CustomObject = App.Pin().Get()->GetWorkingAsset();
	for(const auto& Data : CustomObject.Get()->GetMapDataValue())
	{
		if(IncludedInFilter(Data.GetScriptStruct()))
		{
			MyListItems.Emplace(MakeShared<FInstancedStruct>(Data));
		}
	}
	if(EditableStructListDisplay)
	{
		EditableStructListDisplay->RefreshList();
	}
}

bool FMapEditorDataAppMode::IncludedInFilter(const UScriptStruct* Type) const
{
	if(!ListFilterByType)
		return true;

	return ListFilterByType == Type;
}	

