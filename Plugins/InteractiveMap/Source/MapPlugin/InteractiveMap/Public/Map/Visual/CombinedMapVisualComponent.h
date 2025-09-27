// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Map/MapEnums.h"
#include "Map/MapVisualComponent.h"
#include "CombinedMapVisualComponent.generated.h"

class UStaticMeshComponent;
/**
 * Component for managing the visual representation of the map.
 * Map is made up of 4 meshes
 * If you wish to combine the borderMesh, GameplayMesh and TerrainMesh you have: 
 *  - create a material that blends all 3 textures
 *  - assign the material in a ClickableMap class (GameplayMapMaterial)
 *  - hide the terrain and border mesh 
 *  - override the set map mode function either in c++ or blueprint
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTERACTIVEMAP_API UCombinedMapVisualComponent : public UMapVisualComponent
{
    GENERATED_BODY()

//public:
    /** Default constructor. */
    UCombinedMapVisualComponent();

    /** Attaches meshes to the root component. */
    void AttachMeshes(USceneComponent* root) override;
    virtual void AttachMeshesOutsideConstructor(USceneComponent* root) override;

    /** Initializes the visual component from an original one. */
    void InitVisualComponentFromOriginal(UMapVisualComponent* mapVisual) override;

    virtual void UpdateVisualComponent(UMapVisualComponent* mapVisual) override;

    /** Gets the mesh component for a specific map mode. */
    UStaticMeshComponent* GetMeshComponent(MapMode mode) override;
    /** Gets the map select mesh component. */
    UStaticMeshComponent* GetMapSelectMeshComponent() override;

    /** Gets the map gameplay mesh component. */
    UStaticMeshComponent* GetMapGameplayMeshComponent() override;

protected:
    /** The mesh for selecting map elements. */
    UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> MapSelectMesh;

    /** The mesh for the gameplay map. */
    UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> GameplayMapMesh;

		
};
