//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Globe/GlobeInteractiveLayeredMap.h"
#include "Map/Visual/LayeredMapVisualComponent.h"

AGlobeInteractiveLayeredMap::AGlobeInteractiveLayeredMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<ULayeredMapVisualComponent>(TEXT("Map Visual")))
{

}

void AGlobeInteractiveLayeredMap::SetMapMode_Implementation(MapMode mode)
{
	Super::SetMapMode_Implementation(mode);
	UStaticMeshComponent* mesh = MapVisualComponent->GetMapGameplayMeshComponent();
	UStaticMeshComponent* terrainMesh = MapVisualComponent->GetMapTerrainMeshComponent();
	switch (mode)
	{
	case MapMode::POLITICAL:
		mesh->SetVisibility(true);
		terrainMesh->SetVisibility(false);
		break;
	case MapMode::RELIGIOUS:
		mesh->SetVisibility(true);
		terrainMesh->SetVisibility(false);

		break;
	case MapMode::CULTURAL:
		mesh->SetVisibility(true);
		terrainMesh->SetVisibility(false);

		break;
	case MapMode::TERRAIN:
		mesh->SetVisibility(false);
		terrainMesh->SetVisibility(true);
		terrainMesh->SetMaterial(0, TerrainDynamicMaterial);


		break;
	default:
		break;
	}
}
