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

class UMapObject;

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
class MAPEDITOR_API RMapEditorMenu
{
public:
	RMapEditorMenu();
	 ~RMapEditorMenu();

	void RegisterTabs();
	TSharedRef<SDockTab> SpawnDetailsTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnViewport(const FSpawnTabArgs& Args);

	// UFUNCTION()
	void GenerateMap();
	void SaveMap() const;
private:
	UMapEditorPreset* MapEditorPreset = nullptr;
	TSharedPtr<STextureViewer> TextureViewer;
	MapGenerator::Map Map;

	static TObjectPtr<UTexture2D> CreateLookupTexture(const MapGenerator::TileMap& TileMap);

	static TObjectPtr<UTexture2D> CreateTexture(uint8* Buffer, unsigned Width, unsigned Height);

	void OutputLookupJson(const FString& FilePath) const;
	UTexture2D* CreateLookupTextureAsset(const FString& PackagePath) const;
	UMapObject* CreateMapObjectAsset(const FString& PackagePath, UTexture2D* Texture, const FString& LookupFilePath, UMaterialInstanceConstant* Material) const;
	UMaterialInstanceConstant* CreateMaterialInstanceAsset(UTexture2D* Texture, const FString& PackagePath) const;
	
	TObjectPtr<UTexture2D> LookupTexture;
	TObjectPtr<UTexture2D> LookupLandTexture;
	TObjectPtr<UTexture2D> LookupOceanTexture;
	TObjectPtr<UTexture2D> HeightMapTexture;
};