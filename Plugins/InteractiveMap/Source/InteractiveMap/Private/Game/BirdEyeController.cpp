// Copyright 2024 An@stacioDev All rights reserved.

#include "Game/BirdEyeController.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Game/MapPawn.h"
#include "Map/ClickableMap.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ManagerHUD.h"
#include "Components/SlateWrapperTypes.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"
#include "InteractiveMap.h"
#include "Engine/LocalPlayer.h"

ABirdEyeController::ABirdEyeController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ABirdEyeController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	MapPawn = GetPawn<AMapPawn>();
}

void ABirdEyeController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FHitResult Hit;
	bool OutResultUnderCursor = false;
	AClickableMap* TempMap = PerformLineTraceToFindMap(Hit, OutResultUnderCursor);
	if (OutResultUnderCursor)
	{
		if(!Map)
			Map = TempMap;
		if (Map)
		{
			FVector2D Uvs = FVector2D(0, 0);
			UGameplayStatics::FindCollisionUV(Hit, 0, Uvs);
			const FColor Color = Map->GetColorFromLookUpTexture(Uvs);

			bool bResult;
			const int ID = Map->GetProvinceID(Color, bResult);

			if (bResult && !bProvinceSelected)
			{
				ProvinceHoveredDelegate.Broadcast(Color, ID);
			}
			else if (!bProvinceSelected)
			{
				ProvinceHoveredDelegate.Broadcast(Color, ID);
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
#if WITH_EDITOR
		EnhancedInputComponent->BindAction(DisplayLookupTextureAction, ETriggerEvent::Started, this, &ABirdEyeController::SetLookupTextureActive);
#endif
		
	}
	else
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABirdEyeController::MouseClick()
{
	bProvinceSelected = false;
	FHitResult OutHit;
	bool OutResultUnderCursor = false;
	AClickableMap* TempMap = PerformLineTraceToFindMap(OutHit, OutResultUnderCursor);
	if (OutResultUnderCursor)
	{
		if (!Map)
			Map = TempMap;

		if (Map)
		{
			FVector2D Uvs = FVector2D(0, 0);
			UGameplayStatics::FindCollisionUV(OutHit, 0, Uvs);
			const FColor Color = Map->GetColorFromLookUpTexture(Uvs);
				
			bool bOutResult;
			const int ID = Map->GetProvinceID(Color, bOutResult);
			UE_LOG(LogInteractiveMap, Warning, TEXT("Clicked on province ID: %d"),ID);
			UE_LOG(LogInteractiveMap, Warning, TEXT("Color searched: %s"), *Color.ToString());
			UE_LOG(LogInteractiveMap, Warning, TEXT("UVs: %s"), *Uvs.ToString());
			
			FInstancedStruct* Data = Map->GetProvinceData(ID);
			if (Data)
			{
				ProvinceHoveredDelegate.Broadcast(Color, ID);

				ProvinceClickedDelegate.Broadcast(ID, *Data);
				AManagerHUD* ManagerHUD = Cast<AManagerHUD>(GetHUD());
				if (ManagerHUD)
				{
					ManagerHUD->SetInteractiveMapReference(Map);
				}
			}
			else
			{
				MapClickedDelegate.Broadcast();
				GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, FString("Error"));
			}
		}
	}
	if (!bProvinceSelected)
	{
		MapClickedDelegate.Broadcast();
	}
}

void ABirdEyeController::CameraMovement(const FInputActionInstance& Instance)
{
	FVector2D Input = Instance.GetValue().Get<FVector2D>();
	Input.Y *= -1;
	AMapPawn* pawn = GetPawn<AMapPawn>();
	pawn->MoveCamera(Input);

}

void ABirdEyeController::CameraZoom(const FInputActionInstance& Instance)
{
	float Input = Instance.GetValue().Get<float>();
	Input *= -1;
	AMapPawn* pawn = GetPawn<AMapPawn>();
	pawn->ZoomCamera(Input);
}

void ABirdEyeController::StartMovement(const FVector2D& MousePos)
{
	bMoveCamera = true;
	const FVector2D ViewportSize = FVector2D(GEngine->GameUserSettings->GetLastConfirmedScreenResolution());
	ViewportCenter = ViewportSize * 0.5f;

	MoveInput = MousePos - ViewportCenter; 
	MoveInput.Normalize();

}

void ABirdEyeController::HideHUD()
{
	AManagerHUD* ManagerHUD = Cast<AManagerHUD>(GetHUD());
	if (ManagerHUD)
	{
		ManagerHUD->SetProvinceEditorVisibility(ESlateVisibility::Collapsed);
	}
}

void ABirdEyeController::ShowProvinceInfo(int Id, const FInstancedStruct& Data)
{
	AManagerHUD* HUD = Cast<AManagerHUD>(GetHUD());
	if (HUD)
	{
		HUD->SetInteractiveMapReference(Map);
		HUD->DisplayProvinceEditorWidget(Data, Id);
		bProvinceSelected = true;
	}
}

void ABirdEyeController::HighlightProvince(const FColor& Color)
{
	Map->UpdateProvinceHovered(Color);
}
#if WITH_EDITOR
void ABirdEyeController::SetLookupTextureActive()
{
	Map->SetMapMode("Debug");
}
#endif

AClickableMap* ABirdEyeController::PerformLineTraceToFindMap(FHitResult& OutHit, bool& OutResultUnderCursor) const
{
	OutResultUnderCursor = GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(MouseTraceChannel), true, OutHit);
	if(OutResultUnderCursor)
	{
		FVector Start = OutHit.ImpactPoint;
		FVector End = OutHit.ImpactPoint;
		Start.Z = OutHit.ImpactPoint.Z + zOffset;
		End.Z = OutHit.ImpactPoint.Z - zOffset;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		params.bTraceComplex = true;
		params.bReturnFaceIndex = true;
		if (!GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, MouseTraceChannel, params))
			return nullptr;
		if (!IsValid(OutHit.GetActor()))
			return nullptr;
		return Cast<AClickableMap>(OutHit.GetActor());
	}
	
	return  nullptr;
}

