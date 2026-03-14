// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 4, 4)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "VisualProperties.h"
#include "UObject/Object.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "Misc/Paths.h"
#include "UObject/ObjectSaveContext.h"
#include "Engine/Texture2D.h"
#if WITH_EDITOR
#include "source/map/Map.h"
#include "MapGenParamStructs.h"
#endif
#include <vector>


#include "MapObject.generated.h"
USTRUCT(BlueprintType)
struct FBaseMapStruct
{
	GENERATED_BODY()

	virtual ~FBaseMapStruct() {};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	int32 ID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name = "NO NAME";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Area = "NO AREA";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Region = "NO REGION";

	virtual FString ToString() const
	{
		return FString::Printf(TEXT("ID: %d - Name: %s - Area: %s - Region - %s"), ID, *Name, *Area, *Region);
	}
};

USTRUCT(BlueprintType)
struct FLookupEntry
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Color;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name;

	FString ToString() const
	{
		return FString::Printf(TEXT("ColorInHex: %s, Name: %s"), *Color, *Name);
	}
};

USTRUCT(BlueprintType)
struct FExampleStruct : public FBaseMapStruct
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Population = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Owner = "ProvinceOwner";
};

USTRUCT(BlueprintType)
struct FMapDataStruct : public FBaseMapStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Population = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Country = "POR";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Religion = "CAT";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Culture = "BAS";

	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("ID: %d - Name: %s, Population %d, Owner: %s"), ID, *Name, Population, *Country);
	}
};
USTRUCT(BlueprintType)
struct FPopulation : public FBaseMapStruct
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	int Size = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FName Profession;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FName Culture = "BRA";
};

USTRUCT(BlueprintType)
struct FComplexMapDataStruct : public FBaseMapStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FPopulation Population;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Country = "POR";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Religion = "CAT";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Culture = "BAS";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EClimate Climate = EClimate::Arctic;

	// virtual FString ToString() const override
	// {
	// 	return FString::Printf(TEXT("ID: %d - Name: %s, Population %d, Owner: %s"), ID, *Name, Population, *Country);
	// }
};

DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);
DECLARE_DELEGATE(FOnMapDetailsChange);
DECLARE_DELEGATE_OneParam(FOnTilesSelectedChanged, const TArray<int32>&);
UCLASS()
class SHAREDMODULE_API UMapObject : public UObject
{
	GENERATED_BODY()

	UMapObject(const FObjectInitializer& ObjectInitializer);
	// UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;
#endif
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
	virtual void PostLoad() override;
	virtual void Serialize(FArchive& Ar) override;
	// ======================================================
	
#if WITH_EDITOR
public:
	TSharedPtr<MapGenerator::Map> GetMapGen() const;
	void SetMapGen(TSharedPtr<MapGenerator::Map> MapGen);
	
	UTexture2D*  GetRootTexture() const;
	void SetRootTexture(UTexture2D* texture);

	TSharedPtr<MapGenerator::Map> GetLastMapGen() const;
	void SetLastMapGen(const TSharedPtr<MapGenerator::Map>& MapGen);
	
	FMapGenParams GetLastParamsUsed() const;
	void SetLastParamsUsed(const FMapGenParams& Params);
	
	bool IsMapSaved() const;
	void SetMapSaved(bool Saved);
	// counter of mapGens without saving 
	void IncrementCounter();
	// Map Gen serialization -> move somewhere else
	void SerializeMap(FArchive& Ar);

	void SaveData() const;
	void SetLookupTexture(UTexture2D *Texture2D);
	void SetMapDataFilePath(const FString &FilePath);
	void SetFilePathMapData(const FString &FilePath);
	void SetMapData(const TArray<FInstancedStruct>& NewData);
	void SetLookupFilePath(const FString &FilePath)
	{
		this->LookupFilePath = FPaths::CreateStandardFilename(FilePath);
		LoadLookupMap(FilePath);
	}

	// Update In Editor only, used in Map Object Display
	void UpdateDataInEditor(const FInstancedStruct &NewData, const TArray<int32>& IDs);
	
	void SetMaterialOverride(UMaterialInterface* MaterialInterface);
	UMaterialInterface* GetMaterialOverride() const;


	void AddTileSelected(int32 ID);
	void AddTilesSelected(const TArray<int32>& IDs);
	void RemoveTileSelected(int32 ID);
	void ClearTilesSelected(const bool triggerNotification = true);
	const TArray<int32>& GetTilesSelected() const;
	bool IsTileSelected(int32 ID) const;

	void ReplaceDataMap(const UScriptStruct* NewStruct, const UScriptStruct* OldStruct);
	bool ValidateStructChange(UScriptStruct* NewStruct, UScriptStruct* OldStruct);
	bool ProcessStructChange(const UScriptStruct* NewStruct, const UScriptStruct* OldStruct);

	void InitLandStructType(UScriptStruct* NewStruct);
	void InitOceanStructType(UScriptStruct* NewStruct);
#endif
	
public:
	TWeakObjectPtr<UTexture2D> GetLookupTexture() const;
	
