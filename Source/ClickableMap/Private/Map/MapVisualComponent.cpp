// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/MapVisualComponent.h"
UE_DISABLE_OPTIMIZATION

// Sets default values for this component's properties
UMapVisualComponent::UMapVisualComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UMapVisualComponent::InitVisualComponentFromOriginal(UMapVisualComponent* mapVisual)
{
    MapSelectMesh = InitMeshComponent(mapVisual->GetMapSelectMeshComponent());
    MapBorderMesh = InitMeshComponent(mapVisual->GetMapBorderMeshComponent());
    GameplayMapMesh = InitMeshComponent(mapVisual->GetMapGameplayMeshComponent());
    TerrainMapMesh = InitMeshComponent(mapVisual->GetMapTerrainMeshComponent());
}

void UMapVisualComponent::InitVisualComponents(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder, UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap)
{
    MapSelectMesh = InitMeshComponent(mapSelectMesh);
    MapBorderMesh = InitMeshComponent(mapBorder);
    GameplayMapMesh = InitMeshComponent(gameplayMap);
    TerrainMapMesh =InitMeshComponent(terrainMap);
}

UStaticMeshComponent* UMapVisualComponent::InitMeshComponent(UStaticMeshComponent* original)
{
    if (original)
    {
        // Create a new instance of the static mesh component
        UStaticMeshComponent* instance = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());

        if (instance)
        {
            // Set properties of the new static mesh component to match the original
            instance->SetStaticMesh(original->GetStaticMesh());
            instance->SetRelativeLocation(original->GetRelativeLocation());
            instance->SetRelativeRotation(original->GetRelativeRotation());
            instance->SetRelativeScale3D(original->GetRelativeScale3D());
            // Attach the new static mesh component to the actor
            instance->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

            // Optionally, adjust other properties such as collision settings or material overrides
            instance->SetMaterial(0, original->GetMaterial(0));
            // Register the new static mesh component with the world
            instance->RegisterComponent();
        }
        
        return instance;
    }

    UE_LOG(LogTemp, Error, TEXT("Original Mesh is not valid"))
    return nullptr;
}
void UMapVisualComponent::UpdateMeshMaterial(UStaticMeshComponent* meshToUpdate, UMaterialInterface* mat)
{
    if (!meshToUpdate)
        return;
    meshToUpdate->SetMaterial(0, mat);
}

UStaticMeshComponent* UMapVisualComponent::GetMeshComponent(MapMode mode)
{
    switch (mode)
    {
    case MapMode::POLITICAL:
        return GameplayMapMesh;
        break;
    case MapMode::RELIGIOUS:
        return GameplayMapMesh;
        break;
    case MapMode::CULTURAL:
        return GameplayMapMesh;
        break;
    case MapMode::TERRAIN:
        return nullptr;
        break;
    default:
        break;
    }
    return nullptr;
}

UStaticMeshComponent* UMapVisualComponent::GetMapSelectMeshComponent()
{
    return MapSelectMesh;
}

UStaticMeshComponent* UMapVisualComponent::GetMapGameplayMeshComponent()
{
    return GameplayMapMesh;
}

UStaticMeshComponent* UMapVisualComponent::GetMapBorderMeshComponent()
{
    return MapBorderMesh;
}

UStaticMeshComponent* UMapVisualComponent::GetMapTerrainMeshComponent()
{
    return TerrainMapMesh;
}

// Called when the game starts
void UMapVisualComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMapVisualComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UE_ENABLE_OPTIMIZATION
