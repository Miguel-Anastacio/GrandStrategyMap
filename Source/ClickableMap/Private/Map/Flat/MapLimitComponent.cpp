// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Flat/MapLimitComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Map/MapVisualComponent.h"
// Sets default values for this component's properties
UMapLimitComponent::UMapLimitComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	MapVisualComponent = CreateDefaultSubobject<UMapVisualComponent>(TEXT("Map Visual"));
	MapVisualComponent->SetupAttachment(this);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Limit Capsule"));
	Capsule->SetupAttachment(MapVisualComponent);

}

void UMapLimitComponent::UpdateGameplayMapMesh(UMaterialInstanceDynamic* mat)
{
	//GameplayMapMesh->set
}


// Called when the game starts
void UMapLimitComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Capsule)
	{
		Capsule->OnComponentBeginOverlap.AddDynamic(this, &UMapLimitComponent::OnOverlapBegin);
	}
}
UE_DISABLE_OPTIMIZATION
void UMapLimitComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	if (TimeSinceOverlap < 1.0f)
		return;

	APawn* player = Cast<APawn>(OtherActor);
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

	player->SetActorLocation(owner->GetActorLocation(), true);
	
	UE_LOG(LogTemp, Error, TEXT("Overlap Begin %s"), *GetName());
	TimeSinceOverlap = 0.0f;

}

void UMapLimitComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TimeSinceOverlap += DeltaTime;
}

void UMapLimitComponent::InitLimitComponent(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder, UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap)
{
	MapVisualComponent->InitVisualComponents(mapSelectMesh, mapBorder, gameplayMap, terrainMap);
}

UMapVisualComponent* UMapLimitComponent::GetVisualComponent()
{
	return MapVisualComponent;
}
UE_ENABLE_OPTIMIZATION
