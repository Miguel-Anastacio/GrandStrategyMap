// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Map/ClickableMap.h"
#include "GlobeInteractiveMap.generated.h"

UCLASS()
class INTERACTIVEMAP_API AGlobeInteractiveMap : public AClickableMap
{
	GENERATED_BODY()

protected:
	AGlobeInteractiveMap(const FObjectInitializer& ObjectInitializer);

};

