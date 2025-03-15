// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "ADStructUtilsFunctionLibrary.h"
#include "InstancedStruct.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PropertyUtilityFunctionLibrary.generated.h"

UCLASS()
class UTILITYMODULE_API UPropertyUtilityFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // From a UObject, get a specific property by name and return it as a UType defined by ObjClass
    UFUNCTION( BlueprintCallable, BlueprintPure, Category=PropertyUtility, meta = (DeterminesOutputType = "ObjClass") ) 
    static UObject* GetObjectFromProperty(const UObject* Container, const FName& PropertyName, TSubclassOf<class UObject> ObjClass);
    
    // From a InstancedStruct, get a specific property by name and return it as a UType defined by ObjClass
    UFUNCTION( BlueprintCallable, BlueprintPure, Category=PropertyUtility, meta = (DeterminesOutputType = "ObjClass") ) 
    static UObject* GetObjectFromPropertyStruct(const FInstancedStruct& InstancedStruct, const FName& PropertyName, TSubclassOf<class UObject> ObjClass);
    
    static bool IsPropertyOfType(const FProperty* Property, const UStruct* Class);
    
    // From a InstancedStruct, get a specific property by name and return it as a T (T must inherit from UObject
    template <typename T>
    static T* GetPropertyAsObjectType(const FInstancedStruct& Struct, const FName& PropertyName)
    {
        if(!Struct.IsValid())
        {
            return nullptr;
        }
        
        FProperty* Property = UADStructUtilsFunctionLibrary::FindPropertyByDisplayName(Struct.GetScriptStruct(), PropertyName);
        return GetPropertyAs<T>(Property, T::StaticClass(), Struct.GetMemory());
    }
    
    // From a UObject, get a specific property by name and return it as a T (T must inherit from UObject)
    template <typename T>
    static T* GetPropertyAsObjectType(const UObject* Object, const FName& PropertyName)
    {
        if(!Object)
        {
            return nullptr;
        }
        
        FProperty* Property = UADStructUtilsFunctionLibrary::FindPropertyByDisplayName(Object->StaticClass(), PropertyName);
        return GetPropertyAs<T>(Property, T::StaticClass(), Object);
    }

    template <typename T>
    static T* GetPropertyAs(const FProperty* Property, const UStruct* Class, const void* ContainerPtr)
    {
        if(!Property || !ContainerPtr)
            return nullptr;
        
        // Check if it's an object property first
        const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property);
        if (!ObjectProperty)
        {
            return nullptr;
        }
    
        // Make sure it's of same UClass property
        const UClass* PropertyClass = ObjectProperty->PropertyClass;
        if (!PropertyClass || (!PropertyClass->IsChildOf(Class)) && PropertyClass != Class)
        {
            return nullptr;
        }
        
        // Get the UTexture2D* from the container
        UObject* ObjectValue = ObjectProperty->GetObjectPropertyValue(ObjectProperty->ContainerPtrToValuePtr<void>(ContainerPtr));
        return Cast<T>(ObjectValue);
    }
};


