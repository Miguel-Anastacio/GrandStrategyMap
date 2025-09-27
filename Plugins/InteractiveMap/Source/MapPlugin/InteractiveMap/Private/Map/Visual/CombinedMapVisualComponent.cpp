// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/Visual/CombinedMapVisualComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InteractiveMap.h"
#include "Engine/CollisionProfile.h"

// Sets default values for this component's properties
UCombinedMapVisualComponent::UCombinedMapVisualComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
    MapSelectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map New Select"));
    MapSelectMesh->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
    MapSelectMesh->bHiddenInGame = true;

    GameplayMapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map New Gameplay"));
    GameplayMapMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

void UCombinedMapVisualComponent::AttachMeshes(USceneComponent* root)
{
    MapSelectMesh->SetupAttachment(root);
    GameplayMapMesh->SetupAttachment(root);
}
void UCombinedMapVisualComponent::AttachMeshesOutsideConstructor(USceneComponent* root)
{
    FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepRelative, true);
    MapSelectMesh->AttachToComponent(root, attachmentRules);
    GameplayMapMesh->AttachToComponent(root, attachmentRules);

}
void UCombinedMapVisualComponent::InitVisualComponentFromOriginal(UMapVisualComponent* mapVisual)
{
    UCombinedMapVisualComponent* combinedVisual = Cast<UCombinedMapVisualComponent>(mapVisual);
    if (!combinedVisual)
    {
        UE_LOG(LogInteractiveMap, Error, TEXT("Mismatch between type of Visual Components"));
        return;
    }

    MapSelectMesh = InitMeshComponent(combinedVisual->GetMapSelectMeshComponent());
    GameplayMapMesh = InitMeshComponent(combinedVisual->GetMapGameplayMeshComponent());
}

void UCombinedMapVisualComponent::UpdateVisualComponent(UMapVisualComponent* mapVisual)
{

    UCombinedMapVisualComponent* combinedVisual = Cast<UCombinedMapVisualComponent>(mapVisual);
    if (!combinedVisual)
    {
        UE_LOG(LogInteractiveMap, Error, TEXT("Mismatch between type of Visual Components"));
        return;
    }

    SetMeshProperties(mapVisual->GetMapSelectMeshComponent(), MapSelectMesh);
    SetMeshProperties(mapVisual->GetMapGameplayMeshComponent(), GameplayMapMesh);
}


UStaticMeshComponent* UCombinedMapVisualComponent::GetMeshComponent(MapMode mode)
{
    return GameplayMapMesh;
}

UStaticMeshComponent* UCombinedMapVisualComponent::GetMapSelectMeshComponent()
{
    return MapSelectMesh;
}
//
UStaticMeshComponent* UCombinedMapVisualComponent::GetMapGameplayMeshComponent()
{
    return GameplayMapMesh;
}


