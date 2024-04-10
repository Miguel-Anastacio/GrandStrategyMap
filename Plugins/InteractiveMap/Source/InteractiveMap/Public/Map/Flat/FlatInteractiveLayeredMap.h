// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Map/Flat/FlatInteractiveMap.h"
#include "FlatInteractiveLayeredMap.generated.h"
/**
 *  FlatMap base class -
 */
class UMapLimitComponent;
class UMapVisualComponent;
UCLASS(Blueprintable, BlueprintType)
class INTERACTIVEMAP_API AFlatInteractiveLayeredMap : public AFlatInteractiveMap
{
	GENERATED_BODY()
	//
public:
	AFlatInteractiveLayeredMap(const FObjectInitializer& ObjectInitializer);
	
	virtual void SetMapMode_Implementation(MapMode mode) override;
	virtual void InitializeMap_Implementation() override;
};