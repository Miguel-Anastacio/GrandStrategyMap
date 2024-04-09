// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Map/Globe/GlobeInteractiveMap.h"
#include "GlobeInteractiveCombinedMap.generated.h"

UCLASS()
class INTERACTIVEMAP_API AGlobeInteractiveCombinedMap : public AGlobeInteractiveMap
{
	GENERATED_BODY()

public:
	AGlobeInteractiveCombinedMap(const FObjectInitializer& ObjectInitializer);
	virtual void SetMapMode_Implementation(MapMode mode) override;

protected:
	//UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	//TObjectPtr<class UTexture2D> TerrainTexture;
};

