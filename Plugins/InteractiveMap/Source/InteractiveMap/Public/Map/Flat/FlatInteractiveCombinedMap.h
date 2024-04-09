// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Map/Flat/FlatInteractiveMap.h"
#include "FlatInteractiveCombinedMap.generated.h"
/**
 *  FlatMap base class -
 */
class UMapLimitComponent;
class UMapVisualComponent;
UCLASS()
class INTERACTIVEMAP_API AFlatInteractiveCombinedMap : public AFlatInteractiveMap
{
	GENERATED_BODY()

	AFlatInteractiveCombinedMap(const FObjectInitializer& ObjectInitializer);
//protected:
//	void BeginPlay() override;
//	void InitializeMap() override;
//
//	void SetMapMode_Implementation(MapMode mode) override;
//	void UpdateLimitComponent(UMapVisualComponent* mapLimit, MapMode mode, UStaticMeshComponent* originalMesh);
//
//	/** Map limit component. */
//	UPROPERTY(EditAnywhere, Category = "Limit", BlueprintReadOnly)
//	TObjectPtr<class UMapLimitComponent> LeftMapLimit;
//	/** Map limit component. */
//	UPROPERTY(EditAnywhere, Category = "Limit", BlueprintReadOnly)
//	TObjectPtr<class UMapLimitComponent> RightMapLimit;
//
//	/** The size of the box overlap on X as a percentage of the map size values between 0 a 0.5*/
//	UPROPERTY(EditAnywhere, Category = "Limit")
//	float BoxPercentageOfMap = 0.1;
};

