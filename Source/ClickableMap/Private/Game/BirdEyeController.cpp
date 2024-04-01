// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/BirdEyeController.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Game/MapPawn.h"
#include "Map/InteractiveMap.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ManagerHUD.h"
#include "Components/SlateWrapperTypes.h"
#include "GameFramework/GameUserSettings.h"
DEFINE_LOG_CATEGORY(LogTemplateCharacter);
ABirdEyeController::ABirdEyeController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
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

	MapPawn = GetPawn<AMapPawn>();
}

void ABirdEyeController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
			FColor color = map->GetColorFromLookUpTexture(uvs);
			FName id = map->GetProvinceID(FVector(color.R, color.G, color.B));
			if (id != FName() && !bProvinceSelected)
			{
				map->UpdateProvinceHovered(color);
			}
		}
	}

	if (bMoveCamera)
	{
		MapPawn->MoveCamera(MoveInput);
	}
	else
	{
		MapPawn->Stop();
	}
}

void ABirdEyeController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		//// Setup touch input events
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &ABirdEyeController::MouseClick);
		EnhancedInputComponent->BindAction(CameraMoveAction, ETriggerEvent::Started, this, &ABirdEyeController::CameraMovement);
		//EnhancedInputComponent->BindAction(CameraMoveAction, ETriggerEvent::Triggered, this, &ABirdEyeController::CameraMovement);
		//EnhancedInputComponent->BindAction(MouseMoveAction, ETriggerEvent::Triggered, this, &ABirdEyeController::CameraMovement);

		EnhancedInputComponent->BindAction(MouseScrollAction, ETriggerEvent::Started, this, &ABirdEyeController::CameraZoom);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

UE_DISABLE_OPTIMIZATION
void ABirdEyeController::MouseClick()
{
	FHitResult hit;
	bProvinceSelected = false;
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
			//Map = map;

			FVector2D uvs = FVector2D(0, 0);
			UGameplayStatics::FindCollisionUV(hit, 0, uvs);
			FColor color = map->GetColorFromLookUpTexture(uvs);
				
			FName id = map->GetProvinceID(FVector(color.R, color.G, color.B));
			
			FProvinceData* data = map->GetProvinceData(id);
			if (data)
			{
				map->UpdateProvinceHovered(color);

				AManagerHUD* hud = Cast<AManagerHUD>(GetHUD());
				if (hud)
				{
					hud->DisplayProvinceEditorWidget(*data, id);
					hud->SetInteractiveMapReference(map);
					bProvinceSelected = true;
				}
			}
			else
			{
				AManagerHUD* hud = Cast<AManagerHUD>(GetHUD());
				if (hud)
				{
					hud->SetProvinceEditorVisibility(ESlateVisibility::Collapsed);
				}
				GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, FString("Error"));
			}
		}
	}
	if (!bProvinceSelected)
	{
		AManagerHUD* hud = Cast<AManagerHUD>(GetHUD());
		if (hud)
		{
			hud->SetProvinceEditorVisibility(ESlateVisibility::Collapsed);
		}
	}

}

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

void ABirdEyeController::CameraZoom(const FInputActionInstance& instance)
{
	float input = instance.GetValue().Get<float>();
	input *= -1;
	AMapPawn* pawn = GetPawn<AMapPawn>();
	pawn->ZoomCamera(input);
}

void ABirdEyeController::StartMovement(const FVector2D& mousePos)
{
	bMoveCamera = true;
	FVector2D ViewportSize = FVector2D(GEngine->GameUserSettings->GetLastConfirmedScreenResolution());
	ViewportCenter = ViewportSize * 0.5f;

	MoveInput = mousePos - ViewportCenter; 
	MoveInput.Normalize();

	
}

UE_ENABLE_OPTIMIZATION