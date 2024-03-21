// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MapPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Map/InteractiveMap.h"
// Sets default values
AMapPawn::AMapPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Pivot"));
	CollisionComponent->InitSphereRadius(35.0f);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->SetShouldUpdatePhysicsVolume(true);
	CollisionComponent->SetCanEverAffectNavigation(false);
	CollisionComponent->bDynamicObstacle = true;

	RootComponent = CollisionComponent;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(CollisionComponent);


	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("Movement Component"));
	MovementComponent->UpdatedComponent = CollisionComponent;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

}

// Called when the game starts or when spawned
void AMapPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMapPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapPawn::MoveCamera(FVector2D input)
{
	AddMovementInput(FVector(input.X, input.Y, 0), CameraMoveSpeed);
}

void AMapPawn::ZoomCamera(float input)
{
	float newZoom = CameraBoom->TargetArmLength + input * ZoomSpeed;
	if (newZoom > ZoomLimit.X && newZoom < ZoomLimit.Y)
	{
		CameraBoom->TargetArmLength = newZoom;
	}
	if(CameraBoom->TargetArmLength <= ZoomCameraRot)
	{
		CameraBoom->SetRelativeRotation(FRotator(-70, 0, 0));
		// hide borders
		if (GameMap.Get())
		{
			GameMap->SetBorderVisibility(false);
		}
	}
	else
	{
		// Show borders
		if (GameMap.Get())
		{
			GameMap->SetBorderVisibility(true);
		}

		CameraBoom->SetRelativeRotation(FRotator(-90, 0, 0));
	}
}

void AMapPawn::SetInteractiveMap(AInteractiveMap* map)
{
	GameMap = map;
}

//// Called to bind functionality to input
//void AMapPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

