// Copyright 2025 An@stacioDev All rights reserved.
#include "MapDataEditorTabFactory.h"

#include "MapEditorDataAppMode.h"
#include "Editor/MapEditorApp.h"
#include "Editor/NameDefines.h"

FMapDataEditorTabFactory::FMapDataEditorTabFactory(TSharedPtr<class FMapEditorApp> app)
	: FWorkflowTabFactory(MapDataEditorTabName, app), App(app)
{
	TabLabel = FText::FromString("Data Editor");
	ViewMenuDescription = FText::FromString("Edit Map Object Data");
	ViewMenuTooltip = FText::FromString("");
}
// Set the contents of the tab
TSharedRef<SWidget> FMapDataEditorTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(STextBlock).Text(FText::FromString("MAP DATA EDITOR PLACEHOLDER"));
}

FText FMapDataEditorTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return ViewMenuTooltip;
}
