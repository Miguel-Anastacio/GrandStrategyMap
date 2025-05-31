// Copyright 2025 An@stacioDev All rights reserved.
#include "MapDataEditorTabFactory.h"

#include "Editor/InstancedStructWrapper.h"
#include "MapEditorDataAppMode.h"
#include "Asset/SCustomInstancedStructList.h"
#include "Editor/MapEditorApp.h"
#include "Editor/NameDefines.h"
#include "MapObject.h"
#include "Asset/SMapObjectViewport.h"

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
	FMapEditorDataAppMode* appMode = AppMode.Pin().Get();
	FMapEditorApp* app = App.Pin().Get();
	return SAssignNew(appMode->EditableStructListDisplay, SCustomInstancedStructList)
		.ListSource(&appMode->MyListItems)
		.StructTypes(&appMode->StructTypes)
		.NotEditableProperties(&appMode->PropertyNamesNotEditable)
		.PropertiesWithDropdown(&appMode->PropertiesWithDropDown)
		.MapObject(app->GetWorkingAsset())
		.OnItemChanged_Lambda([this, app](const FInstancedStruct& Item)
		{
			app->GetWorkingAsset()->UpdateDataInEditor(Item);
		})
		.OnSelectionChanged_Lambda([this, app, appMode](const int32 ID)
		{
			app->MapViewport->UpdatePreviewActor(ID);
			FInstancedStruct* Entry = app->GetWorkingAsset()->GetTileData(ID);
			appMode->EntryWrapper->StructInstance = *Entry;
			appMode->EntryWrapper->ID = ID;
		});
}

FText FMapDataEditorTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return ViewMenuTooltip;
}

void FMapDataEditorTabFactory::SetAppMode(TSharedPtr<class FMapEditorDataAppMode> appMode)
{
	AppMode = appMode;
}
