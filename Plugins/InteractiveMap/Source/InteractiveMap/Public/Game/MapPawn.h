// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MapPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class AInteractiveMap;
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
    void SetInteractiveMap(AInteractiveMap* map);

    /** Sets vertical movement limits. */
    FORCEINLINE void SetVerticalMovementLimits(FVector2D limits) { VerticalLimitMovement = limits; };

    /** Gets vertical movement limits. */
    FORCEINLINE FVector2D GetVerticalMovementLimits() { return VerticalLimitMovement; };

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

    /** Camera collision scale on zoom. */
    UPROPERTY(EditAnywhere, Category = "Camera|Zoom")
    float CameraCollisionScaleOnZoom = 2.0f;

    /** Vertical movement limits. (x top, y bottom) */
    UPROPERTY(BlueprintReadWrite, Category = "Camera|Movement")
    FVector2D VerticalLimitMovement = FVector2D(-3000, 3000);

    /** Weak reference to the interactive map. */
    UPROPERTY()
    TWeakObjectPtr<AInteractiveMap> GameMap;
};

