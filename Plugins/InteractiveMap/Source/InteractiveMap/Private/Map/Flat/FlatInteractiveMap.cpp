// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Flat/FlatInteractiveMap.h"
#include "Map/Flat/MapLimitComponent.h"
#include "Map/MapVisualComponent.h"
#include "Game/MapPawn.h"
#include "Kismet/GameplayStatics.h"
AFlatInteractiveMap::AFlatInteractiveMap()
	: AInteractiveMap()
{
	//LeftLimitComponent = CreateDefaultSubobject<UMapLimitComponent>(TEXT("Left Limit"));
	//LeftLimitComponent->SetupAttachment(MapRoot);

	//RightLimitComponent = CreateDefaultSubobject<UMapLimitComponent>(TEXT("Right Limit"));
	//RightLimitComponent->SetupAttachment(MapRoot);
}
UE_DISABLE_OPTIMIZATION
void AFlatInteractiveMap::BeginPlay()
{
	Super::BeginPlay();

	// Maybe try to find a way to make it indepedent of the map orientation
	// probably by applying the inverse of the actor transform to the vectto size
	// set vertical Limits of player movement
	FVector size = MapVisualComponent->CalculateSizeOfMap();
	AMapPawn* pawn = Cast<AMapPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (pawn)
	{
		pawn->SetVerticalMovementLimits(FVector2D(size.Y * 0.5, -size.Y * 0.5));
	}

	//LeftLimitComponent->InitLimitComponent(MapSelectMesh, MapBorderMesh, GameplayMapMesh, TerrainMapMesh);
	//RightLimitComponent->InitLimitComponent(MapSelectMesh, MapBorderMesh, GameplayMapMesh, TerrainMapMesh);

	
}
UE_ENABLE_OPTIMIZATION

//void AFlatInteractiveMap::SetMapMode(MapMode mode)
//{
//	Super::SetMapMode(mode);
//	//UMapVisualComponent* limitVisual = LeftLimitComponent->GetVisualComponent();
//	//limitVisual->UpdateMeshMaterial(limitVisual->GetMeshComponent(mode), GameplayMapMesh->GetMaterial(0));
//
//	////limitVisual = RightLimitComponent->GetVisualComponent();
//	//limitVisual->UpdateMeshMaterial(limitVisual->GetMeshComponent(mode), GameplayMapMesh->GetMaterial(0));
//}
