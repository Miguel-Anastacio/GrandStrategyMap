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
UCLASS()
class INTERACTIVEMAP_API AFlatInteractiveLayeredMap : public AFlatInteractiveMap
{
	GENERATED_BODY()
	//
	protected:
		AFlatInteractiveLayeredMap(const FObjectInitializer& ObjectInitializer);
};