// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	void CreateLookUpTable();
protected:
	UPROPERTY(EditAnywhere, Category = "Selection", BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MapSelectMesh;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* MapDataTable;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextureRenderTarget2D> MapRenderTarget;

	UPROPERTY(BlueprintReadOnly)
	TMap<FVector, FString> LookUpTable;
};
