// Copyright 2024 An@stacioDev All rights reserved.

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
#include "Engine/Engine.h"

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
		if (!GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params))
			return;
		if (!IsValid(hit.GetActor()))
			return;

		if(!Map)
			Map = Cast<AInteractiveMap>(hit.GetActor());

		if (Map)
		{
			FVector2D uvs = FVector2D(0, 0);
			UGameplayStatics::FindCollisionUV(hit, 0, uvs);
			FColor color = Map->GetColorFromLookUpTexture(uvs);

			bool result;
			FName id = Map->GetProvinceID(FVector(color.R, color.G, color.B), result);


			if (result && !bProvinceSelected)
			{
				Map->UpdateProvinceHovered(color);
			}
			else if (!bProvinceSelected)
			{
				Map->UpdateProvinceHovered(color);
			}
		}
	}
	else
	{
		if (Map && !bProvinceSelected)
			Map->UpdateProvinceHovered(FColor(0, 0, 0, 0));
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
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &ABirdEyeController::MouseClick);
		EnhancedInputComponent->BindAction(CameraMoveAction, ETriggerEvent::Started, this, &ABirdEyeController::CameraMovement);

		EnhancedInputComponent->BindAction(MouseScrollAction, ETriggerEvent::Started, this, &ABirdEyeController::CameraZoom);
		
		EnhancedInputComponent->BindAction(OpenCountryEditorAction, ETriggerEvent::Started, this, &ABirdEyeController::ToggleCountryEditor);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

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
		if (!GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params))
			return;
		if (!IsValid(hit.GetActor()))
			return;

		if (!Map)
			Map = Cast<AInteractiveMap>(hit.GetActor());

		if (Map)
		{
			FVector2D uvs = FVector2D(0, 0);
			UGameplayStatics::FindCollisionUV(hit, 0, uvs);
			FColor color = Map->GetColorFromLookUpTexture(uvs);
				
			bool result;
			FName id = Map->GetProvinceID(FVector(color.R, color.G, color.B), result);
			//if(!result)


			FProvinceData* data = Map->GetProvinceData(id);
			if (data)
			{
				Map->UpdateProvinceHovered(color);

				AManagerHUD* hud = Cast<AManagerHUD>(GetHUD());
				if (hud)
				{
					hud->SetInteractiveMapReference(Map);
					hud->DisplayProvinceEditorWidget(*data, id);
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
	input.Y *= -1;
	AMapPawn* pawn = GetPawn<AMapPawn>();
	pawn->MoveCamera(input);

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

void ABirdEyeController::ToggleCountryEditor()
{
	AManagerHUD* hud = Cast<AManagerHUD>(GetHUD());

	if (hud)
	{
		if (!Map)
		{
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInteractiveMap::StaticClass(), FoundActors);
			if (FoundActors.Num() > 0)
			{
				Map = Cast<AInteractiveMap>(FoundActors[0]);
				hud->SetInteractiveMapReference(Map);
			}
		}
		
		hud->SetInteractiveMapReference(Map);
		hud->ToggleCountryEditorVisibility();
	}
}

