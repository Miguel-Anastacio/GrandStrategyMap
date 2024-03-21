// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapEnums.generated.h"

UENUM(BlueprintType)
enum class MapMode : uint8
{
	POLITICAL UMETA(DisplayName = "Political"),
	RELIGIOUS   UMETA(DisplayName = "Religious"),
	CULTURAL   UMETA(DisplayName = "Cultural"),
	TERRAIN   UMETA(DisplayName = "Terrain")
};

