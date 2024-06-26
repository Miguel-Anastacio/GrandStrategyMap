// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MapPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class AClickableMap;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapUpCloseSignature, AClickableMap*, map);
/**
 * Pawn class for controlling the map camera.
 */
UCLASS(BlueprintType)
class INTERACTIVEMAP_API AMapPawn : public APawn
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    AMapPawn();

    /** Called every frame. */
    virtual void Tick(float DeltaTime) override;

    /** Moves the camera based on input. */
    UFUNCTION(BlueprintCallable, Category = "Map Pawn")
    void MoveCamera(FVector2D input);

    /** Stops camera movement. */
    void Stop();

    /** Zooms the camera based on input. */
    UFUNCTION(BlueprintCallable, Category = "Map Pawn")
    void ZoomCamera(float input);

    /** Sets the interactive map reference. */
    UFUNCTION(BlueprintCallable, Category = "Map Pawn")
    void SetInteractiveMap(AClickableMap* map);

    /** Sets vertical movement limits. */
    FORCEINLINE void SetVerticalMovementLimits(FVector2D limits) { VerticalLimitMovement = limits; };

    /** Gets vertical movement limits. */
    FORCEINLINE FVector2D GetVerticalMovementLimits() { return VerticalLimitMovement; };

public:
    UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Map")
    FMapUpCloseSignature MapZoomInThresholdDelegate;
    UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Map")
    FMapUpCloseSignature MapZoomOutThresholdDelegate;


protected:
    virtual void BeginPlay() override;

protected:
    /** Collision component for the pawn. */
    UPROPERTY(EditAnywhere, Category = "Pivot")
    TObjectPtr<USphereComponent> CollisionComponent;

    /** Spring arm component for the camera. */
    UPROPERTY(EditAnywhere, Category = "Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    /** Camera component. */
    UPROPERTY(EditAnywhere, Category = "Camera")
    TObjectPtr<UCameraComponent> Camera;

    /** Camera movement speed. */
    UPROPERTY(EditAnywhere, Category = "Camera|Movement")
    float CameraMoveSpeed = 10.0f;

    /** Camera zoom speed. */
    UPROPERTY(EditAnywhere, Category = "Camera|Zoom")
    float ZoomSpeed = 10.0f;

    /** Min (x) and max (Y) zoom distance. */
    UPROPERTY(EditAnywhere, Category = "Camera|Zoom")
    FVector2D ZoomLimit = FVector2D(500.0f, 3000.0f);

    /** Distance at which camera turns slightly. */
    UPROPERTY(EditAnywhere, Category = "Camera|Zoom")
    float ZoomCameraRot = 2000.0f;

    /** Adjust size of sphere component based on camera zoom */
    UPROPERTY(EditAnywhere, Category = "Camera|Zoom")
    float CameraCollisionScaleOnZoom = 0.1f;

    /** Max Sphere Component Scale */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
    float MaxSphereScale = 5;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
    float MinSphereScale = 0.1;
    /** Vertical movement limits. (x top, y bottom) */
    UPROPERTY(BlueprintReadWrite, Category = "Camera|Movement")
    FVector2D VerticalLimitMovement = FVector2D(-3000, 3000);

    /** Weak reference to the interactive map. */
    UPROPERTY()
    TWeakObjectPtr<AClickableMap> GameMap;

    float OldZoom = 0.0f;
};

