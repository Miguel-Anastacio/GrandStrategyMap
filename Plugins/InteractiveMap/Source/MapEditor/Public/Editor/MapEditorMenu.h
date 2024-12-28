#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IDetailsView.h"
#include "MapEditorPreset.h"
#include "PropertyEditorModule.h"
#include "Misc/Optional.h"
#include "Framework/Docking/TabManager.h"
#include "MapEditor/MapGenerator/source/map/Map.h"
#include "MapEditorMenu.generated.h"

USTRUCT(NotBlueprintType)
struct FTileIdData 
{
	GENERATED_BODY()
	FTileIdData()  : ColorString(TEXT("")), Name(-1) {};
	FTileIdData(const char* color, int32 id) :  ColorString(color), Name(id) {};

	UPROPERTY()
	FString ColorString = "";
	UPROPERTY()
	int32 Name = -1;

	// Serialize to JSON
	void SerializeToJson(TSharedPtr<FJsonObject>& OutJsonObject) const
	{
		OutJsonObject->SetStringField(TEXT("Name"), FString::FromInt(Name));
		OutJsonObject->SetStringField(TEXT("Color"), ColorString);
	}
};

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
	void SaveMap() const;
private:
	UMapEditorPreset* MapEditorPreset;
	TSharedPtr<STextureViewer> TextureViewer;
	MapGenerator::Map Map;

	static TObjectPtr<UTexture2D> CreateLookupTexture(const MapGenerator::TileMap& TileMap);

	static TObjectPtr<UTexture2D> CreateTexture(uint8* buffer, unsigned width, unsigned height);
	
	static bool ValidateTexture(UTexture2D* texture);
	
	TObjectPtr<UTexture2D> LookupTexture;
	TObjectPtr<UTexture2D> LookupLandTexture;
	TObjectPtr<UTexture2D> LookupOceanTexture;
	TObjectPtr<UTexture2D> HeightMapTexture;
};