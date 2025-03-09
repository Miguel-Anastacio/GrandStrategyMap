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
	void InitFromStruct(const FName& PropertyName, const FInstancedStruct& InstancedStruct) const;

	virtual void InitFromStruct_Implementation(const FName& PropertyName, const FInstancedStruct& InstancedStruct) const
	{
		InitFromData(PropertyName, InstancedStruct.GetScriptStruct()->GetClass(), InstancedStruct.GetMemory());
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Generic Widget Interface")
	void InitFromUObject(const FName& PropertyName, const UObject* Object) const;

	virtual void InitFromUObject_Implementation(const FName& PropertyName, const UObject* Object) const
	{
		InitFromData(PropertyName, Object->GetClass(), Object);
	};
	
	virtual void InitFromData(const FName& PropertyName, const UClass* ClassType, const void* Data) const = 0;
protected:
};

