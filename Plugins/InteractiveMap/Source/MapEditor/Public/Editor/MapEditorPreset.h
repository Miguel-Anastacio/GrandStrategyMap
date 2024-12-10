// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapEditor/MapGenerator/source/map/components/HeightMap.h"
#include "MapEditorPreset.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);
USTRUCT(BlueprintType)
struct FMapDetails
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int NumberOfTiles = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Seed = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int LloydIteration = 10;
};

USTRUCT(BlueprintType)
struct FModuleDefinition
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* HeightMapTexture;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General Settings")
	float CutoffHeight = 0.001f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Land Settings")
	FMapDetails LandDetails;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ocean Settings")
	FMapDetails OceanDetails;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Border Settings")
	int Seed = 12002943;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Border Settings")
	int Octaves = 4;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Border Settings")
	float Frequency = 0.05f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Border Settings")
	float Scale = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Border Settings")
	float Lacunarity = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Border Settings")
	float LineThickness = 1.0f;
};

UCLASS()
class MAPEDITOR_API UMapEditorPreset : public UObject
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FModuleDefinition MapEditorDetails;
	
	FOnAssetChanged OnObjectChanged;

	MapGenerator::NoiseData GetNoiseData() const
	{
		return MapGenerator::NoiseData(MapEditorDetails.Seed, MapEditorDetails.Octaves, MapEditorDetails.Frequency, MapEditorDetails.Scale,
										MapEditorDetails.Lacunarity, 0.2);
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
		const uint32 width = MapEditorDetails.HeightMapTexture->GetSizeX();
		const uint32 height = MapEditorDetails.HeightMapTexture->GetSizeY();
		return MapGenerator::LookupMapData(GetNoiseData(), LandSettings(), OceanSettings(),
											width, height, MapEditorDetails.LineThickness, MapEditorDetails.CutoffHeight);
	}
	

};
