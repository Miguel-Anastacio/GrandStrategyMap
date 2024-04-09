// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/MapVisualComponent.h"
#include "InteractiveMap.h"

// Sets default values for this component's properties
UMapVisualComponent::UMapVisualComponent()
{

}
UE_DISABLE_OPTIMIZATION
void UMapVisualComponent::InitVisualComponentFromOriginal(UMapVisualComponent* mapVisual)
{
    int a = 0;
}

void UMapVisualComponent::InitVisualComponents(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder, UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap)
{

}

UE_ENABLE_OPTIMIZATION
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

    UE_LOG(LogInteractiveMap, Error, TEXT("Original Mesh is not valid"));
    return nullptr;
}

void UMapVisualComponent::InitMeshProperty(UStaticMeshComponent* original, UStaticMeshComponent* meshToUpdate)
{
    if (!original)
    {
        UE_LOG(LogInteractiveMap, Error, TEXT("Original Mesh is not valid"));
        return;
    }
    if (!meshToUpdate)
    {
        UE_LOG(LogInteractiveMap, Error, TEXT("Mesh to update is not valid"));
        return;
    }

    meshToUpdate->SetVisibility(original->IsVisible());
    meshToUpdate->SetStaticMesh(original->GetStaticMesh());
    meshToUpdate->SetRelativeLocation(original->GetRelativeLocation());
    meshToUpdate->SetRelativeRotation(original->GetRelativeRotation());
    meshToUpdate->SetRelativeScale3D(original->GetRelativeScale3D());
    meshToUpdate->SetMaterial(0, original->GetMaterial(0));
}
void UMapVisualComponent::UpdateMeshMaterial(UStaticMeshComponent* meshToUpdate, UMaterialInterface* mat)
{
    if (!meshToUpdate)
        return;

    meshToUpdate->SetMaterial(0, mat);
}

UStaticMeshComponent* UMapVisualComponent::GetMeshComponent(MapMode mode)
{
    return nullptr;
}

UStaticMeshComponent* UMapVisualComponent::GetMapSelectMeshComponent()
{
    return nullptr;
}

UStaticMeshComponent* UMapVisualComponent::GetMapGameplayMeshComponent()
{
    return nullptr;
}

UStaticMeshComponent* UMapVisualComponent::GetMapBorderMeshComponent()
{
    return nullptr;
}

UStaticMeshComponent* UMapVisualComponent::GetMapTerrainMeshComponent()
{
    return nullptr;
}

FVector UMapVisualComponent::CalculateSizeOfMesh(UStaticMeshComponent* mesh) const
{
    FBox boundingBox = mesh->Bounds.GetBox();
    return boundingBox.Max - boundingBox.Min;
    
}
