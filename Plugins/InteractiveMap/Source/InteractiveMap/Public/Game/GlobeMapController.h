// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Game/BirdEyeController.h"
#include "GlobeMapController.generated.h"

/**
 * Controller class used with globe based maps - expands on the original map controller by adding functionality to rotate the globe
 */
struct FInputActionInstance;
UCLASS()
class INTERACTIVEMAP_API AGlobeMapController : public ABirdEyeController
{
    GENERATED_BODY()

protected:
    /** Called when the game starts. */
    void BeginPlay() override;

    /** Sets up the input component. */
    virtual void SetupInputComponent() override;

    /** Starts rotating the map. */
    UFUNCTION(Category = "Globe Controller")
    void StartRotate();

    /** Rotates the map. */
    UFUNCTION(Category = "Globe Controller")
    void RotateMap(const FInputActionInstance& instance);

protected:
    /** Mapping context for the globe. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* GlobeMappingContext;

    /** Action to start rotating the map. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* StartRotateAction;

    /** Action to rotate the map. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* RotateMapAction;

    /** Speed of map rotation. */
    UPROPERTY(EditAnywhere, Category = "MapRotation")
    float MapRotSpeed = 20.0f;
};
