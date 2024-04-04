// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Map/InteractiveMap.h"
#include "FlatInteractiveMap.generated.h"

/**
 *  FlatMap base class - 
 */
class UMapLimitComponent;
UCLASS()
class INTERACTIVEMAP_API AFlatInteractiveMap : public AInteractiveMap
{
	GENERATED_BODY()

	AFlatInteractiveMap();
protected:
	void BeginPlay() override;


};
