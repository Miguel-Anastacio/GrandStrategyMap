// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/BirdEyeController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
//#include "TP_TopDownCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Game/MapPawn.h"
#include "Map/InteractiveMap.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);
ABirdEyeController::ABirdEyeController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ABirdEyeController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ABirdEyeController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
	/*	EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ATP_TopDownPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ATP_TopDownPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ATP_TopDownPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ATP_TopDownPlayerController::OnSetDestinationReleased);*/

		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &ABirdEyeController::MouseClick);
		EnhancedInputComponent->BindAction(CameraMoveAction, ETriggerEvent::Started, this, &ABirdEyeController::CameraMovement);
		EnhancedInputComponent->BindAction(CameraMoveAction, ETriggerEvent::Triggered, this, &ABirdEyeController::CameraMovement);
		EnhancedInputComponent->BindAction(MouseMoveAction, ETriggerEvent::Triggered, this, &ABirdEyeController::CameraMovement);

		//// Setup touch input events
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ATP_TopDownPlayerController::OnInputStarted);
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ATP_TopDownPlayerController::OnTouchTriggered);
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ATP_TopDownPlayerController::OnTouchReleased);
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ATP_TopDownPlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

//void ATP_TopDownPlayerController::OnInputStarted()
//{
//	StopMovement();
//}
//
//// Triggered every frame when the input is held down
//void ATP_TopDownPlayerController::OnSetDestinationTriggered()
//{
//	// We flag that the input is being pressed
//	FollowTime += GetWorld()->GetDeltaSeconds();
//	
//	// We look for the location in the world where the player has pressed the input
//	FHitResult Hit;
//	bool bHitSuccessful = false;
//	if (bIsTouch)
//	{
//		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
//	}
//	else
//	{
//		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
//	}
//
//	// If we hit a surface, cache the location
//	if (bHitSuccessful)
//	{
//		CachedDestination = Hit.Location;
//	}
//	
//	// Move towards mouse pointer or touch
//	APawn* ControlledPawn = GetPawn();
//	if (ControlledPawn != nullptr)
//	{
//		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
//		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
//	}
//}
//
//void ATP_TopDownPlayerController::OnSetDestinationReleased()
//{
//	// If it was a short press
//	if (FollowTime <= ShortPressThreshold)
//	{
//		// We move there and spawn some particles
//		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
//		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
//	}
//
//	FollowTime = 0.f;
//}
//
//// Triggered every frame when the input is held down
//void ATP_TopDownPlayerController::OnTouchTriggered()
//{
//	bIsTouch = true;
//	OnSetDestinationTriggered();
//}
//
//void ATP_TopDownPlayerController::OnTouchReleased()
//{
//	bIsTouch = false;
//	OnSetDestinationReleased();
//}
UE_DISABLE_OPTIMIZATION
void ABirdEyeController::MouseClick()
{
	FHitResult hit;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, hit))
	{
		FVector start = hit.ImpactPoint;
		FVector end = hit.ImpactPoint;
		start.Z = hit.ImpactPoint.Z + zOffset;
		end.Z = hit.ImpactPoint.Z - zOffset;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		params.bTraceComplex = true;
		params.bReturnFaceIndex = true;
		//params.
		if (!GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params))
			return;
		if (!IsValid(hit.GetActor()))
			return;

		AInteractiveMap* map = Cast<AInteractiveMap>(hit.GetActor());
		if (map)
		{
			FVector2D uvs = FVector2D(0, 0);
			UGameplayStatics::FindCollisionUV(hit, 0, uvs);
			if (map->GetMapRenderTarget())
			{
				FLinearColor color = UKismetRenderingLibrary::ReadRenderTargetRawUV(GetWorld(), map->GetMapRenderTarget(), uvs.X, uvs.Y);

				FName id = map->GetProvinceID(FVector(color.R, color.G, color.B));
				FProvinceData* data = map->GetProvinceData(id);
				if (data)
				{
					//GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, data->ProvinceName);
					data->PrintProvinceData();
					//UE_LOG(LogTemp, Warning, TEXT("%s"), map->GetLookUpTable[]);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, FString("Error"));
				}
			}
		}

	}
}

UE_ENABLE_OPTIMIZATION
void ABirdEyeController::CameraMovement(const FInputActionInstance& instance)
{
	FVector2D input = instance.GetValue().Get<FVector2D>();
	//FVector2D input = FVector2D(rawInput.Y, rawInput.X);
	input.Y *= -1;
	AMapPawn* pawn = GetPawn<AMapPawn>();
	pawn->MoveCamera(input);

}

void ABirdEyeController::MouseMovement()
{
	
}
