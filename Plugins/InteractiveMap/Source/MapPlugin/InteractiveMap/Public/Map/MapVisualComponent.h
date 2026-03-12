// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Components/SceneComponent.h"
#include "MapVisualComponent.generated.h"

class UStaticMeshComponent;
/**
 * Component for managing the visual representation of the map.
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
    virtual void InitVisualComponents(UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap);

    /** Initializes a mesh component. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    virtual UStaticMeshComponent* InitMeshComponent(UStaticMeshComponent* original);

    /** Initializes the mesh property. */
    UFUNCTION(BlueprintCallable, Category = "MapVisual")
    void SetMeshProperties(UStaticMeshComponent* original, UStaticMeshComponent* meshToUpdate);

    /** Gets the map gameplay mesh component. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    virtual UStaticMeshComponent* GetMapGameplayMeshComponent();

    /** Calculates the size of the map. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
    virtual FVector CalculateSizeOfMesh(UStaticMeshComponent* mesh) const;


protected:
		
};
