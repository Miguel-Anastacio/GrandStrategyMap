// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include <string_view>

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MiscFunctionLibrary.generated.h"

UCLASS(DisplayName = "MiscFunctionLibrary")
class UTILITYMODULE_API UAtkMiscFunctionLibrary: public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static void ExecuteSlowTaskWithProgressBar(TFunction<void(TFunction<void(float, std::string_view)>)> Task);
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MiscFunctionLibrary")
    static FColor ConvertHexStringToRGB(const FString& Color);
private:
    static auto HexToDecimal(const FString& Hex) -> int32;
  
};

