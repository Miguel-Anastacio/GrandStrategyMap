//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Globe/GlobeInteractiveMap.h"
#include "Map/MapVisualComponent.h"
AGlobeInteractiveMap::AGlobeInteractiveMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MapVisualComponent = CreateDefaultSubobject<UMapVisualComponent>(TEXT("Map Visual"));
	MapVisualComponent->SetupAttachment(RootComponent);
	MapVisualComponent->AttachMeshes(RootComponent);

}