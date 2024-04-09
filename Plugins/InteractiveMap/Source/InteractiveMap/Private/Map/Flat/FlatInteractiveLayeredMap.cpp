//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Flat/FlatInteractiveLayeredMap.h"
#include "Map/Visual/LayeredMapVisualComponent.h"

AFlatInteractiveLayeredMap::AFlatInteractiveLayeredMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<ULayeredMapVisualComponent>(TEXT("Map Visual")))
{

}