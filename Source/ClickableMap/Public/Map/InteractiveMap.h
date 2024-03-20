// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapUtils.h"
#include "InteractiveMap.generated.h"

class UTextureRenderTarget2D;
UCLASS()
class CLICKABLEMAP_API AInteractiveMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractiveMap();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTextureRenderTarget2D* GetMapRenderTarget() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TMap<FVector, FString> GetLookUpTable() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetProvinceID(const FVector& color) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SaveMapTextureData();
	void CreateLookUpTable();
	void CreatePoliticalMapTexture();
protected:
	UPROPERTY(EditAnywhere, Category = "Selection", BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MapSelectMesh;
	UPROPERTY(EditAnywhere, Category = "Selection", BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MapBorderMesh;
	UPROPERTY(EditAnywhere, Category = "Selection", BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> GameplayMapMesh;

	// Material that show the current gameplay map
	// Political religious or terrain
	UPROPERTY(EditAnywhere)
	UMaterialInterface* GameplayMapMaterial;
	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* TestPoliticalMapRenderTarget;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* MapDataTable;

	UPROPERTY(EditAnywhere, Category = "Map")
	TObjectPtr<UTexture2D> MapLookUpTexture;


	UPROPERTY()
	TObjectPtr<UTexture2D> PoliticalMapTexture;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextureRenderTarget2D> MapRenderTarget;

	UPROPERTY(BlueprintReadOnly)
	TMap<FVector, FString> LookUpTable;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FProvinceData> ProvinceDataMap;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FCountryData> CountryData;
	
	TArray<FColor> MapColorCodeTextureData;

};
