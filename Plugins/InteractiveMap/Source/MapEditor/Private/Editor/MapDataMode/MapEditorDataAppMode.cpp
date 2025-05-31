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

FMapEditorDataAppMode::FMapEditorDataAppMode(TSharedPtr<FMapEditorApp> app)
	: FApplicationMode(MapDataEditorModeName), App(app)
{
	EntryWrapper = NewObject<UInstancedStructWrapper>();
	
	TabSet.RegisterFactory(MakeShareable(new FMapObjectDetailsTabFactory(app)));
	TabSet.RegisterFactory(MakeShareable(new FMapViewportTabFactory(app)));
	
	Init();
	TSharedPtr<FWorkflowTabFactory> Factory = MakeShareable(new FMapDataEditorTabFactory(app));
	DataEditorListTabID = Factory->GetIdentifier();
	TabSet.RegisterFactory(Factory);

	TSharedPtr<FWorkflowTabFactory> FactoryDatEntry = MakeShareable(new FMapDataEntryEditorTabFactory(app));
	DataEditorEntryTabID = FactoryDatEntry->GetIdentifier();
	TabSet.RegisterFactory(FactoryDatEntry);
	
	
	TabLayout = FTabManager::NewLayout("MapEditorDataAppMode_Layout_v1.2")
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
	
	FApplicationMode::PostActivateMode();
}

void FMapEditorDataAppMode::Init()
{
	RefreshDataList();
	// StructTypes.Empty();
	// PropertyNamesNotEditable.Empty();
	
	TWeakObjectPtr<UMapObject> CustomObject = App.Pin().Get()->GetWorkingAsset();
	StructTypes.Emplace(CustomObject.Get()->StructType);
	StructTypes.Emplace(CustomObject.Get()->OceanStructType);
	// PropertyNamesNotEditable.Emplace("ID");
	// PropertiesWithDropDown = CustomObject->GetNamesOfVisualPropertiesInMapData();
	
}

void FMapEditorDataAppMode::UpdateMap(const FInstancedStruct& Data, int ID) 
{
	TSharedPtr<FMapEditorApp> app = App.Pin();
	app->GetWorkingAsset()->SetTileData(Data, ID);
	RefreshDataList();
}

void FMapEditorDataAppMode::RefreshDataList()
{
	MyListItems.Empty();
	
	TWeakObjectPtr<UMapObject> CustomObject = App.Pin().Get()->GetWorkingAsset();
	for(const auto& Data : CustomObject.Get()->GetMapDataValue())
	{
		MyListItems.Emplace(MakeShared<FInstancedStruct>(Data));
	}
	if(EditableStructListDisplay)
	{
		EditableStructListDisplay->RefreshList();
	}
}
