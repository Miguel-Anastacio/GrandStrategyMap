// Copyright 2024 An@stacioDev All rights reserved.

#include "Game/MapPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Map/ClickableMap.h"
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
	OldZoom = CameraBoom->TargetArmLength;
}

void AMapPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector currentPos = GetActorLocation();
	FVector futurePos = GetActorLocation() + GetVelocity() * DeltaTime;
	FVector currentVelocity = GetVelocity();
	if (abs(futurePos.Y) > abs(VerticalLimitMovement.Y))
	{
		CollisionComponent->SetPhysicsLinearVelocity(FVector(currentVelocity.X, 0, currentVelocity.Z));
	}
}

void AMapPawn::MoveCamera(FVector2D input)
{
	FVector vel = FVector(input.X, input.Y, 0) * CameraMoveSpeed;
	CollisionComponent->SetPhysicsLinearVelocity(vel);
}

void AMapPawn::Stop()
{
	CollisionComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

void AMapPawn::ZoomCamera(float input)
{
	float newZoom = CameraBoom->TargetArmLength + input * ZoomSpeed;

	FHitResult hit;
	FVector start = Camera->GetComponentLocation();
	FVector end = Camera->GetComponentLocation() + Camera->GetForwardVector() * ZoomLimit.Y * 2;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	if (!GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params))
		return;

	FVector cameraToMap = hit.Location - (start + Camera->GetForwardVector() * input * ZoomSpeed);
	float distance = cameraToMap.Size();

	if (distance < ZoomLimit.X && input < 0)
		return;
	if (distance > ZoomLimit.Y && input > 0)
		return;

	CameraBoom->TargetArmLength = newZoom;
	// adjust the size of the collision component depending on the zoom distance
	FVector scale = FVector(CameraBoom->TargetArmLength, CameraBoom->TargetArmLength, CameraBoom->TargetArmLength) * CameraCollisionScaleOnZoom;

	scale.X = FMath::Clamp(scale.X, MinSphereScale, MaxSphereScale);
	scale.Y = FMath::Clamp(scale.Y, MinSphereScale, MaxSphereScale);
	scale.Z = FMath::Clamp(scale.Z, MinSphereScale, MaxSphereScale); 

	CollisionComponent->SetWorldScale3D(scale);

	if(distance <= ZoomCameraRot)
	{
		if(OldZoom > ZoomCameraRot)
			MapZoomInThresholdDelegate.Broadcast(GameMap.Get());
	}
	else
	{
		if(OldZoom < ZoomCameraRot)
			MapZoomOutThresholdDelegate.Broadcast(GameMap.Get());
	}

	OldZoom = distance;
}

void AMapPawn::SetInteractiveMap(AClickableMap* map)
{
	GameMap = map;
}
