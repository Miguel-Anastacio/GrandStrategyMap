// Copyright 2025 An@stacioDev All rights reserved.
#include "MapEditorApp.h" 
#include "Editor/NameDefines.h"
#include "MapObject.h"
#include "Editor/MapEditorPreset.h"
#include "MapDataMode/MapEditorDataAppMode.h"
#include "MapEditorMode/MapEditorGenAppMode.h"

void FMapEditorApp::RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(tabManager);
}

void FMapEditorApp::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost,
	UObject* InObject)
{
	WorkingAsset = Cast<UMapObject>(InObject);
	MapGenPreset = NewObject<UMapEditorPreset>();

	AddApplicationMode(MapEditorGenModeName, MakeShareable(new FMapEditorGenAppMode(SharedThis(this))));
	AddApplicationMode(MapDataEditorModeName, MakeShareable(new FMapEditorDataAppMode(SharedThis(this))));
	
	TArray<UObject*> objectsToEdit{InObject};
	InitAssetEditor(
		Mode
		,InitToolkitHost
		,GetToolkitFName()
		,FTabManager::FLayout::NullLayout
		,true
		,true
		,objectsToEdit
		);
	AddToolbarExtender(); 
	
	SetCurrentMode(MapDataEditorModeName);
}

void FMapEditorApp::OnTexturePreviewClicked(FName ID) const
{
	if(GetCurrentMode() == MapEditorGenModeName)
	{
		UE_LOG(LogTemp, Display, TEXT("OnTexturePreviewClicked %s"), *ID.ToString());
	}
}

void FMapEditorApp::AddReferencedObjects(FReferenceCollector& Collector)
{
	if(MapGenPreset)
	{
		Collector.AddReferencedObject(MapGenPreset);
	}
	if(WorkingAsset)
	{
		Collector.AddReferencedObject(WorkingAsset);
	}
}

void FMapEditorApp::AddToolbarExtender()
{
	// Get the toolbar builder
	FToolMenuOwnerScoped OwnerScoped(this);
    
	// Create the main editor toolbar
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("AssetEditor.MapEditorApp.Toolbar");
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Default");
        
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(
			"SetMapDataEditor",
			FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					SetCurrentMode(MapDataEditorModeName);
				}),
				FCanExecuteAction().CreateLambda([this]() -> bool
				{
					return GetCurrentMode() != MapDataEditorModeName;
				}),
				FIsActionChecked::CreateLambda([this]() -> bool
				{
					return GetCurrentMode() == MapDataEditorModeName;
				})
			),
			NSLOCTEXT("MapEditor", "MapDataTabLabel", "Details"),
			NSLOCTEXT("MapEditor", "MapDataTabTooltip", "Show the details tab"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details")
		));

		Section.AddEntry(FToolMenuEntry::InitToolBarButton(
			"SetMapGenEditor",
			FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					SetCurrentMode(MapEditorGenModeName);
				}),
				FCanExecuteAction().CreateLambda([this]() -> bool
				{
					return !IsModeCurrent(MapEditorGenModeName);
				}),
				FIsActionChecked::CreateLambda([this]() -> bool
				{
					return IsModeCurrent(MapEditorGenModeName);
				})
			),
			NSLOCTEXT("MapEditor", "MapGenTabLabel", "Map Lookup Gen"),
			NSLOCTEXT("MapEditor", "MapGenTabTooltip", "Show Map Lookup Generator"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details")
		));
	}
}
