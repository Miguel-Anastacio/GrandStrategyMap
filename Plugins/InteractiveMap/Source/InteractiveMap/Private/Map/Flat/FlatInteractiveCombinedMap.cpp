//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Flat/FlatInteractiveCombinedMap.h"
#include "Map/Flat/MapLimitComponent.h"
#include "Map/Visual/CombinedMapVisualComponent.h"
#include "Game/MapPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
AFlatInteractiveCombinedMap::AFlatInteractiveCombinedMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCombinedMapVisualComponent>(TEXT("Map Visual")))
{

}
//void AFlatInteractiveMap::BeginPlay()
//{
//	Super::BeginPlay();
//}
//UE_DISABLE_OPTIMIZATION
//void AFlatInteractiveMap::InitializeMap()
//{
//	Super::InitializeMap();
//	// Maybe try to find a way to make it indepedent of the map orientation
//	// probably by applying the inverse of the actor transform to the vectto size
//	// or make the user set the 
//	// set vertical Limits of player movement
//	if (!MapLookUpTexture)
//		return;
//	LeftMapLimit->GetVisualComponent()->InitVisualComponentFromOriginal(MapVisualComponent);
//	RightMapLimit->GetVisualComponent()->InitVisualComponentFromOriginal(MapVisualComponent);
//
//	FVector size = MapVisualComponent->CalculateSizeOfMesh(MapVisualComponent->GetMapGameplayMeshComponent());
//	AMapPawn* pawn = Cast<AMapPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
//	if (pawn)
//	{
//		pawn->SetVerticalMovementLimits(FVector2D(size.Y * 0.5, -size.Y * 0.5));
//	}
//
//	size /= GetActorScale();
//
//	BoxPercentageOfMap = FMath::Clamp(BoxPercentageOfMap, 0.05, 0.5);
//
//	LeftMapLimit->SetRelativeLocation(FVector(-size.X, 0, 0));
//	RightMapLimit->SetRelativeLocation(FVector(size.X, 0, 0));
//
//	LeftMapLimit->GetBoxComponent()->SetBoxExtent(FVector(size.X * BoxPercentageOfMap, size.Y, 100));
//	RightMapLimit->GetBoxComponent()->SetBoxExtent(FVector(size.X * BoxPercentageOfMap, size.Y, 100));
//
//}
//
//UE_ENABLE_OPTIMIZATION
//void AFlatInteractiveMap::SetMapMode_Implementation(MapMode mode)
//{
//	Super::SetMapMode_Implementation(mode);
//	UMapVisualComponent* leftVisual = LeftMapLimit->GetVisualComponent();
//	UMapVisualComponent* rightVisual = RightMapLimit->GetVisualComponent();
//
//	UStaticMeshComponent* gameplayMesh = MapVisualComponent->GetMapGameplayMeshComponent();
//	UStaticMeshComponent* terrainMesh = MapVisualComponent->GetMapTerrainMeshComponent();
//
//
//	/*switch (mode)
//	{
//	case MapMode::POLITICAL:
//		UpdateLimitComponent(leftVisual, mode, gameplayMesh);
//		UpdateLimitComponent(rightVisual, mode, gameplayMesh);
//		break;
//	case MapMode::RELIGIOUS:
//		UpdateLimitComponent(leftVisual, mode, gameplayMesh);
//		UpdateLimitComponent(rightVisual, mode, gameplayMesh);
//		break;
//	case MapMode::CULTURAL:
//		UpdateLimitComponent(leftVisual, mode, gameplayMesh);
//		UpdateLimitComponent(rightVisual, mode, gameplayMesh);
//		break;
//
//	case MapMode::TERRAIN:
//		leftVisual->InitMeshProperty(gameplayMesh, leftVisual->GetMapGameplayMeshComponent());
//		leftVisual->InitMeshProperty(terrainMesh, leftVisual->GetMapTerrainMeshComponent());
//
//		rightVisual->InitMeshProperty(gameplayMesh, rightVisual->GetMapGameplayMeshComponent());
//		rightVisual->InitMeshProperty(terrainMesh, rightVisual->GetMapTerrainMeshComponent());
//		break;
//	default:
//		break;
//	}*/
//
//	leftVisual->InitMeshProperty(gameplayMesh, leftVisual->GetMapGameplayMeshComponent());
//	leftVisual->InitMeshProperty(terrainMesh, leftVisual->GetMapTerrainMeshComponent());
//
//	rightVisual->InitMeshProperty(gameplayMesh, rightVisual->GetMapGameplayMeshComponent());
//	rightVisual->InitMeshProperty(terrainMesh, rightVisual->GetMapTerrainMeshComponent());
//}
//
//void AFlatInteractiveMap::UpdateLimitComponent(UMapVisualComponent* mapLimitVisual, MapMode mode, UStaticMeshComponent* originalMesh)
//{
//	mapLimitVisual->InitMeshProperty(originalMesh, mapLimitVisual->GetMeshComponent(mode));
//}