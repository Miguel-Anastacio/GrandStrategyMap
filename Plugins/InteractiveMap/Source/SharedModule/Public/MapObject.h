// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 5, 0)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "VisualProperties.h"
#include "UObject/Object.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "Misc/Paths.h"
#if WITH_EDITOR
#include "source/map/Map.h"
#include "MapGenParamStructs.h"
#endif


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

UENUM(BlueprintType)
enum class EClimate : uint8
{
	Arctic,
	Desert,
	Jungle,
	Swamp,
	Savana,
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
	FMapGenParams GetLastParamsUsed() const;
	void SetLastParamsUsed(const FMapGenParams& Params);
	bool IsMapSaved() const;
	void SetMapSaved(bool Saved);
	// counter of mapGens without saving 
	void IncrementCounter();
	// Map Gen serialization -> move somewhere else
	void SerializeMap(FArchive& Ar);
	// static void SerializeTile(MapGenerator::Tile& Tile, FArchive& Ar);

	void SaveData() const;
	void LoadDataFromFile();
	void SetLookupTexture(UTexture2D *Texture2D);
	TWeakObjectPtr<UTexture2D> GetLookupTexture() const;
	void SetMapDataFilePath(const FString &FilePath, bool LoadFromFile = true);
	void SetLookupFilePath(const FString &FilePath)
	{
		this->LookupFilePath = FPaths::CreateStandardFilename(FilePath);
		LoadLookupMap(FilePath);
	}

	// Update In Editor only, used in Map Object Display
	void UpdateDataInEditor(const FInstancedStruct &NewData, const int32 ID);
	void UpdateDataInEditor(const FInstancedStruct &NewData, const TArray<int32>& IDs);
	
	void SetMaterialOverride(UMaterialInterface* MaterialInterface);
	UMaterialInterface* GetMaterialOverride() const;

	UDataTable* GetVisualPropertyTypes() const;

	void AddTileSelected(int32 ID);
	void RemoveTileSelected(int32 ID);
	void ClearTilesSelected();
	const TArray<int32>& GetTilesSelected() const;
	bool IsTileSelected(int32 ID) const;
#endif
	
public:	
	// Logging
	void LogLookupTable() const;
	void LogMapData() const;
	void LogVisualProperties() const;

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
	const TArray<uint8> &GetLookupTextureData() const
	{
		return LookupTextureData;
	}
	FColor GetColorFromUv(const FVector2D &Uv) const;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	UScriptStruct *StructType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	UScriptStruct *OceanStructType;

	FOnAssetChanged OnObjectChanged;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lookup")
	class UTexture2D *LookupTexture;

	const TMap<FVisualPropertyType, FArrayOfVisualProperties> &GetVisualPropertiesMap() const;
	TMap<FName, FArrayOfVisualProperties> GetVisualPropertyNameMap() const;

	FColor GetPropertyColorFromInstancedStruct(const FInstancedStruct &InstancedStruct, const FName &PropertyName, bool &OutResult) const;
	void ReadDataTables();
	FVisualProperty GetVisualProperty(const FName &Type, const FName &Tag, bool &OutResult) const;
	FVisualProperty GetVisualProperty(const FVisualPropertyType &Type, const FName &Tag, bool &OutResult) const;

	TArray<FName> GetVisualPropertiesNamesOfType(const FName &Type) const;
	TSet<FName> GetNamesOfVisualPropertiesInMapData() const;
	
private:
	bool IsTileOfType(int32 ID, const UScriptStruct *ScriptStruct) const;

	UPROPERTY()
	TMap<FVisualPropertyType, FArrayOfVisualProperties> VisualPropertiesMap;

	UPROPERTY()
	TMap<int32, FInstancedStruct> MapData;

	UPROPERTY()
	TMap<FColor, int32> LookupTable;

	UPROPERTY(VisibleAnywhere, Category = "Lookup")
	FString LookupFilePath;

	UPROPERTY()
	TArray<uint8> LookupTextureData;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	FString FilePathMapData;

	// UPROPERTY
#if WITH_EDITORONLY_DATA
	/** Data table for visual property types */
	UPROPERTY(EditAnywhere, Category = "Data", DisplayName = "Visual Property Types", meta = (RequiredAssetDataTags = "RowStructure=/Script/SharedModule.VisualPropertyType"))
	UDataTable *VisualPropertyTypesDT;

	/** Data table for visual properties */
	UPROPERTY(EditAnywhere, Category = "Data", DisplayName = "Visual Properties", meta = (RequiredAssetDataTags = "RowStructure=/Script/SharedModule.VisualProperty"))
	class UDataTable *VisualPropertiesDT;
	// Material used to apply to MapAsset in preview
	UPROPERTY(EditAnywhere, Category = "Lookup")
	class UMaterialInterface *MaterialOverride;

	// Used for Map Editor
	UPROPERTY()
	UTexture2D* OriginTexture;

	UPROPERTY()
	FMapGenParams LastParamsUsedGen;
	
	TSharedPtr<MapGenerator::Map> Map;

	UPROPERTY(VisibleAnywhere) // temp visibility
	bool bMapSaved = true;

	// Used for undo to work
	UPROPERTY(VisibleAnywhere) // temp visibility
	int Counter = 0;

	// Tracks tiles selected by user in editor
	// UPROPERTY(VisibleAnywhere) // temp the visibility
	TArray<int32> SelectedTiles;
#endif
	
};

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
