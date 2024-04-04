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

	UFUNCTION(BlueprintCallable, Category = "MapVisual")
	void InitVisualComponentFromOriginal(UMapVisualComponent* mapVisual);

	UFUNCTION(BlueprintCallable, Category = "MapVisual")
	void InitVisualComponents(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder, 
		UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap);

	UFUNCTION(BlueprintCallable, Category = "MapVisual")
	UStaticMeshComponent* InitMeshComponent(UStaticMeshComponent* original);

	UFUNCTION(BlueprintCallable, Category = "MapVisual")
	void InitMeshProperty(UStaticMeshComponent* original, UStaticMeshComponent* meshToUpdate);
	UFUNCTION(BlueprintCallable, Category = "MapVisual")
	void UpdateMeshMaterial(UStaticMeshComponent* meshToUpdate, UMaterialInterface* mat);

	UFUNCTION(BlueprintCallable, Category = "MapVisual")
	UStaticMeshComponent* GetMeshComponent(MapMode mode);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
	UStaticMeshComponent* GetMapSelectMeshComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
	UStaticMeshComponent* GetMapGameplayMeshComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
	UStaticMeshComponent* GetMapBorderMeshComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
	UStaticMeshComponent* GetMapTerrainMeshComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapVisual")
	FVector CalculateSizeOfMap() const;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MapSelectMesh;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MapBorderMesh;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> GameplayMapMesh;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> TerrainMapMesh;

		
};
