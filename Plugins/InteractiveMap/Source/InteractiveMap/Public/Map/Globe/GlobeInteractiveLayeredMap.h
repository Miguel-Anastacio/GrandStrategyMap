// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Map/Globe/GlobeInteractiveMap.h"
#include "GlobeInteractiveLayeredMap.generated.h"
UCLASS(Blueprintable, BlueprintType)
class INTERACTIVEMAP_API AGlobeInteractiveLayeredMap : public AGlobeInteractiveMap
{
	GENERATED_BODY()
	//
protected:
	AGlobeInteractiveLayeredMap(const FObjectInitializer& ObjectInitializer);

public:
	void SetMapMode_Implementation(MapMode mode) override;
	virtual void InitializeMap() override;
};