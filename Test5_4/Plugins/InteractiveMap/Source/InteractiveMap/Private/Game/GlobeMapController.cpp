// Copyright 2024 An@stacioDev All rights reserved.

#include "Game/GlobeMapController.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Map/ClickableMap.h"
#include "InteractiveMap.h"
#include "Engine/LocalPlayer.h"
#include "Engine/HitResult.h"

void AGlobeMapController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(GlobeMappingContext, 0);
	}
}
void AGlobeMapController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(RotateMapAction, ETriggerEvent::Triggered, this, &AGlobeMapController::RotateMap);
		EnhancedInputComponent->BindAction(StartRotateAction, ETriggerEvent::Triggered, this, &AGlobeMapController::StartRotate);
	}
}

void AGlobeMapController::StartRotate()
{
	if (Map)
		return;

	FHitResult hit;
	if (!GetHitResultUnderCursor(ECC_Visibility, true, hit))
		return;

	if (!hit.GetActor())
		return;

	Map = Cast<AClickableMap>(hit.GetActor());

}
UE_DISABLE_OPTIMIZATION
void AGlobeMapController::RotateMap(const FInputActionInstance& instance)
{
	if (!Map)
	{
		UE_LOG(LogInteractiveMap, Warning, TEXT("Rotate not possible - make sure you click on the globe to start rotation"));
		return;
	}
	FVector2D input = instance.GetValue().Get<FVector2D>();
	input.Y *= -1;

	float max = FMath::Max(abs(input.X), abs(input.Y));
	if (max < 0.1)
		return;

	input *= MapRotSpeed;
	Map->AddActorWorldRotation(FRotator(-input.X, 0, input.Y));

}
UE_ENABLE_OPTIMIZATION
