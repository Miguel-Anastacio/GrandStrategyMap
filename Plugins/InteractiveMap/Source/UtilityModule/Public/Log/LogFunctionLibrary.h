// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
// #include "UtilityModule/Public/UtilityModule.h"
#include "LogFunctionLibrary.generated.h"

UCLASS()
class UTILITYMODULE_API ULogFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    template<typename T>
    static void LogArray(const TArray<T>& Array, const FString& Message)
    {
        UE_LOG(LogTemp, Display, TEXT("Logging Array, %s"), *Message);
        for(const T& Value : Array)
        {
            UE_LOG(LogTemp, Display, TEXT("%s"), *Value.ToString());
        }
    }
    static void LogReadJsonFailed(const FString& FilePath);
};

