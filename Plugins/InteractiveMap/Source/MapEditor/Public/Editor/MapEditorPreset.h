// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapEditor/MapGenerator/source/map/components/HeightMap.h"
#include "MapEditorPreset.generated.h"

struct FBaseMapStruct;
DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);
USTRUCT(BlueprintType)
struct FMapDetails
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin=1, ClampMax=100000), Category = "Map Details")
	int NumberOfTiles = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Map Details")
	int Seed = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin=1, ClampMax=20), Category = "Map Details")
	int LloydIteration = 10;
};

USTRUCT(BlueprintType)
struct FNoiseDetails
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Noise Details")
	int Seed = 12002943;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta=(ClampMin=0, ClampMax=16), Category = "Noise Details")
	int Octaves = 4;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin=0, ClampMax=1), Category = "Noise Details")
	float Frequency = 0.05f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin=0.5, ClampMax=3), Category = "Noise Details")
	float Scale = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Noise Details")
	float Lacunarity = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin=0, ClampMax=7), Category = "Noise Details")
	float LineThickness = 1.0f;
};

USTRUCT(BlueprintType)
struct FModuleDefinition
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General Settings" )
	UTexture2D* HeightMapTexture;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General Settings", meta=(ClampMin=0, ClampMax=1))
	float CutoffHeight = 0.001f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Land Settings")
	FMapDetails LandDetails;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ocean Settings")
	FMapDetails OceanDetails;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Border Noise Settings")
	FNoiseDetails NoiseDetails;
	
};

UCLASS()
class MAPEDITOR_API UMapEditorPreset : public UObject
{
	GENERATED_BODY()
	

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Lookup Settings")
	FModuleDefinition MapEditorDetails;
	
	FOnAssetChanged OnObjectChanged;
	UPROPERTY()
	UMaterial* Material =nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	UScriptStruct* TileDataStructType = nullptr;;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	UScriptStruct* OceanTileDataType;

	UMapEditorPreset();
	
	MapGenerator::NoiseData GetNoiseData() const
	{
		return MapGenerator::NoiseData(MapEditorDetails.NoiseDetails.Seed, MapEditorDetails.NoiseDetails.Octaves, MapEditorDetails.NoiseDetails.Frequency,
										MapEditorDetails.NoiseDetails.Scale, MapEditorDetails.NoiseDetails.Lacunarity, 0.2);
	};
	MapGenerator::LookupFeatures LandSettings() const
	{
		return MapGenerator::LookupFeatures(MapEditorDetails.LandDetails.Seed, MapEditorDetails.LandDetails.NumberOfTiles,
							MapEditorDetails.LandDetails.LloydIteration);
	};
	MapGenerator::LookupFeatures OceanSettings() const
	{
		return MapGenerator::LookupFeatures(MapEditorDetails.OceanDetails.Seed, MapEditorDetails.OceanDetails.NumberOfTiles,
							MapEditorDetails.OceanDetails.LloydIteration);
	};

	MapGenerator::LookupMapData GetLookupMapData() const
	{
		const uint32 Width = MapEditorDetails.HeightMapTexture->GetSizeX();
		const uint32 Height = MapEditorDetails.HeightMapTexture->GetSizeY();
		return MapGenerator::LookupMapData(GetNoiseData(), LandSettings(), OceanSettings(),
											Width, Height, MapEditorDetails.NoiseDetails.LineThickness, MapEditorDetails.CutoffHeight);
	}
};
