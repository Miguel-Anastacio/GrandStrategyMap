// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EdgeScrollComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERACTIVEMAP_API UEdgeScrollComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEdgeScrollComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

protected:	
	/** Vertical movement limits. (x top, y bottom) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Edge Scrolling")
	FVector2D VerticalLimitMovement = FVector2D(-3000, 3000);
	
private:
	void HandleEdgeScroll(float DeltaTime) const;
	FVector2D GetDirection(const FVector2f& ViewportSize, const FVector2f& MousePosition) const;
	float GetDirectionPerAxis(float ViewportSize, float MousePosition) const;
	
	// Distance in screen pixels from the viewport edge at which edge scrolling starts, in percentage of size of viewport
	UPROPERTY(EditAnywhere, Category = "Edge Scrolling", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EdgeThresholdPercentage = 0.15f;

	// Movement speed applied while the cursor remains within the edge scroll threshold.
	UPROPERTY(EditAnywhere, Category = "Edge Scrolling")
	float ScrollSpeed = 1000.f;
	
	UPROPERTY(EditAnywhere, Category = "Edge Scrolling")
	FVector ScrollAxisX = FVector(0.f, 1.f, 0.f); // maps mouse X to world axis

	UPROPERTY(EditAnywhere, Category = "Edge Scrolling")
	FVector ScrollAxisY = FVector(1.f, 0.f, 0.f); // maps mouse Y to world axis
	
	UPROPERTY()
	APlayerController* OwningController;
	
	
	
};
