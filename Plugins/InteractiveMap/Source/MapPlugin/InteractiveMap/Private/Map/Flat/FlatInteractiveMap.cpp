//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Flat/FlatInteractiveMap.h"
#include "Map/Flat/MapLimitComponent.h"
#include "Map/MapVisualComponent.h"
#include "Game/MapPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "InteractiveMap.h"
#include "Engine/World.h"
AFlatInteractiveMap::AFlatInteractiveMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LeftMapLimit = CreateDefaultSubobject<UMapLimitComponent>(TEXT("Limit L"));
	LeftMapLimit->SetupAttachment(RootComponent);
	LeftMapLimit->Attach(LeftMapLimit);

	MapVisualComponent = CreateDefaultSubobject<UMapVisualComponent>(TEXT("Map Visual"));
	MapVisualComponent->SetupAttachment(RootComponent);
	MapVisualComponent->AttachMeshes(RootComponent);

	RightMapLimit = CreateDefaultSubobject<UMapLimitComponent>(TEXT("Limit R"));
	RightMapLimit->SetupAttachment(RootComponent);
	RightMapLimit->Attach(RightMapLimit);

}
void AFlatInteractiveMap::BeginPlay()
{
	Super::BeginPlay();
}
UE_DISABLE_OPTIMIZATION
void AFlatInteractiveMap::InitializeMap_Implementation()
{
	Super::InitializeMap_Implementation();
	LeftMapLimit->CreateVisualComponent(MapVisualComponent->GetClass());
	if (!MapLookUpTexture)
		return;
	RightMapLimit->CreateVisualComponent(MapVisualComponent->GetClass());

	LeftMapLimit->GetVisualComponent()->InitVisualComponentFromOriginal(MapVisualComponent);
	RightMapLimit->GetVisualComponent()->InitVisualComponentFromOriginal(MapVisualComponent);

	FVector size = MapVisualComponent->CalculateSizeOfMesh(MapVisualComponent->GetMapGameplayMeshComponent());
	AMapPawn* pawn = Cast<AMapPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (pawn)
	{
		// Maybe try to find a way to make it indepedent of the map orientation
		// probably by applying the inverse of the actor transform to the vectto size
		// or make the user set the 
		// set vertical Limits of player movement

		pawn->SetVerticalMovementLimits(FVector2D(size.Y * 0.5, -size.Y * 0.5));
	}

	size /= GetActorScale();

	BoxPercentageOfMap = FMath::Clamp(BoxPercentageOfMap, 0.05, 0.5);

	LeftMapLimit->SetRelativeLocation(FVector(-size.X, 0, 0));
	RightMapLimit->SetRelativeLocation(FVector(size.X, 0, 0));

	LeftMapLimit->GetBoxComponent()->SetBoxExtent(FVector(size.X * BoxPercentageOfMap, size.Y, 100));
	RightMapLimit->GetBoxComponent()->SetBoxExtent(FVector(size.X * BoxPercentageOfMap, size.Y, 100));

}

UE_ENABLE_OPTIMIZATION
void AFlatInteractiveMap::SetMapMode_Implementation(const FName& Mode )
{
	Super::SetMapMode_Implementation(Mode);
}

void AFlatInteractiveMap::UpdateLimits()
{
	UMapVisualComponent* leftVisual = LeftMapLimit->GetVisualComponent();
	UMapVisualComponent* rightVisual = RightMapLimit->GetVisualComponent();

	if (!leftVisual)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Visual Component of left limit is not valid"));
		return;
	}

	if (!rightVisual)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Visual Component of right limit is not valid"));
		return;
	}

	leftVisual->UpdateVisualComponent(MapVisualComponent);
	rightVisual->UpdateVisualComponent(MapVisualComponent);

}
