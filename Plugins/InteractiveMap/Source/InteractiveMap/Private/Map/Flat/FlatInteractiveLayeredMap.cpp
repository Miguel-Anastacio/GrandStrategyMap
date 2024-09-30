//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Flat/FlatInteractiveLayeredMap.h"
#include "Map/Visual/LayeredMapVisualComponent.h"
#include "InteractiveMap.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"

AFlatInteractiveLayeredMap::AFlatInteractiveLayeredMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<ULayeredMapVisualComponent>(TEXT("Map Visual")))
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Script/Engine.MaterialInstanceConstant'/InteractiveMap/GSMap/InteractiveMap/Material/Instances/MI_GameplayMap.MI_GameplayMap'"));
	if(MaterialFinder.Succeeded())
		GameplayMapMaterial = MaterialFinder.Object;

}

void AFlatInteractiveLayeredMap::SetMapMode_Implementation(MapMode mode)
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
	UpdateLimits();
}

void AFlatInteractiveLayeredMap::InitializeMap_Implementation()
{
	Super::InitializeMap_Implementation();

	if (!bUseBorderMesh)
	{
		MapVisualComponent->GetMapBorderMeshComponent()->SetVisibility(false);
	}
	else if (!BorderMaterial)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Use border set to true but border material is null"));
	}
	else
	{
		UStaticMeshComponent* borderMesh = MapVisualComponent->GetMapBorderMeshComponent();
		UMaterialInstanceDynamic* material = UMaterialInstanceDynamic::Create(BorderMaterial, this);
		material->SetTextureParameterValue("LookUpTexture", MapLookUpTexture);
		borderMesh->SetMaterial(0, material);
	}

	UpdateLimits();
	MapInitializationDelegate.Broadcast(this);
}
