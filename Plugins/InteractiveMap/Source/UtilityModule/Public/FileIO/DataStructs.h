// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DataStructs.generated.h"

/**
 *  Struct that can hold any data of that type 
 */
USTRUCT(BlueprintType)
struct FVariantData : public FTableRowBase
{
    GENERATED_BODY()

    TMap<FString, TVariant<int, float, FString, bool, FLinearColor>> Properties;
};

USTRUCT(BlueprintType)
struct FTestBasic
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ID = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    FString ToString() const
    {
        return FString::Printf(TEXT("Id: %d, Name: %s"), ID, *Name);
    }
};

USTRUCT(Blueprintable)
struct FTestAdvanced
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ID = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Population = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;
    // bool Type = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor Color;
    FString ToString() const
    {
        return FString::Printf(TEXT("Id: %d, Name: %s, Population: %d"), ID, *Name, Population);
    }
};

