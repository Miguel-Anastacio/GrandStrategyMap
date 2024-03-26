// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Map/MapEnums.h"
#include "Components/SceneComponent.h"
#include "MapVisualComponent.generated.h"

class UStaticMeshComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLICKABLEMAP_API UMapVisualComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMapVisualComponent();
	void InitVisualComponents(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder, 
		UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap);

	UStaticMeshComponent* InitMeshComponent(UStaticMeshComponent* original);

	void UpdateMeshMaterial(UStaticMeshComponent* meshToUpdate, UMaterialInterface* mat);

	UStaticMeshComponent* GetMeshComponent(MapMode mode);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class USceneComponent> RootComponent;

	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MapSelectMesh;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MapBorderMesh;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> GameplayMapMesh;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> TerrainMapMesh;

		
};
