// Copyright 2025 An@stacioDev All rights reserved.
#include "MapEditorDataAppMode.h"
#include "MapDataEditorTabFactory.h"
#include "MapObjectDetailsTabFactory.h"
#include "Editor/MapViewportTabFactory.h"
#include "Editor/NameDefines.h"
#include "Editor/MapEditorApp.h"

FMapEditorDataAppMode::FMapEditorDataAppMode(TSharedPtr<FMapEditorApp> app)
	: FApplicationMode(MapDataEditorModeName), App(app)
{
	TabSet.RegisterFactory(MakeShareable(new FMapObjectDetailsTabFactory(app)));
	TabSet.RegisterFactory(MakeShareable(new FMapViewportTabFactory(app)));
	TabSet.RegisterFactory(MakeShareable(new FMapDataEditorTabFactory(app)));
	TabLayout = FTabManager::NewLayout("MapEditorDataAppMode_Layout_v1.1")
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
					FTabManager::NewStack()
					->SetSizeCoefficient(0.5f)
					->AddTab(MapObjectDetailsTabName, ETabState::OpenedTab)
					->AddTab(MapDataEditorTabName, ETabState::OpenedTab)
					->SetHideTabWell(false)
				)
		)
	);
}

void FMapEditorDataAppMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FMapEditorApp> app = App.Pin();
	app->PushTabFactories(TabSet);
	
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
	FApplicationMode::PostActivateMode();
}