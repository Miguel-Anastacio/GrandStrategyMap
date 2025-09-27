// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "source/map/components/HeightMap.h"
#include "MapGenParamStructs.h"
#include "MapEditorPreset.generated.h"

struct FBaseMapStruct;
DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);

USTRUCT(NotBlueprintType)
struct FMapLookupGenFeedback
{
	GENERATED_BODY()
	UPROPERTY()
	FMapDetails LandDetails;
	UPROPERTY()
	FMapDetails OceanDetails;
	UPROPERTY()
	FNoiseDetails NoiseDetails;
	UPROPERTY()
	float CutoffHeight = 0.001f;
	UPROPERTY()
	uint32 LandTiles = 0;
	UPROPERTY()
	uint32 OceanTiles = 0;
	
	FMapLookupGenFeedback(const FMapGenParams& MapParams, uint32 LTiles, uint32 OTiles) :
		LandDetails(MapParams.LandDetails),
		OceanDetails(MapParams.OceanDetails),
		NoiseDetails(MapParams.NoiseDetails),
		CutoffHeight(MapParams.CutoffHeight),
		LandTiles(LTiles),
		OceanTiles(OTiles)
	{
		
	}
	FMapLookupGenFeedback(){};
	
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
	FMapGenParams MapEditorDetails;
	
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
							MapEditorDetails.LandDetails.LloydIteration, MapEditorDetails.LandDetails.SearchRadiusBeforeNewTile);
	};
	MapGenerator::LookupFeatures OceanSettings() const
	{
		return MapGenerator::LookupFeatures(MapEditorDetails.OceanDetails.Seed, MapEditorDetails.OceanDetails.NumberOfTiles,
							MapEditorDetails.OceanDetails.LloydIteration, MapEditorDetails.LandDetails.SearchRadiusBeforeNewTile);
	};

	MapGenerator::LookupMapData GetLookupMapData() const
	{
		const uint32 Width = MapEditorDetails.OriginTexture->GetSizeX();
		const uint32 Height = MapEditorDetails.OriginTexture->GetSizeY();
		const float CutOffHeight =  MapEditorDetails.CutoffHeight;
		return MapGenerator::LookupMapData(GetNoiseData(), LandSettings(), OceanSettings(),
											Width, Height, MapEditorDetails.NoiseDetails.LineThickness,
											CutOffHeight, MapEditorDetails.LandBelowCutoffHeight);
	}

	UTexture2D* GetBorderTexture() const
	{
		return MapEditorDetails.BorderTexture;
	}
	
	bool UploadBorder() const
	{
		return (MapEditorDetails.BorderTexture != nullptr) && MapEditorDetails.UploadBorder;
	}

	bool BorderMatchesOriginTexture() const
	{
		if (MapEditorDetails.BorderTexture == nullptr || MapEditorDetails.OriginTexture == nullptr)
			return false;
		
		const int32 sizeOriginX = MapEditorDetails.OriginTexture->GetSizeX();
		const int32 sizeOriginY = MapEditorDetails.OriginTexture->GetSizeY();

		const int32 sizeBorderX = MapEditorDetails.BorderTexture->GetSizeX();
		const int32 sizeBorderY = MapEditorDetails.BorderTexture->GetSizeY();

		return (sizeOriginX == sizeBorderX && sizeOriginY == sizeBorderY);
	}
};
