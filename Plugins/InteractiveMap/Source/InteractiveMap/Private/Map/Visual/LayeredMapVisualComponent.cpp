// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/Visual/LayeredMapVisualComponent.h"
#include "InteractiveMap.h"

//// Sets default values for this component's properties
ULayeredMapVisualComponent::ULayeredMapVisualComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
    MapSelectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map New Select"));
    MapSelectMesh->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
    //MapSelectMesh->RegisterComponent();
    MapSelectMesh->bHiddenInGame = true;

    MapBorderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map New Border"));
    MapBorderMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    //MapBorderMesh->RegisterComponent();

    TerrainMapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map New Terrain"));
    TerrainMapMesh->SetCollisionProfileName(TEXT("NoCollision"));
    TerrainMapMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    //TerrainMapMesh->RegisterComponent();

    GameplayMapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map New Gameplay"));
    GameplayMapMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    //GameplayMapMesh->RegisterComponent();
}

void ULayeredMapVisualComponent::AttachMeshes(USceneComponent* root)
{
    FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepRelative, true);

    MapSelectMesh->SetupAttachment(root);
    MapBorderMesh->SetupAttachment(root);
    GameplayMapMesh->SetupAttachment(root);
    TerrainMapMesh->SetupAttachment(root);
}
void ULayeredMapVisualComponent::AttachMeshesOutsideConstructor(USceneComponent* root)
{
    FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepWorld, true);
    MapSelectMesh->AttachToComponent(root, attachmentRules);
    MapBorderMesh->AttachToComponent(root, attachmentRules);
    GameplayMapMesh->AttachToComponent(root, attachmentRules);
    TerrainMapMesh->AttachToComponent(root, attachmentRules);
}


void ULayeredMapVisualComponent::InitVisualComponentFromOriginal(UMapVisualComponent* mapVisual)
{
    ULayeredMapVisualComponent* layerVisual = Cast<ULayeredMapVisualComponent>(mapVisual);
    if (!layerVisual)
    {
        UE_LOG(LogInteractiveMap, Error, TEXT("Mismatch between type of Visual Components"));
        return;
    }

    //InitMeshProperty(layerVisual->GetMapSelectMeshComponent(), MapSelectMesh);
    //InitMeshProperty(layerVisual->GetMapBorderMeshComponent(), MapBorderMesh);
    //InitMeshProperty(layerVisual->GetMapGameplayMeshComponent(), GameplayMapMesh);
    //InitMeshProperty(layerVisual->GetMapTerrainMeshComponent(), TerrainMapMesh);

    MapSelectMesh = InitMeshComponent(layerVisual->GetMapSelectMeshComponent());
    MapBorderMesh = InitMeshComponent(layerVisual->GetMapBorderMeshComponent());
    GameplayMapMesh = InitMeshComponent(layerVisual->GetMapGameplayMeshComponent());
    TerrainMapMesh = InitMeshComponent(layerVisual->GetMapTerrainMeshComponent());
}

void ULayeredMapVisualComponent::UpdateVisualComponent(UMapVisualComponent* mapVisual)
{
    ULayeredMapVisualComponent* layerVisual = Cast<ULayeredMapVisualComponent>(mapVisual);
    if (!layerVisual)
    {
        UE_LOG(LogInteractiveMap, Error, TEXT("Mismatch between type of Visual Components"));
        return;
    }

    SetMeshProperties(layerVisual->GetMapSelectMeshComponent(), MapSelectMesh);
    SetMeshProperties(layerVisual->GetMapBorderMeshComponent(), MapBorderMesh);
    SetMeshProperties(layerVisual->GetMapGameplayMeshComponent(), GameplayMapMesh);
    SetMeshProperties(layerVisual->GetMapTerrainMeshComponent(), TerrainMapMesh);
}


UStaticMeshComponent* ULayeredMapVisualComponent::GetMeshComponent(MapMode mode)
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
        return TerrainMapMesh;
        break;
    default:
        break;
    }
    return nullptr;
}

UStaticMeshComponent* ULayeredMapVisualComponent::GetMapSelectMeshComponent()
{
    return MapSelectMesh;
}

UStaticMeshComponent* ULayeredMapVisualComponent::GetMapGameplayMeshComponent()
{
    return GameplayMapMesh;
}

UStaticMeshComponent* ULayeredMapVisualComponent::GetMapBorderMeshComponent()
{
    return MapBorderMesh;
}

UStaticMeshComponent* ULayeredMapVisualComponent::GetMapTerrainMeshComponent()
{
    return TerrainMapMesh;
}

