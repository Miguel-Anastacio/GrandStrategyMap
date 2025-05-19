// Copyright 2025 An@stacioDev All rights reserved.
#include "MapEditorApp.h" 
#include "Editor/NameDefines.h"
#include "MapObject.h"
#include "Editor/MapEditorPreset.h"
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
	
	TArray<UObject*> objectsToEdit{InObject};
	InitAssetEditor(
		Mode
		,InitToolkitHost
		,TEXT("MapObjectEditorNew")
		,FTabManager::FLayout::NullLayout
		,true
		,true
		,objectsToEdit
		);


	AddApplicationMode(MapEditorGenModeName, MakeShareable(new FMapEditorGenAppMode(SharedThis(this))));
	SetCurrentMode(MapEditorGenModeName);
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
