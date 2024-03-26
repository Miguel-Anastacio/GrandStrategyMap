// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Map/InteractiveMap.h"
#include "FlatInteractiveMap.generated.h"

/**
 * 
 */
class UMapLimitComponent;
UCLASS()
class CLICKABLEMAP_API AFlatInteractiveMap : public AInteractiveMap
{
	GENERATED_BODY()

	AFlatInteractiveMap();
	void BeginPlay() override;
	void SetMapMode(MapMode mode) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limits")
	TObjectPtr<UMapLimitComponent> LeftLimitComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limits")
	TObjectPtr<UMapLimitComponent> RightLimitComponent;
	
};
