// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "VisualProperties.generated.h"

USTRUCT(BlueprintType)
struct FVisualPropertyType : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,  Category = "Visual Property Type")
	class UMaterialInterface* MaterialInstance = nullptr;

	UPROPERTY(EditAnywhere, Category = "Visual Property Type")
	FName Type;

	bool operator == (const FVisualPropertyType& Other) const
	{
		return Other.Type == Type;
	}
	friend uint32 GetTypeHash (const FVisualPropertyType& Other)
	{
		return GetTypeHash(Other.Type);
	}

	FVisualPropertyType(const FName& Name) : MaterialInstance(nullptr), Type(Name){};
	FVisualPropertyType() : MaterialInstance(nullptr), Type(){};
};


USTRUCT(BlueprintType)
struct FVisualProperty : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual Property")
	FName Type = "Null";
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual Property")
	FName Tag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category = "Visual Property")
	FColor Color = FColor::Black;
};

USTRUCT(BlueprintType)
struct FArrayOfVisualProperties
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FVisualProperty> VisualProperties;
};