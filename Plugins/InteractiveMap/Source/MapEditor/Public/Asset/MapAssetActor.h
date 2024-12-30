// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapAssetActor.generated.h"
class UMapObject;
class UStaticMeshComponent;
UCLASS()
class MAPEDITOR_API AMapAsset : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapAsset")
	UStaticMeshComponent* MapMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapAsset")
	UMapObject* MapObject;

protected:
	AMapAsset();
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Destroyed() override;
};
