// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapEditorPreset.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);

USTRUCT(BlueprintType)
struct FModuleDefinition
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* CornerMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* WallMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* MaterialOverride;
};

UCLASS()
class MAPEDITOR_API UMapEditorPreset : public UObject
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FModuleDefinition GroundFloor;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FModuleDefinition RegularFloor;
	
	FOnAssetChanged OnObjectChanged;

};