	// Logging
	void LogLookupTable() const;
	void LogMapData() const;

	// Get type of tiles, checks DataStruct used
	bool IsTileWater(int32 ID) const;
	bool IsTileLand(int32 ID) const;
	bool IsStructValid(const FInstancedStruct &Struct) const;
	bool IsStructValid(const UScriptStruct *Struct) const;

	void LoadLookupMap(const FString &FilePath);

	FString GetFilePath() const
	{
		return this->FilePathMapData;
	}

	// Getters MapData
	TMap<int32, FInstancedStruct> &GetMapData() { return MapData; }
	TArray<FInstancedStruct> GetMapDataValue() const
	{
		TArray<FInstancedStruct> OutArray;
		MapData.GenerateValueArray(OutArray);
		return OutArray;
	}
	int32 FindID(const FColor &Color);
	int32 *FindIDPtr(const FColor &Color);
	FColor GetColor(int32 ID) const;
	FInstancedStruct *GetTileData(int32 ID);

	// Setter Map Data
	bool SetTileData(const FInstancedStruct &NewData, int32 ID);

	// Get Lookup
	TMap<FColor, int32> GetLookupTable() const
	{
		return LookupTable;
	};
	TArray<uint8> GetLookupTextureData();
	void LoadLookupTextureData();
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	UScriptStruct *StructType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	UScriptStruct *OceanStructType;
#if WITH_EDITORONLY_DATA
	FOnMapDetailsChange OnMapDetailsChange;
	FOnTilesSelectedChanged OnTilesSelectedChanged;
	FOnMapDetailsChange OnMapDataChanged;
#endif

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Lookup")
	TSoftObjectPtr<UTexture2D> LookupTexture;

	TArray<TObjectPtr<UVisualProperty>> GetVisualProperties();

	FColor GetPropertyColorFromInstancedStruct(const FInstancedStruct &InstancedStruct, const FName &PropertyName, bool &OutResult) const;

private:
	bool IsTileOfType(int32 ID, const UScriptStruct *ScriptStruct) const;
#if WITH_EDITOR
	bool UpdateDataInEditor(const FInstancedStruct &NewData, const int32 ID);
#endif

	UPROPERTY(EditAnywhere, Category="Visual Property")
	TArray<TSubclassOf<UVisualProperty>> VisualProperties;
	UPROPERTY()
	TArray<TObjectPtr<UVisualProperty>> VisualPropertyInstances;
	UPROPERTY()
	TMap<int32, FInstancedStruct> MapData;

	UPROPERTY()
	TMap<FColor, int32> LookupTable;

	UPROPERTY(VisibleAnywhere, Category = "Lookup")
	FString LookupFilePath;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	FString FilePathMapData;
	
	TArray<uint8> LookupTextureData;

#if WITH_EDITORONLY_DATA
	// Material used to apply to MapAsset in preview
	UPROPERTY(VisibleAnywhere, Category = "Debug")
	class UMaterialInterface *MaterialOverride;
	
	UPROPERTY(VisibleAnywhere, Category = "Debug")
	UScriptStruct *StructTypePrevious;
	UPROPERTY(VisibleAnywhere, Category = "Debug")
	UScriptStruct *OceanStructTypePrevious;
	
	// Used for Map Editor
	UPROPERTY()
	TSoftObjectPtr<UTexture2D> OriginTexture;

	UPROPERTY()
	FMapGenParams LastParamsUsedGen;

	// Keeps ref to last saved map gen object
	TSharedPtr<MapGenerator::Map> MapGenSaved;
	
	// Keeps ref to last created map gen object
	TSharedPtr<MapGenerator::Map> LastMapGen;

	UPROPERTY(VisibleAnywhere, Category = "Debug") // temp visibility
	bool bMapSaved = true;

	// Used for undo to work
	UPROPERTY(VisibleAnywhere, Category = "Debug") // temp visibility
	int Counter = 0;

	// Tracks tiles selected by user in editor
	// UPROPERTY(VisibleAnywhere) // temp the visibility
	TArray<int32> SelectedTiles;
#endif
	
};

#if WITH_EDITOR
static void SerializeTile(MapGenerator::Tile& Tile, FArchive& Ar);

template <typename T>
static void SerializeParallel(const int NumThreads, TArray<TArray<uint8>>& ThreadBuffers, std::vector<MapGenerator::Tile>& tiles)
{
	ParallelFor(NumThreads, [&](int32 Index)
	{
		TArray<uint8>& Buffer = ThreadBuffers[Index];
		T Target(Buffer);

		int32 ChunkSize = tiles.size() / NumThreads;
		int32 Start = Index * ChunkSize;
		int32 End = (Index == NumThreads - 1) ? tiles.size() : Start + ChunkSize;

		for (int32 i = Start; i < End; ++i)
		{
			SerializeTile(tiles[i], Target);  // Manual serialize per field
		}
	});
}
#endif