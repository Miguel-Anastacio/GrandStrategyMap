// Copyright 2025 An@stacioDev All rights reserved.
#include "MapEditorGenAppMode.h"

#include "MapEditorGenTabFactory.h"
#include "MapEditorTexturePreviewFactory.h"
#include "Editor/MapViewportTabFactory.h"
#include "Editor/NameDefines.h"
#include "Editor/MapEditorApp.h"

FMapEditorGenAppMode::FMapEditorGenAppMode(TSharedPtr<FMapEditorApp> app)
	: FApplicationMode(MapEditorGenModeName), App(app)
{
	TabSet.RegisterFactory(MakeShareable(new FMapEditorGenTabFactory(app)));
	TabSet.RegisterFactory(MakeShareable(new FMapViewportTabFactory(app)));
	TabSet.RegisterFactory(MakeShareable(new FMapEditorTexturePreviewFactory(app)));
	TabLayout = FTabManager::NewLayout("MapEditorAppMode_Layout_v2")
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
						->SetSizeCoefficient(0.5f)
						->AddTab(MapEditorPreviewTabName, ETabState::OpenedTab)
					)
					
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.5f)
					->AddTab(MapEditorGenDetailsTabName, ETabState::OpenedTab)
				)
		)
	);
}

void FMapEditorGenAppMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FMapEditorApp> app = App.Pin();
	app->PushTabFactories(TabSet);
	
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FMapEditorGenAppMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FMapEditorGenAppMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}