#pragma once

#include "CoreMinimal.h"
#include "MapGenParamStructs.generated.h"

struct FBaseMapStruct;
DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);
USTRUCT(BlueprintType)
struct SHAREDMODULE_API FMapDetails
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin=1, ClampMax=100000), Category = "Map Details")
	int32 NumberOfTiles = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Map Details")
	int32 Seed = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin=1, ClampMax=20), Category = "Map Details")
	int32 LloydIteration = 10;
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
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General Settings" )
	UTexture2D* OriginTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings", meta = (InlineEditConditionToggle))
	bool UseHeightMap = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General Settings", meta=( EditCondition = "UseHeightMap", ClampMin=0, ClampMax=1))
	float CutoffHeight = 0.001f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Land Settings")
	FMapDetails LandDetails;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ocean Settings")
	FMapDetails OceanDetails;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Border Noise Settings")
	FNoiseDetails NoiseDetails;
	
};