// Copyright 2025 An@stacioDev All rights reserved.
#include "MapEditorTexturePreviewFactory.h"

#include "MapEditorGenAppMode.h"
#include "Editor/MapEditorApp.h"
#include "Editor/NameDefines.h"
#include "Editor/SMapTextureViewer.h"

FMapEditorTexturePreviewFactory::FMapEditorTexturePreviewFactory(TSharedPtr<class FMapEditorApp> app)
	: FWorkflowTabFactory(MapEditorPreviewTabName, app), App(app)
{
	TabLabel = FText::FromString("Preview");
	ViewMenuDescription = FText::FromString("Preview View Menu");
	ViewMenuTooltip = FText::FromString("");
}
// Set the contents of the tab
TSharedRef<SWidget> FMapEditorTexturePreviewFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	FMapEditorApp* app = App.Pin().Get();
	return SAssignNew(app->MapTexturePreview, STextureViewer)
		.OnTextureSelected_Raw(app, &FMapEditorApp::OnTexturePreviewClicked);
}

FText FMapEditorTexturePreviewFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return ViewMenuTooltip;
}
