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
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTERACTIVEMAP_API UMapVisualComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    UMapVisualComponent();

    /** Attaches meshes to the root component. */
    void AttachMeshes(USceneComponent* root);

    /** Initializes the visual component from an original one. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    void InitVisualComponentFromOriginal(UMapVisualComponent* mapVisual);

    /** Initializes the mesh components. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    void InitVisualComponents(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder,
        UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap);

    /** Initializes a mesh component. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    UStaticMeshComponent* InitMeshComponent(UStaticMeshComponent* original);

    /** Initializes the mesh property. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    void InitMeshProperty(UStaticMeshComponent* original, UStaticMeshComponent* meshToUpdate);

    /** Updates the mesh material. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    void UpdateMeshMaterial(UStaticMeshComponent* meshToUpdate, UMaterialInterface* mat);

    /** Gets the mesh component for a specific map mode. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    UStaticMeshComponent* GetMeshComponent(MapMode mode);

    /** Gets the map select mesh component. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    UStaticMeshComponent* GetMapSelectMeshComponent();

    /** Gets the map gameplay mesh component. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    UStaticMeshComponent* GetMapGameplayMeshComponent();

    /** Gets the map border mesh component. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    UStaticMeshComponent* GetMapBorderMeshComponent();

    /** Gets the map terrain mesh component. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    UStaticMeshComponent* GetMapTerrainMeshComponent();

    /** Calculates the size of the map. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    FVector CalculateSizeOfMap() const;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    virtual void BeginPlay() override;

    /** The mesh for selecting map elements. */
    UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> MapSelectMesh;

    /** The mesh for the map border. */
    UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> MapBorderMesh;

    /** The mesh for the gameplay map. */
    UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> GameplayMapMesh;

    /** The mesh for the terrain map. */
    UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> TerrainMapMesh;
		
};
