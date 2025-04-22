// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 5, 0)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "UObject/Interface.h"
#include "DataDrivenUserWidgetInterface.generated.h"
UINTERFACE(MinimalAPI, BlueprintType, DisplayName = "DataDrivenUserWidgetInterface")
class UPropGenDataDrivenUserWidgetInterface : public UInterface
{
    GENERATED_BODY() // Unreal Engine macro that generates required interface code
};

class DATABASEDWIDGET_API IPropGenDataDrivenUserWidgetInterface
{
    GENERATED_BODY()

public:
    /**
     * Initializes widget from an instanced struct
     * @param PropertyName - Name of the property to initialize
     * @param InstancedStruct - Struct containing the data
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Data Driven Widget Interface")
    void InitFromStruct(const FName &PropertyName, const FInstancedStruct &InstancedStruct) const;

    /**
     * Default implementation for initializing from struct
     * Validates the struct and passes it to the lower-level InitFromData method
     */
    virtual void InitFromStruct_Implementation(const FName &PropertyName, const FInstancedStruct &InstancedStruct) const
    {
        if (InstancedStruct.IsValid())
        {
            InitFromData(PropertyName, InstancedStruct.GetScriptStruct(), InstancedStruct.GetMemory());
        }
    }

    /**
     * Initializes widget from a UObject
     * @param PropertyName - Name of the property to initialize
     * @param Object - UObject containing the data
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Data Driven Widget Interface")
    void InitFromUObject(const FName &PropertyName, const UObject *Object) const;

    /**
     * Default implementation for initializing from UObject
     * Passes the object to the lower-level InitFromData method
     */
    virtual void InitFromUObject_Implementation(const FName &PropertyName, const UObject *Object) const
    {
        InitFromData(PropertyName, Object->GetClass(), Object);
    };

    /**
     * Low-level implementation to initialize widget fields
     * This is a common path for both initialization methods
     * @param PropertyName - Name of the property to initialize
     * @param ClassType - Type information for the data
     * @param Data - Pointer to the actual data
     */
    virtual void InitFromData(const FName &PropertyName, const UStruct *ClassType, const void *Data) const {};
};
