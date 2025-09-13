#pragma once

#include "CoreMinimal.h"
#include "MapGenParamStructs.generated.h"

struct FBaseMapStruct;
DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);
USTRUCT(BlueprintType)
struct SHAREDMODULE_API FMapDetails
{
	GENERATED_BODY()
	// Number of tiles that the algorithm will try to create the actual number might be lower or higher
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin=1, ClampMax=100000), Category = "Map Details")
	int32 NumberOfTiles = 10;
	// seed to randomize starting tile centroids
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Map Details")
	int32 Seed = 10;
	// lloyd iterations performed to the voronoi diagram
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin=0, ClampMax=20), Category = "Map Details")
	int32 LloydIteration = 10;
	// How far around the algo will search for a neighbouring tile before adding a new one
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin=1, ClampMax=1000), Category = "Map Details")
	int32 SearchRadiusBeforeNewTile = 100;
};

USTRUCT(BlueprintType)
struct SHAREDMODULE_API FNoiseDetails
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Noise Details")
	int32 Seed = 12002943;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta=(ClampMin=0, ClampMax=16), Category = "Noise Details")
	int32 Octaves = 4;
	
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
struct SHAREDMODULE_API FMapGenParams
{
	GENERATED_BODY()
	// The starting texture, a black and white image (heightmap or mask map that you wish to use for your map)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General Settings" )
	UTexture2D* OriginTexture = nullptr;
	// Set to true if you wish to upload a texture with the borders for your map, only possible to upload for either land or ocean
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings", meta = (InlineEditConditionToggle))
	bool UploadBorder = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General Settings", meta=( EditCondition ="UploadBorder"))
	UTexture2D* BorderTexture = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	bool LandBelowCutoffHeight = false;

	// Value above which tiles are considered land/water (can be flipped with LandBelowCutoffHeight at true)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General Settings", meta=(ClampMin=0, ClampMax=1))
	float CutoffHeight = 0.001f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Land Settings")
	FMapDetails LandDetails;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ocean Settings")
	FMapDetails OceanDetails;

	// Settings of the noise that is applied to deform the borders between tiles
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Border Noise Settings")
	FNoiseDetails NoiseDetails;
	
};