// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Map/ClickableMap.h"
#include "FlatInteractiveMap.generated.h"
/**
 *  FlatMap base class -
 */
class UMapLimitComponent;
class UMapVisualComponent;
UCLASS(Abstract, NotBlueprintable)
class INTERACTIVEMAP_API AFlatInteractiveMap : public AClickableMap
{
	GENERATED_BODY()

protected:
	AFlatInteractiveMap(const FObjectInitializer& ObjectInitializer);
	void BeginPlay() override;
	virtual void InitializeMap_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Limit")
	void UpdateLimits();


	/** Map limit component. */
	UPROPERTY(EditAnywhere, Category = "Limit", BlueprintReadOnly)
	TObjectPtr<class UMapLimitComponent> LeftMapLimit;
	/** Map limit component. */
	UPROPERTY(EditAnywhere, Category = "Limit", BlueprintReadOnly)
	TObjectPtr<class UMapLimitComponent> RightMapLimit;

	/** The size of the box overlap on X as a percentage of the map size values between 0 to 0.5*/
	UPROPERTY(EditAnywhere, Category = "Limit", meta=(ClampMin=0.05, ClampMax=0.5))
	float BoxPercentageOfMap = 0.1;
};

