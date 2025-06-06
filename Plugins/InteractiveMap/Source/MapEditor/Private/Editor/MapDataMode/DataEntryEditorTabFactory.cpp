// Copyright 2025 An@stacioDev All rights reserved.
#include "DataEntryEditorTabFactory.h"
#include "Editor/InstancedStructWrapper.h"
#include "MapEditorDataAppMode.h"
#include "Editor/MapEditorApp.h"
#include "Editor/NameDefines.h"

FMapDataEntryEditorTabFactory::FMapDataEntryEditorTabFactory(TSharedPtr<class FMapEditorApp> app)
	: FWorkflowTabFactory(MapDataEntryEditorTabName, app), App(app)
{
	TabLabel = FText::FromString("Data Entry Editor");
	ViewMenuDescription = FText::FromString("Edit Map Object Data entry");
	ViewMenuTooltip = FText::FromString("");
}
// Set the contents of the tab
TSharedRef<SWidget> FMapDataEntryEditorTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	FDetailsViewArgs DetailsViewArgs;
	{
		DetailsViewArgs.bAllowSearch = false;
		DetailsViewArgs.bHideSelectionTip = true;
		DetailsViewArgs.bLockable = false;
		DetailsViewArgs.bSearchInitialKeyFocus = true;
		DetailsViewArgs.bUpdatesFromSelection = false;
		DetailsViewArgs.NotifyHook = nullptr;
		DetailsViewArgs.bShowOptions = true;
		DetailsViewArgs.bShowModifiedPropertiesOption = false;
		DetailsViewArgs.bShowScrollBar = false;
	}
    
	const TSharedRef<IDetailsView> DetailsView = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(DetailsViewArgs);
	FMapEditorDataAppMode* appMode = AppMode.Pin().Get();
	DetailsView->SetObjects(TArray<UObject*>{appMode->EntryWrapper});

	return SNew(SVerticalBox)
			// Add the Details View at the top
			+ SVerticalBox::Slot()
			.AutoHeight() // Let the Details View determine its own height
			[
				DetailsView
			];
}

FText FMapDataEntryEditorTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return ViewMenuTooltip;
}

void FMapDataEntryEditorTabFactory::SetAppMode(TSharedPtr<class FMapEditorDataAppMode> appMode)
{
	AppMode = appMode;
}
