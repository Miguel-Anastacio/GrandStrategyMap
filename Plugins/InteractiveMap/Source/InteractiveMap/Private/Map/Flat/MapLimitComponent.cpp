
#include "Map/Flat/MapLimitComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Map/Visual/LayeredMapVisualComponent.h"
#include "Map/MapVisualComponent.h"
#include "Map/ClickableMap.h"
#include "Game/MapPawn.h"
#include "InteractiveMap.h"

// Sets default values for this component's properties
UMapLimitComponent::UMapLimitComponent()
	: USceneComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


	Box = CreateDefaultSubobject<UBoxComponent>("Box Limit");
	//Box->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);




	//MapVisualComponent->SetupAttachment(Box);
	// 	//MapVisualComponent->AttachMeshes(Box);
}

void UMapLimitComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Box)
	{
		Box->OnComponentBeginOverlap.AddDynamic(this, &UMapLimitComponent::OnOverlapBegin);
	}
}
UE_DISABLE_OPTIMIZATION
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
		UE_LOG(LogInteractiveMap, Error, TEXT("Owner of Map Limit not set"));
		return;
	}

	const FVector currentPlayerPos = player->GetActorLocation();
	const float distanceToMapCenter = currentPlayerPos.X - OverlappedComp->GetComponentLocation().X;

	const float xPos = owner->GetActorLocation().X + distanceToMapCenter;

	player->SetActorLocation(FVector(xPos, currentPlayerPos.Y, currentPlayerPos.Z));

}
UE_ENABLE_OPTIMIZATION
void UMapLimitComponent::InitLimitComponent(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder, UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap)
{
	MapVisualComponent->InitVisualComponents(mapSelectMesh, mapBorder, gameplayMap, terrainMap);
}

UMapVisualComponent* UMapLimitComponent::GetVisualComponent()
{
	return MapVisualComponent;
}

void UMapLimitComponent::Attach(USceneComponent* root)
{
	Box->SetupAttachment(root);
	//MapVisualComponent->SetupAttachment(Box);
	//MapVisualComponent->AttachMeshes(Box);
}

void UMapLimitComponent::CreateVisualComponent(TSubclassOf<UMapVisualComponent> mapVisualClass)
{
	if (mapVisualClass)
	{
		MapVisualComponent = NewObject<UMapVisualComponent>(this, mapVisualClass, TEXT("Limit Visual"));
		//MapVisualComponent = NewObject<ULayeredMapVisualComponent>(this);
		MapVisualComponent->SetupAttachment(Box);
		MapVisualComponent->RegisterComponent();
		MapVisualComponent->AttachMeshesOutsideConstructor(Box);
		//MapVisualComponent->Attacg(Box);
	}
}

