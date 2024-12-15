#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
// #include "Editor/EditorWidgets/Public/SEditorViewport.h"
#include "IDetailsView.h"
#include "MapEditorPreset.h"
#include "PropertyEditorModule.h"
#include "Misc/Optional.h"
#include "Framework/Docking/TabManager.h"
#include "MapEditor/MapGenerator/source/map/Map.h"
#include "MapEditor/MapGenerator/source/map/components/HeightMap.h"

class STextureViewer;
class FAdvancedPreviewScene;
class UTexture2D;  
class MAPEDITOR_API MapEditorMenu  
{
public:
	MapEditorMenu();
	~MapEditorMenu();

	void RegisterTabs();
	TSharedRef<SDockTab> SpawnDetailsTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnViewport(const FSpawnTabArgs& Args);

	// UFUNCTION()
	void GenerateMap();
	void SaveMap();
private:
	UMapEditorPreset* MapEditorPreset;
	TSharedPtr<STextureViewer> TextureViewer;
	MapGenerator::Map Map;

	static TObjectPtr<UTexture2D> CreateLookupTexture(const MapGenerator::TileMap& TileMap);

	static TObjectPtr<UTexture2D> CreateTexture(uint8* buffer, unsigned width, unsigned height);
	
	static const uint8* ReadTextureToBuffer(UTexture2D* texture);

	static bool ValidateTexture(UTexture2D* texture);
	
	UPROPERTY()
	TObjectPtr<UTexture2D> LookupTexture;
	TObjectPtr<UTexture2D> LookupLandTexture;
	TObjectPtr<UTexture2D> LookupOceanTexture;
	TObjectPtr<UTexture2D> HeightMapTexture;

	

};