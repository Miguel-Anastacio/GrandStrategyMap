// Copyright 2024 An@stacioDev All rights reserved.
#include "Game/EdgeScrollComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Engine/GameViewportClient.h"

UEdgeScrollComponent::UEdgeScrollComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called every frame
void UEdgeScrollComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwningController)
		OwningController = Cast<APlayerController>(GetOwner());

	if (OwningController)
		HandleEdgeScroll(DeltaTime);
}

void UEdgeScrollComponent::HandleEdgeScroll(float DeltaTime) const
{
	if (!GEngine->GameViewport->IsFocused(GEngine->GameViewport->Viewport))
		return;
	
	int32 ScreenX, ScreenY;
	OwningController->GetViewportSize(ScreenX, ScreenY);

	float MouseX, MouseY;
	// don't scroll when not inside viewport
	if (!OwningController->GetMousePosition(MouseX, MouseY))
		return;
	
	FVector2D Direction = GetDirection(FVector2f(ScreenX, ScreenY), FVector2f(MouseX, MouseY));
	
	if (!Direction.IsZero())
	{
		Direction.Normalize();
		
		APawn* MyPawn = Cast<APawn>(OwningController->GetPawn());
		if (MyPawn)
		{
			const FVector WorldDirection = (ScrollAxisX * Direction.X) + (ScrollAxisY * Direction.Y);
			const FVector CurrentLocation = MyPawn->GetActorLocation();
			
			FVector Delta = WorldDirection * ScrollSpeed * DeltaTime;
			const FVector NewLocation = CurrentLocation + Delta;

			// Project new location onto ScrollAxisY to check if it exceeds limits
			const float ProjectedY = FVector::DotProduct(NewLocation, ScrollAxisY.GetSafeNormal());
			const float ClampedProjectedY = FMath::Clamp(ProjectedY, VerticalLimitMovement.X, VerticalLimitMovement.Y);

			// If clamping was needed, remove the excess from the Y scroll axis component
			if (!FMath::IsNearlyEqual(ProjectedY, ClampedProjectedY))
			{
				const FVector YComponent = ScrollAxisY.GetSafeNormal() * FVector::DotProduct(Delta, ScrollAxisY.GetSafeNormal());
				const FVector AllowedYDelta = ScrollAxisY.GetSafeNormal() * (ClampedProjectedY - FVector::DotProduct(CurrentLocation, ScrollAxisY.GetSafeNormal()));
				Delta = Delta - YComponent + AllowedYDelta;
			}
			
			MyPawn->AddActorWorldOffset(Delta, true);
		}
	}
}

FVector2D UEdgeScrollComponent::GetDirection(const FVector2f& ViewportSize, const FVector2f& MousePosition) const
{
	return FVector2D(
			GetDirectionPerAxis(ViewportSize.X, MousePosition.X), 
			GetDirectionPerAxis(ViewportSize.Y, MousePosition.Y)
			);
}

float UEdgeScrollComponent::GetDirectionPerAxis(float ViewportSize, float MousePosition) const
{
	const float EdgeThreshold = ViewportSize * EdgeThresholdPercentage;

	if (MousePosition <= EdgeThreshold)
		return FMath::Clamp(-(1.f - (MousePosition / EdgeThreshold)), -1.f, 0.f);

	if (MousePosition >= ViewportSize - EdgeThreshold)
		return FMath::Clamp(1.f - ((ViewportSize - MousePosition) / EdgeThreshold), 0.f, 1.f);

	return 0.f;
}
