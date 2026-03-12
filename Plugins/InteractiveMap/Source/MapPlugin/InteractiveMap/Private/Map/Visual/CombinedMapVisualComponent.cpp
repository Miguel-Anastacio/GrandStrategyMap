// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/Visual/CombinedMapVisualComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InteractiveMap.h"
#include "Engine/CollisionProfile.h"

// Sets default values for this component's properties
UCombinedMapVisualComponent::UCombinedMapVisualComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
    GameplayMapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map New Gameplay"));
    GameplayMapMesh->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
}

void UCombinedMapVisualComponent::AttachMeshes(USceneComponent* root)
{
    GameplayMapMesh->SetupAttachment(root);
}
void UCombinedMapVisualComponent::AttachMeshesOutsideConstructor(USceneComponent* root)
{
    FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepRelative, true);
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

    SetMeshProperties(mapVisual->GetMapGameplayMeshComponent(), GameplayMapMesh);
}

UStaticMeshComponent* UCombinedMapVisualComponent::GetMapGameplayMeshComponent()
{
    return GameplayMapMesh;
}


