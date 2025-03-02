// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "UObject/Interface.h"
#include "GenericUserWidgetInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UGenericUserWidgetInterface: public UInterface
{
	GENERATED_BODY()
};

class DATABASEDWIDGET_API IGenericUserWidgetInterface
{
	GENERATED_BODY()

public:
	// Example of a function declaration in the interface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Generic Widget Interface")
	void PerformAction(const FName& PropertyName, const FInstancedStruct& InstancedStruct) const;
};