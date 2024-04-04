// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/Flat/MapLimitComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Map/MapVisualComponent.h"
#include "Game/MapPawn.h"
// Sets default values for this component's properties
UMapLimitComponent::UMapLimitComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	

	Box = CreateDefaultSubobject<UBoxComponent>("Box Limit");
	Box->SetupAttachment(this);

	MapVisualComponent = CreateDefaultSubobject<UMapVisualComponent>(TEXT("Map Visual"));
	MapVisualComponent->SetupAttachment(Box);
	MapVisualComponent->AttachMeshes(Box);
}

void UMapLimitComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Box)
	{
		Box->OnComponentBeginOverlap.AddDynamic(this, &UMapLimitComponent::OnOverlapBegin);
	}
}

void UMapLimitComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	AMapPawn* player = Cast<AMapPawn>(OtherActor);
	if (!player)
		return;

	AActor* owner = GetOwner();
	if (!owner)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner of Map Limit not set"));
		return;
	}

	const FVector currentPlayerPos = player->GetActorLocation();
	const float distanceToMapCenter = currentPlayerPos.X - OverlappedComp->GetComponentLocation().X;

	const float xPos = owner->GetActorLocation().X + distanceToMapCenter;

	player->SetActorLocation(FVector(xPos, currentPlayerPos.Y , currentPlayerPos.Z));
	
}
void UMapLimitComponent::InitLimitComponent(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder, UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap)
{
	MapVisualComponent->InitVisualComponents(mapSelectMesh, mapBorder, gameplayMap, terrainMap);
}

UMapVisualComponent* UMapLimitComponent::GetVisualComponent()
{
	return MapVisualComponent;
}
