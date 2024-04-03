// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MapPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
//#include "GameFramework/PawnMovementComponent.h"
//#include "GameFramework/FloatingPawnMovement.h"
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


	//MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("Movement Component"));
	//MovementComponent->UpdatedComponent = CollisionComponent;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

}

// Called when the game starts or when spawned
void AMapPawn::BeginPlay()
{
	Super::BeginPlay();
	// adjust the size of the collision component depending on the zoom distance
	FVector scale = FVector(CameraBoom->TargetArmLength, CameraBoom->TargetArmLength, CameraBoom->TargetArmLength) * CameraCollisionScaleOnZoom;
	CollisionComponent->SetWorldScale3D(scale);
}
UE_DISABLE_OPTIMIZATION
// Called every frame
void AMapPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector currentPos = GetActorLocation();
	FVector futurePos = GetActorLocation() + GetVelocity() * DeltaTime;
	FVector currentVelocity = GetVelocity();
	if (abs(futurePos.Y) > abs(VerticalLimitMovement.Y))
	{
		//SetActorLocation(currentPos.X, )
		CollisionComponent->SetPhysicsLinearVelocity(FVector(currentVelocity.X, 0, currentVelocity.Z));
	}
}

void AMapPawn::MoveCamera(FVector2D input)
{
	//AddMovementInput(FVector(input.X, input.Y, 0), CameraMoveSpeed);
	
	FVector vel = FVector(input.X, input.Y, 0) * CameraMoveSpeed;
	float yPos = GetActorLocation().Y + vel.Y;
	//if (abs(yPos) > abs(VerticalLimitMovement.Y))
	//	vel.Y = 0;
	
	CollisionComponent->SetPhysicsLinearVelocity(vel);

}
UE_ENABLE_OPTIMIZATION

void AMapPawn::Stop()
{
	CollisionComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

void AMapPawn::ZoomCamera(float input)
{
	float newZoom = CameraBoom->TargetArmLength + input * ZoomSpeed;
	if (newZoom > ZoomLimit.X && newZoom < ZoomLimit.Y)
	{
		CameraBoom->TargetArmLength = newZoom;
		// adjust the size of the collision component depending on the zoom distance
		FVector scale = FVector(CameraBoom->TargetArmLength, CameraBoom->TargetArmLength, CameraBoom->TargetArmLength) * CameraCollisionScaleOnZoom;
		CollisionComponent->SetWorldScale3D(scale);
	}





	if(CameraBoom->TargetArmLength <= ZoomCameraRot)
	{
		//CameraBoom->SetRelativeRotation(FRotator(-70, 0, 0));
		// hide borders
		if (GameMap.Get())
		{
			//GameMap->SetBorderVisibility(false);
		}
	}
	else
	{
		// Show borders
		if (GameMap.Get())
		{
			//GameMap->SetBorderVisibility(true);
		}

		//CameraBoom->SetRelativeRotation(FRotator(-90, 0, 0));
	}
}

void AMapPawn::SetInteractiveMap(AInteractiveMap* map)
{
	GameMap = map;
}
