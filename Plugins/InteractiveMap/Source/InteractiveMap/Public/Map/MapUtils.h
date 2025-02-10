// Copyright 2024 An@stacioDev All rights reserved. 
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MapUtils.generated.h"

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
