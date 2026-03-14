// Copyright 2025 An@stacioDev All rights reserved.
#include "MapDataEditorTabFactory.h"
#include "MapEditorDataAppMode.h"
#include "Asset/SCustomInstancedStructList.h"
#include "Editor/MapEditorApp.h"
#include "Editor/NameDefines.h"
#include "MapObject.h"

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

	// Why is this list widget wrapped in a scrollbox?
	return SNew(SScrollBox)
		.Orientation(Orient_Horizontal)
		.OnUserScrolled_Lambda([](float ScrollOffset) 
		{
		// Don't do anything - effectively disables programmatic scrolling from mouse wheel
		})
		.ConsumeMouseWheel(EConsumeMouseWheel::Never)
		+ SScrollBox::Slot()
		[
		SAssignNew(appMode->EditableStructListDisplay, SCustomInstancedStructList)
		.ListSource(&appMode->MyListItems)
		.StructTypes(&appMode->StructTypes)
		.NotEditableProperties(&appMode->PropertyNamesNotEditable)
		.PropertiesWithDropdown(&appMode->PropertiesWithDropDown)
		.MapObject(app->GetWorkingAsset())
		.OnSelectionChanged_Lambda([this, app, appMode](const TArray<int32>& IDs, const ESelectInfo::Type type)
		{
		   if(IDs.IsEmpty() && type == ESelectInfo::Type::OnMouseClick)
		   {
			   app->GetWorkingAsset()->ClearTilesSelected();
			   return;
		   }

		   const FInstancedStruct* Entry = app->GetWorkingAsset()->GetTileData(IDs.Last());
		   appMode->UpdateDataEntryEditor(*Entry, IDs.Last());
		   // on mouse click on list clear tiles selected and update highlight texture
		   if(type == ESelectInfo::Type::OnMouseClick)
		   {
			   app->GetWorkingAsset()->ClearTilesSelected(false);
			   app->GetWorkingAsset()->AddTilesSelected(IDs);
		   }
		})
		];
}

TSharedRef<SDockTab> FMapDataEditorTabFactory::SpawnTab(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedRef<SDockTab> DockTab = FWorkflowTabFactory::SpawnTab(Info);
	
	// Bind function to execute when tab is activated
	DockTab->SetOnTabActivated(
		SDockTab::FOnTabActivatedCallback::CreateSP(
			this,
			&FMapDataEditorTabFactory::HandleTabActivated));

	return DockTab;
}

FText FMapDataEditorTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return ViewMenuTooltip;
}

void FMapDataEditorTabFactory::SetAppMode(TSharedPtr<class FMapEditorDataAppMode> appMode)
{
	AppMode = appMode;
}

void FMapDataEditorTabFactory::HandleTabActivated(TSharedRef<SDockTab> Tab, ETabActivationCause Cause) const
{
	if (TSharedPtr<FMapEditorDataAppMode> Mode = AppMode.Pin())
	{
		Mode->EditableStructListDisplay->RebuildColumns(Mode->StructTypes);
	}
}
