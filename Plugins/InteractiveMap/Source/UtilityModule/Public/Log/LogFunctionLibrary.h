// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UserWidgets/CustomStructWidget.h"
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
    
    static FString InstancedStructToString(const FInstancedStruct& Instance, const UScriptStruct* StructType)
    {
        FString result;
        const void* StructData = Instance.GetMemory();
        if (!StructData)
        {
            UE_LOG(LogTemp, Error, TEXT("StructData is null."));
            return TEXT("");
        }
        for (TFieldIterator<FProperty> It(StructType); It; ++It)
        {
            FProperty* Property = *It;
            if (!Property)
            {
                UE_LOG(LogTemp, Warning, TEXT("Encountered null property in struct '%s'."), *StructType->GetName());
                continue;
            }
            bool bResult = false;
            const FName PropertyName = Property->GetFName();
            FString PropertyValue = UGenericStructWidget::GetPropertyValueAsString(Property, &StructData, bResult);
            if(bResult)
                result.Append(FString::Printf(TEXT("Property: %s : Value %s\n"), *PropertyName.ToString(), *PropertyValue));
        }
        return result;
    }
    
    static void LogArrayInstancedStructs(const TArray<FInstancedStruct>& Array, const FString& Message, UScriptStruct* StructType)
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
        for(const FInstancedStruct& Value : Array)
        {
            UE_LOG(LogTemp, Display, TEXT("%s"), *InstancedStructToString(Value, StructType));
        }
    }
   
    static void LogReadJsonFailed(const FString& FilePath);
};

inline void ULogFunctionLibrary::LogReadJsonFailed(const FString& FilePath)
{
    
}

