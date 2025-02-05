// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DataStructs.generated.h"

//// TEST FILE WITH EXAMPLE STRUCTS

USTRUCT(BlueprintType)
struct FTestBasic
{
    GENERATED_BODY()
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
    int32 ID = -1;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
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
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite,  Category = "Data")
    int32 ID = -1;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite,  Category = "Data")
    int32 Population = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite,  Category = "Data")
    FString Name = "Name";
    // bool Type = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite,  Category = "Data")
    FLinearColor Color = FLinearColor::Black;
    FString ToString() const
    {
        return FString::Printf(TEXT("Id: %d, Name: %s, Population: %d"), ID, *Name, Population);
    }
};

