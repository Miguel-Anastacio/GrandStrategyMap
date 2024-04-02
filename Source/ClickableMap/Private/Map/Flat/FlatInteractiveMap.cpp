// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Flat/FlatInteractiveMap.h"
#include "Map/Flat/MapLimitComponent.h"
#include "Map/MapVisualComponent.h"

AFlatInteractiveMap::AFlatInteractiveMap()
	: AInteractiveMap()
{
	//LeftLimitComponent = CreateDefaultSubobject<UMapLimitComponent>(TEXT("Left Limit"));
	//LeftLimitComponent->SetupAttachment(MapRoot);

	//RightLimitComponent = CreateDefaultSubobject<UMapLimitComponent>(TEXT("Right Limit"));
	//RightLimitComponent->SetupAttachment(MapRoot);
}

void AFlatInteractiveMap::BeginPlay()
{
	Super::BeginPlay();

	//LeftLimitComponent->InitLimitComponent(MapSelectMesh, MapBorderMesh, GameplayMapMesh, TerrainMapMesh);
	//RightLimitComponent->InitLimitComponent(MapSelectMesh, MapBorderMesh, GameplayMapMesh, TerrainMapMesh);

	
}

//void AFlatInteractiveMap::SetMapMode(MapMode mode)
//{
//	Super::SetMapMode(mode);
//	//UMapVisualComponent* limitVisual = LeftLimitComponent->GetVisualComponent();
//	//limitVisual->UpdateMeshMaterial(limitVisual->GetMeshComponent(mode), GameplayMapMesh->GetMaterial(0));
//
//	////limitVisual = RightLimitComponent->GetVisualComponent();
//	//limitVisual->UpdateMeshMaterial(limitVisual->GetMeshComponent(mode), GameplayMapMesh->GetMaterial(0));
//}
