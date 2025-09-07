// Copyright 2025 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "ThirdParty/MapGeneratorLibrary/MapGenerator/source/map/Map.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

// Application that owns the editor window
class FMapEditorApp : public  FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook, public FGCObject
{
public:
	
	virtual ~FMapEditorApp();
	// FWorkflowCentricApplication interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager) override;
	void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* InObject);

	// FEditorUndoClient interface
	virtual void PostUndo(bool bSuccess) override;

	class UMapObject* GetWorkingAsset() const { return WorkingAsset; }
	class UMapEditorPreset* GetMapGenPreset() const { return MapGenPreset; }

	void OnTexturePreviewClicked(FName ID);
	TArray<TPair<FName, UTexture2D*>> GetTexturesPairs() const;
	
public: // FAssetEditorToolkit interface
	virtual FName GetToolkitFName() const override { return FName("MapEditorApp"); }
	virtual FText GetBaseToolkitName() const override { return FText::FromString("MapEditorApp"); }
	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("MapEditorApp"); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override {  return FLinearColor::White; }
	virtual FString GetDocumentationLink() const override { return TEXT(""); }
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override {}
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override {}
	virtual void SaveAsset_Execute() override;
	
	TSharedPtr<class STextureViewer> MapTexturePreview = nullptr;
	TSharedPtr<class SMapObjectViewport> MapViewport = nullptr;

	void GenerateMap();
	void RestoreTexturePreview() const;
	void RestoreMapGenPreset() const;

	void SaveGeneratedMap();
	// MapDataEditor
	void UpdateEntrySelected(int32 Index) const;
	void ClearSelection() const;
	const UScriptStruct* GetFilterForDataList() const;

	static TObjectPtr<UTexture2D> CreateTexture(uint8* Buffer, unsigned Width, unsigned Height);

	// ====================================================
	// Update Viewport
	TWeakObjectPtr<UTexture2D> GetHighlightTexture() const;
	void UpdateHighlightTexture(const TArray<int32>& IDs);
	TWeakObjectPtr<UTexture2D> GetCurrentTexture() const;

private:
	/** FGCObject interface */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("FMapEditorApp");
	}
	void AddToolbarExtender();

	// Map GEN section
	static TObjectPtr<UTexture2D> CreateLookupTexture(const MapGenerator::TileMap& TileMap);
	static TObjectPtr<UTexture2D> CreateTextureSimple(uint8* Buffer, unsigned Width, unsigned Height);
	void SetMapObjectProperties(UMapObject* MapObject, UTexture2D* Texture, const FString& LookupFilePath, const FString& MapDataFilePath,
                               													UMaterialInstanceConstant* Material) const;
	void OutputLookupJson(const FString& FilePath) const;
	void OutputLookupGenFile(const FString& FilePath) const;
	void OutputStubMapDataJson(const FString& FilePath) const;
	UMaterialInstanceConstant* CreateMaterialInstanceAsset(UTexture2D* Texture, const FString& PackagePath) const;
	UTexture2D* CreateLookupTextureAsset(const FString& PackagePath) const;

	// =====================================
	// Data Editor
	void SetFilterForDataList(const UScriptStruct* Struct);
	// =====================================================
	// TexturePreview
	void LoadPreviewTexturesFromMapObject(const UMapObject* MapObject);
	TWeakObjectPtr<UTexture2D> GetLookupTexture() const;
	TWeakObjectPtr<UTexture2D> GetLookupLandTexture() const;
	TWeakObjectPtr<UTexture2D> GetLookupOceanTexture() const;
	TWeakObjectPtr<UTexture2D> GetRootTexture() const;
	TWeakObjectPtr<UTexture2D> GetBorderTexture() const;
	TWeakObjectPtr<UTexture2D> GetVisitedTilesTexture() const;
	
	void UpdatePreviewTextures(const MapGenerator::TileMap& TileMap);
	TSharedPtr<MapGenerator::Map> GetLastMapCreated() const;
	UTexture2D* GetLastRootTexture() const;
	
	TObjectPtr<class UMapObject> WorkingAsset = nullptr;
	TObjectPtr<class UMapEditorPreset> MapGenPreset = nullptr;


	UTexture2D* PreviewLookupTexture = nullptr;
	UTexture2D* PreviewLookupTextureLand = nullptr;
	UTexture2D* PreviewLookupTextureOcean = nullptr;
	UTexture2D* PreviewRootTexture = nullptr;
	UTexture2D* PreviewBorderTexture = nullptr;
	UTexture2D* PreviewVisitedTilesTexture = nullptr;
	TWeakObjectPtr<UTexture2D> CurrentTexture = nullptr;

	// Highlight
	UTexture2D* HighlightTexture = nullptr;
	

	TSharedPtr<MapGenerator::Map> TempMapGenerator = nullptr;

	struct FMapPreview
	{
		TSharedPtr<MapGenerator::Map> Map;
		UTexture2D* PreviewRootTexture;

		FMapPreview(const TSharedPtr<MapGenerator::Map>& map, UTexture2D* rootTexture)
			: Map(map), PreviewRootTexture(rootTexture)
		{
			
		}
	};

	TArray<FMapPreview> TempPreviews;

	const UScriptStruct* DataListStructFilter = nullptr;
	
};
