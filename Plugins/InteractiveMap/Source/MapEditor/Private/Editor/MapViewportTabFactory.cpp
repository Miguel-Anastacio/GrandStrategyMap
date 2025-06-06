// Copyright 2025 An@stacioDev All rights reserved.
#include "MapViewportTabFactory.h"
#include "Editor/MapEditorApp.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "MapObject.h"
#include "Asset/SMapObjectViewport.h"
#include "Editor/NameDefines.h"

FMapViewportTabFactory::FMapViewportTabFactory(TSharedPtr<FMapEditorApp> app)
	: FWorkflowTabFactory(MapEditorViewportTabName, app), App(app)
{
	TabLabel = FText::FromString("Viewport");
	// ViewMenuDescription = FText::FromString("View Menu");
	ViewMenuTooltip = FText::FromString("Viewport Menu Tooltip");
}
// Set the contents of the tab
TSharedRef<SWidget> FMapViewportTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	if(!App.IsValid())
		return SNew(SOverlay);
	
	return SNew(SOverlay)
	+ SOverlay::Slot()
	[
		SAssignNew(App.Pin().Get()->MapViewport, SMapObjectViewport)
		.EditingObject(App.Pin().Get()->GetWorkingAsset())
		.app(App)
	];
}

FText FMapViewportTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return ViewMenuTooltip;
}
