// Copyright 2025 An@stacioDev All rights reserved.
#include "MapObjectDetailsTabFactory.h"
#include "Editor/MapEditorApp.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "MapObject.h"
#include "Editor/NameDefines.h"

FMapObjectDetailsTabFactory::FMapObjectDetailsTabFactory(TSharedPtr<class FMapEditorApp> app)
	: FWorkflowTabFactory(MapObjectDetailsTabName, app), App(app)
{
	TabLabel = FText::FromString("Map Object Details");
	ViewMenuDescription = FText::FromString("View Menu");
	ViewMenuTooltip = FText::FromString("View Menu Tooltip");
}
// Set the contents of the tab
TSharedRef<SWidget> FMapObjectDetailsTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
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
    const FMapEditorApp* app = App.Pin().Get();
    DetailsView->SetObjects(TArray<UObject*>{app->GetWorkingAsset()});

	return	SNew(SScrollBox)
			.Orientation(Orient_Vertical)
			+ SScrollBox::Slot()
			[
				 SNew(SVerticalBox)
	            // Add the Details View at the top
	            + SVerticalBox::Slot()
	            .AutoHeight() // Let the Details View determine its own height
	            [
	                DetailsView
	            ]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Left)
				.Padding(5)
				[
					SNew(SButton)
					.Text(FText::FromString("Load Data file"))
					.OnClicked_Lambda([this]() -> FReply
					{
						// OnLoadFile();
						return FReply::Handled();
					})
				]
			];	
}

FText FMapObjectDetailsTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return ViewMenuTooltip;
}
