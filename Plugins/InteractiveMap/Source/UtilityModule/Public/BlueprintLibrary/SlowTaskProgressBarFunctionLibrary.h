// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include <string_view>

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SlowTaskProgressBarFunctionLibrary.generated.h"

UCLASS()
class UTILITYMODULE_API USlowTaskProgressBarFunctionLibrary: public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // UFUNCTION(BlueprintCallable, Category = "SlowTaskProgressBar")
    static void ExecuteSlowTaskWithProgressBar(TFunction<void(TFunction<void(float, std::string_view)>)> Task);
  
};

