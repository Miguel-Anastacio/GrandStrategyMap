// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Map/MapEnums.h"
#include "Components/SceneComponent.h"
#include "MapVisualComponent.generated.h"

class UStaticMeshComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTIVEMAP_API UMapVisualComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMapVisualComponent();
	
	void AttachMeshes(USceneComponent* root);

	UFUNCTION(BlueprintCallable)
	void InitVisualComponentFromOriginal(UMapVisualComponent* mapVisual);

	UFUNCTION(BlueprintCallable)
	void InitVisualComponents(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder, 
		UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap);

	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* InitMeshComponent(UStaticMeshComponent* original);

	UFUNCTION(BlueprintCallable)
	void InitMeshProperty(UStaticMeshComponent* original, UStaticMeshComponent* meshToUpdate);
	UFUNCTION(BlueprintCallable)
	void UpdateMeshMaterial(UStaticMeshComponent* meshToUpdate, UMaterialInterface* mat);

	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetMeshComponent(MapMode mode);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStaticMeshComponent* GetMapSelectMeshComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStaticMeshComponent* GetMapGameplayMeshComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStaticMeshComponent* GetMapBorderMeshComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStaticMeshComponent* GetMapTerrainMeshComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector CalculateSizeOfMap() const;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	//UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	//TObjectPtr<class USceneComponent> RootComponent;

	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MapSelectMesh;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MapBorderMesh;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> GameplayMapMesh;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> TerrainMapMesh;

		
};
