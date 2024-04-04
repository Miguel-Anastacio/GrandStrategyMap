// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MapPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class AInteractiveMap;
UCLASS(BlueprintType)
class INTERACTIVEMAP_API AMapPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMapPawn();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "Map Pawm")
	void MoveCamera(FVector2D input);

	void Stop();

	UFUNCTION(BlueprintCallable, Category = "Map Pawm")
	void ZoomCamera(float input);

	UFUNCTION(BlueprintCallable, Category = "Map Pawm")
	void SetInteractiveMap(AInteractiveMap* map);

	FORCEINLINE void SetVerticalMovementLimits(FVector2D limits) { VerticalLimitMovement = limits; };
	FORCEINLINE FVector2D GetVerticalMovementLimits() { return VerticalLimitMovement; };
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Pivot")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	///** DefaultPawn movement component */
	//UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UPawnMovementComponent> MovementComponent;

	UPROPERTY(EditAnywhere, Category = "Camera| Movement");
	float CameraMoveSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Camera| Zoom");
	float ZoomSpeed = 10.0f;

	// x is min distance to map Y is max distance 
	UPROPERTY(EditAnywhere, Category = "Camera| Zoom");
	FVector2D ZoomLimit = FVector2D(500.0f,3000.0f);

	// distance at which camera turns slightly
	UPROPERTY(EditAnywhere, Category = "Camera| Zoom");
	float ZoomCameraRot = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Camera| Zoom");
	float CameraCollisionScaleOnZoom = 2.0f;

	// x is top of map and y is the bottom
	UPROPERTY(BlueprintReadWrite, Category = "Camera| Movement");
	FVector2D VerticalLimitMovement = FVector2D(-3000, 3000);

	UPROPERTY()
	TWeakObjectPtr<AInteractiveMap> GameMap;

};
