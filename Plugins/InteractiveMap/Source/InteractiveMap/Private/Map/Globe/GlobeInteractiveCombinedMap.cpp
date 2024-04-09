//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Globe/GlobeInteractiveCombinedMap.h"
#include "Map/Visual/CombinedMapVisualComponent.h"
#include "Map/DynamicTextureComponent.h"
AGlobeInteractiveCombinedMap::AGlobeInteractiveCombinedMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCombinedMapVisualComponent>(TEXT("Map Visual")))
{

}

void AGlobeInteractiveCombinedMap::SetMapMode_Implementation(MapMode mode)
{
	Super::SetMapMode_Implementation(mode);
	UStaticMeshComponent* mesh = MapVisualComponent->GetMapGameplayMeshComponent();
	switch (mode)
	{
	case MapMode::POLITICAL:

		break;
	case MapMode::RELIGIOUS:

		break;
	case MapMode::CULTURAL:

		break;
	case MapMode::TERRAIN:

		if (mesh)
		{
			mesh->SetMaterial(0, TerrainDynamicMaterial);
		}
		break;
	default:
		break;
	}
}
