// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ChildActorComponent.h"
#include "MapLimitComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class CLICKABLEMAP_API UMapLimitComponent : public UChildActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMapLimitComponent();
	UFUNCTION(BlueprintCallable)
	void UpdateGameplayMapMesh(class UMaterialInstanceDynamic* mat);
	UFUNCTION(BlueprintCallable)
	void InitLimitComponent(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder,
		UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UMapVisualComponent* GetVisualComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limit")
	TObjectPtr<class UCapsuleComponent> Capsule;
	// UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	//TObjectPtr<class UStaticMeshComponent> Root;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UMapVisualComponent> MapVisualComponent;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limit")
	//TObjectPtr<class USceneComponent> Root;

	float TimeSinceOverlap = 10.0f;


		
};
