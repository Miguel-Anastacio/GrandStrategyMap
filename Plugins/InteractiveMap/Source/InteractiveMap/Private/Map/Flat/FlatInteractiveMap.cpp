//  Copyright 2024 An@stacioDev All rights reserved.

#include "Map/Flat/FlatInteractiveMap.h"
#include "Map/Flat/MapLimitComponent.h"
#include "Map/MapVisualComponent.h"
#include "Game/MapPawn.h"
#include "Kismet/GameplayStatics.h"
AFlatInteractiveMap::AFlatInteractiveMap()
	: AInteractiveMap()
{

}
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

	
}

