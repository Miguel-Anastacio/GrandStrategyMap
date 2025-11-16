// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Map/MapVisualComponent.h"
#include "CombinedMapVisualComponent.generated.h"

class UStaticMeshComponent;
/**
 * Component for managing the visual representation of the map.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTERACTIVEMAP_API UCombinedMapVisualComponent : public UMapVisualComponent
{
    GENERATED_BODY()

//public:
    /** Default constructor. */
    UCombinedMapVisualComponent();

    /** Attaches meshes to the root component. */
    virtual void AttachMeshes(USceneComponent* root) override;
    virtual void AttachMeshesOutsideConstructor(USceneComponent* root) override;

    /** Initializes the visual component from an original one. */
    virtual void InitVisualComponentFromOriginal(UMapVisualComponent* mapVisual) override;

    virtual void UpdateVisualComponent(UMapVisualComponent* mapVisual) override;

    /** Gets the map gameplay mesh component. */
    UStaticMeshComponent* GetMapGameplayMeshComponent() override;

protected:
    /** The mesh for the gameplay map. */
    UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> GameplayMapMesh;

		
};
