// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapDataSettingsPreset.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);

USTRUCT(BlueprintType)
struct FMapDataSettingsDefinition
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Map Data Settings")
	UScriptStruct* StructType;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Map Data Settings")
	FString FilePath;

};

UCLASS()
class MAPEDITOR_API UMapDataSettings : public UObject
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Map Data Settings")
	FMapDataSettingsDefinition MapDataSettings;
	
	FOnAssetChanged OnObjectChanged;

};
