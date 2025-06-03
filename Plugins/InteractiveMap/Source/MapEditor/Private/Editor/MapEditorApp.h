// Copyright 2025 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "ThirdParty/MapGeneratorLibrary/MapGenerator/source/map/Map.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

// Application that owns the editor window
class FMapEditorApp : public  FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook, public FGCObject
{
public: // FWorkflowCentricApplication interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager) override;
	void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* InObject);

	class UMapObject* GetWorkingAsset() const { return WorkingAsset; }
	class UMapEditorPreset* GetMapGenPreset() const { return MapGenPreset; }

	void OnTexturePreviewClicked(FName ID) const;
	
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
	
private:
	/** FGCObject interface */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("FMapEditorApp");
	}
	void AddToolbarExtender();

	// Map GEN section
	TObjectPtr<UTexture2D> CreateLookupTexture(const MapGenerator::TileMap& TileMap);
	static TObjectPtr<UTexture2D> CreateTexture(uint8* Buffer, unsigned Width, unsigned Height);
	static TObjectPtr<UTexture2D> CreateTextureSimple(uint8* Buffer, unsigned Width, unsigned Height);
	void SetMapObjectProperties(UMapObject* MapObject, UTexture2D* Texture, const FString& LookupFilePath, const FString& MapDataFilePath,
                               													UMaterialInstanceConstant* Material) const;
	void OutputLookupJson(const FString& FilePath) const;
	void OutputLookupGenFile(const FString& FilePath) const;
	void OutputStubMapDataJson(const FString& FilePath) const;
	UMaterialInstanceConstant* CreateMaterialInstanceAsset(UTexture2D* Texture, const FString& PackagePath) const;
	UTexture2D* CreateLookupTextureAsset(const FString& PackagePath) const;
	// =====================================================
	// TexturePreview
	void LoadPreviewTexturesFromMapMapObject(const UMapObject* MapObject);
	TWeakObjectPtr<UTexture2D> GetLookupTexture() const;
	TWeakObjectPtr<UTexture2D> GetLookupLandTexture() const;
	TWeakObjectPtr<UTexture2D> GetLookupOceanTexture() const;
	TWeakObjectPtr<UTexture2D> GetRootTexture() const;
	
	
	TObjectPtr<class UMapObject> WorkingAsset = nullptr;
	TObjectPtr<class UMapEditorPreset> MapGenPreset = nullptr;


	UTexture2D* PreviewLookupTexture = nullptr;
	UTexture2D* PreviewLookupTextureLand = nullptr;
	UTexture2D* PreviewLookupTextureOcean = nullptr;
	UTexture2D* PreviewRootTexture = nullptr;

	TSharedPtr<MapGenerator::Map> TempMapGenerator = nullptr;
	
};
