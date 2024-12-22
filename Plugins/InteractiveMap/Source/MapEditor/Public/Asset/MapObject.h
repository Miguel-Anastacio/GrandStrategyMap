// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "MapObject.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);

UCLASS()
class MAPEDITOR_API UMapObject : public UObject
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Object")
	TSubclassOf<AActor> RepresentingActorClass = nullptr;
	
	FOnAssetChanged OnObjectChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Object")
	float Radius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Object")
	float Speed = 1.f;
};
