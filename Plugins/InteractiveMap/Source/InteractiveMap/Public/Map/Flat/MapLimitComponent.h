// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MapLimitComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INTERACTIVEMAP_API UMapLimitComponent : public USceneComponent
{

	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMapLimitComponent();
	UFUNCTION(BlueprintCallable, Category = "Limit")
	void InitLimitComponent(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder,
		UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Visual")
	class UMapVisualComponent* GetVisualComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION(Category = "Overlap")
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limit")
	TObjectPtr<class UBoxComponent> Box;

	UPROPERTY(EditAnywhere, Category = "Visual", BlueprintReadOnly)
	TObjectPtr<class UMapVisualComponent> MapVisualComponent;



		
};
