// Copyright 2025 An@stacioDev All rights reserved.
#include "MapEditorGenTabFactory.h"
#include "Editor/MapEditorApp.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Editor/MapEditorPreset.h"
#include "Editor/NameDefines.h"

FMapEditorGenTabFactory::FMapEditorGenTabFactory(TSharedPtr<class FMapEditorApp> app)
	: FWorkflowTabFactory(MapEditorGenDetailsTabName, app), App(app)
{
	TabLabel = FText::FromString("Main");
	ViewMenuDescription = FText::FromString("View Menu");
	ViewMenuTooltip = FText::FromString("View Menu Tooltip");
}
// Set the contents of the tab
TSharedRef<SWidget> FMapEditorGenTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
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
    DetailsView->SetObjects(TArray<UObject*>{app->GetMapGenPreset()});

	return SNew(SVerticalBox)
            // Add the Details View at the top
            + SVerticalBox::Slot()
            .AutoHeight() // Let the Details View determine its own height
            [
                DetailsView
            ]
            // Add a Button Below
            + SVerticalBox::Slot()
            .Padding(10.0f) // Add padding around the button
            .HAlign(HAlign_Center) // Center the button horizontally
            .VAlign(VAlign_Top)    // Align the button to the top of its slot
            .AutoHeight()          // Let the button determine its own height
            [
                SNew(SButton)
                .Text(FText::FromString("Click Me"))
                .OnClicked_Lambda([this]() -> FReply
                {
                    // Button click handler logic
                    ///GenerateMap();
                    return FReply::Handled();
                })
            ]
            + SVerticalBox::Slot()
            .Padding(10.0f) // Add padding around the button
            .HAlign(HAlign_Center) // Center the button horizontally
            .VAlign(VAlign_Top)    // Align the button to the top of its slot
            .AutoHeight()          // Let the button determine its own height
            [
                SNew(SButton)
                .Text(FText::FromString("Save"))
                .OnClicked_Lambda([this]() -> FReply
                {
                    //SaveMap();
                    return FReply::Handled();
                })
            ];
}

FText FMapEditorGenTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return ViewMenuTooltip;
}
