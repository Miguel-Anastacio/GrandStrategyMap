// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Map/MapEnums.h"
#include "Components/SceneComponent.h"
#include "MapVisualComponent.generated.h"

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
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), NotBlueprintable)
class INTERACTIVEMAP_API UMapVisualComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    UMapVisualComponent();

    /** Attaches meshes to the root component. */
    virtual void AttachMeshes(USceneComponent* root) {};
    virtual void AttachMeshesOutsideConstructor(USceneComponent* root) {};

    /** Initializes the visual component from an original one. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    virtual void InitVisualComponentFromOriginal(UMapVisualComponent* mapVisual);
    /** Initializes the visual component from an original one. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    virtual void UpdateVisualComponent(UMapVisualComponent* mapVisual) {};

    /** Initializes the mesh components. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    virtual void InitVisualComponents(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder,
        UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap);

    /** Initializes a mesh component. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    virtual UStaticMeshComponent* InitMeshComponent(UStaticMeshComponent* original);

    /** Initializes the mesh property. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    void SetMeshProperties(UStaticMeshComponent* original, UStaticMeshComponent* meshToUpdate);

    /** Updates the mesh material. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    void UpdateMeshMaterial(UStaticMeshComponent* meshToUpdate, UMaterialInterface* mat);

    /** Gets the mesh component for a specific map mode. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    virtual UStaticMeshComponent* GetMeshComponent(MapMode mode);

    /** Gets the map select mesh component. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    virtual UStaticMeshComponent* GetMapSelectMeshComponent();

    /** Gets the map gameplay mesh component. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    virtual UStaticMeshComponent* GetMapGameplayMeshComponent();

    /** Gets the map border mesh component. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    virtual UStaticMeshComponent* GetMapBorderMeshComponent();

    /** Gets the map terrain mesh component. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    virtual UStaticMeshComponent* GetMapTerrainMeshComponent();

    /** Calculates the size of the map. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    virtual FVector CalculateSizeOfMesh(UStaticMeshComponent* mesh) const;


protected:
		
};
