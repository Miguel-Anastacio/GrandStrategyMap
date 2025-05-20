// Copyright 2025 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
// Application that owns the editor window
class FMapEditorApp : public  FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook, public FGCObject
{
public: // FWorkflowCentricApplication interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager) override;
	void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* InObject);

	class UMapObject* GetWorkingAsset() const { return WorkingAsset; }
	class UMapEditorPreset* GetMapGenPreset() const { return MapGenPreset; }
	
public: // FAssetEditorToolkit interface
	virtual FName GetToolkitFName() const override { return FName("MapEditorApp"); }
	virtual FText GetBaseToolkitName() const override { return FText::FromString("MapEditorApp"); }
	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("MapEditorApp"); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override {  return FLinearColor::White; }
	virtual FString GetDocumentationLink() const override { return TEXT(""); }
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override {}
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override {}

	TSharedPtr<class SMapObjectViewport> GetMapViewport() const
	{
		return MapViewport;
	}
	TSharedPtr<class STextureViewer> GetMapTexturePreview() const
	{
		return MapTexturePreview;
	}
	
private:
	TObjectPtr<class UMapObject> WorkingAsset = nullptr;
	TObjectPtr<class UMapEditorPreset> MapGenPreset = nullptr;


	TSharedPtr<class SMapObjectViewport> MapViewport = nullptr;
	TSharedPtr<class STextureViewer> MapTexturePreview = nullptr;
	/** FGCObject interface */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("FMapEditorApp");
	}
	
};
