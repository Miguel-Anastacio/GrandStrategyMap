//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Flat/FlatInteractiveMap.h"
#include "Map/Flat/MapLimitComponent.h"
#include "Map/MapVisualComponent.h"
#include "Game/MapPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
AFlatInteractiveMap::AFlatInteractiveMap()
	: AInteractiveMap()
{

	LeftMapLimit = CreateDefaultSubobject<UMapLimitComponent>(TEXT("Limit L"));
	LeftMapLimit->SetupAttachment(RootComponent);
	LeftMapLimit->Attach(LeftMapLimit);
	RightMapLimit = CreateDefaultSubobject<UMapLimitComponent>(TEXT("Limit R"));
	RightMapLimit->SetupAttachment(RootComponent);
	RightMapLimit->Attach(RightMapLimit);
}
void AFlatInteractiveMap::BeginPlay()
{
	Super::BeginPlay();

	// Maybe try to find a way to make it indepedent of the map orientation
	// probably by applying the inverse of the actor transform to the vectto size
	// or make the user set the 
	// set vertical Limits of player movement
	FVector size = MapVisualComponent->CalculateSizeOfMap();
	AMapPawn* pawn = Cast<AMapPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (pawn)
	{
		pawn->SetVerticalMovementLimits(FVector2D(size.Y * 0.5, -size.Y * 0.5));
	}

	LeftMapLimit->GetVisualComponent()->InitVisualComponentFromOriginal(MapVisualComponent);
	RightMapLimit->GetVisualComponent()->InitVisualComponentFromOriginal(MapVisualComponent);

	LeftMapLimit->SetRelativeLocation(FVector(-size.X, 0, 0));
	RightMapLimit->SetRelativeLocation(FVector(size.X, 0, 0));

	LeftMapLimit->GetBoxComponent()->SetBoxExtent(FVector(100, size.Y, 100));
	RightMapLimit->GetBoxComponent()->SetBoxExtent(FVector(100, size.Y, 100));


}

void AFlatInteractiveMap::SetMapMode_Implementation(MapMode mode)
{
	Super::SetMapMode_Implementation(mode);
	UMapVisualComponent* leftVisual = LeftMapLimit->GetVisualComponent();
	UMapVisualComponent* rightVisual = RightMapLimit->GetVisualComponent();

	UStaticMeshComponent* gameplayMesh = MapVisualComponent->GetMapGameplayMeshComponent();
	UStaticMeshComponent* terrainMesh = MapVisualComponent->GetMapTerrainMeshComponent();


	switch (mode)
	{
	case MapMode::POLITICAL:
		UpdateLimitComponent(leftVisual, mode, gameplayMesh);
		UpdateLimitComponent(rightVisual, mode, gameplayMesh);
		break;
	case MapMode::RELIGIOUS:
		UpdateLimitComponent(leftVisual, mode, gameplayMesh);
		UpdateLimitComponent(rightVisual, mode, gameplayMesh);
		break;
	case MapMode::CULTURAL:
		UpdateLimitComponent(leftVisual, mode, gameplayMesh);
		UpdateLimitComponent(rightVisual, mode, gameplayMesh);
		break;

	case MapMode::TERRAIN:
		leftVisual->InitMeshProperty(gameplayMesh, leftVisual->GetMapGameplayMeshComponent());
		leftVisual->InitMeshProperty(terrainMesh, leftVisual->GetMapTerrainMeshComponent());

		rightVisual->InitMeshProperty(gameplayMesh, rightVisual->GetMapGameplayMeshComponent());
		rightVisual->InitMeshProperty(terrainMesh, rightVisual->GetMapTerrainMeshComponent());
		break;
	default:
		break;
	}
}

void AFlatInteractiveMap::UpdateLimitComponent(UMapVisualComponent* mapLimitVisual, MapMode mode, UStaticMeshComponent* originalMesh)
{
	mapLimitVisual->InitMeshProperty(originalMesh, mapLimitVisual->GetMeshComponent(mode));
}