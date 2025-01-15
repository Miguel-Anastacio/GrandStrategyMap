// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "UObject/Interface.h"
#include "GenericUserWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UGenericUserWidgetInterface: public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that defines a contract for interacting with objects.
 */
class UTILITYMODULE_API IGenericUserWidgetInterface
{
	GENERATED_BODY()

public:
	// Example of a function declaration in the interface
	// UFUNCTION(BlueprintCallable)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PerformAction(const FName& PropertyName, const FInstancedStruct& InstancedStruct) const;
};