// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Map/Flat/FlatInteractiveMap.h"
#include "FlatInteractiveCombinedMap.generated.h"
/**
 *  FlatMap base class -
 */
class UMapLimitComponent;
class UMapVisualComponent;
UCLASS(Blueprintable, BlueprintType)
class INTERACTIVEMAP_API AFlatInteractiveCombinedMap : public AFlatInteractiveMap
{
	GENERATED_BODY()

public:
	AFlatInteractiveCombinedMap(const FObjectInitializer& ObjectInitializer);
	virtual void SetMapMode_Implementation(const FName& mode) override;
	virtual void InitializeMap_Implementation() override;
	//virtual void UpdateProvinceHovered(const FColor& color) override;

protected:
	//void InitializeMap() override;

	//UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	//TObjectPtr<class UTexture2D> TerrainTexture;

	//UPROPERTY(BlueprintReadWrite, Category = "Texture")
	//UMaterialInstanceDynamic* TerrainDynamicMaterial;

};

