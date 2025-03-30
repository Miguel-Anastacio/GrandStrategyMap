// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "ADStructUtilsFunctionLibrary.h"
#include "InstancedStruct.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "PropertyUtilityFunctionLibrary.generated.h"

class UObject;
UCLASS(DisplayName="Property Utility Function Library")
class UTILITYMODULE_API UAtkPropertyUtilityFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // From a UObject, get a specific property by name and return it as a UType defined by ObjClass
    UFUNCTION( BlueprintCallable, BlueprintPure, Category=PropertyUtility, meta = (DeterminesOutputType = "ObjClass") ) 
    static UObject* GetObjectFromProperty(const UObject* Container, const FName& PropertyName, TSubclassOf<class UObject> ObjClass);
    
    // From a InstancedStruct, get a specific property by name and return it as a UType defined by ObjClass
    UFUNCTION( BlueprintCallable, BlueprintPure, Category=PropertyUtility, meta = (DeterminesOutputType = "ObjClass") ) 
    static UObject* GetObjectFromPropertyStruct(const FInstancedStruct& InstancedStruct, const FName& PropertyName, TSubclassOf<class UObject> ObjClass);
    
    // From a InstancedStruct, get a specific property by name and return it as a InstancedStruct
    UFUNCTION( BlueprintCallable, BlueprintPure, Category=PropertyUtility) 
    static FInstancedStruct GetStructFromPropertyInstancedStruct(const FInstancedStruct& InstancedStruct, const FName& PropertyName);
    
    // From a UObject, get a specific property by name and return it as a InstancedStruct
    UFUNCTION( BlueprintCallable, BlueprintPure, Category=PropertyUtility) 
    static FInstancedStruct GetStructFromPropertyObj(const UObject* Object, const FName& PropertyName);
    
    static bool IsPropertyOfType(const FProperty* Property, const UStruct* Class);
    static FInstancedStruct GetStructFromProperty(const FProperty* Property, const void* Object);
    
    // From a InstancedStruct, get a specific property by name and return it as a T (T must inherit from UObject
    template <typename T>
    static T* GetPropertyAsObjectType(const FInstancedStruct& Struct, const FName& PropertyName)
    {
        if(!Struct.IsValid())
        {
            return nullptr;
        }
        
        FProperty* Property = UAtkStructUtilsFunctionLibrary::FindPropertyByDisplayName(Struct.GetScriptStruct(), PropertyName);
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
        
        FProperty* Property = UAtkStructUtilsFunctionLibrary::FindPropertyByDisplayName(Object->StaticClass(), PropertyName);
        return GetPropertyAs<T>(Property, T::StaticClass(), Object);
    }

    template <typename T>
    static T* GetPropertyAs(const FProperty* Property, const UStruct* Class, const void* ContainerPtr)
    {
        if(!Property || !ContainerPtr)
            return nullptr;
        
        // Check if it's an object property first
        const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property);
        if (ObjectProperty)
        {
            return HandleObjectProperty<T>(ObjectProperty, Class, ContainerPtr);
        }

        return nullptr;
        
    }

    template <typename T>
    static T* HandleObjectProperty(const FObjectProperty* ObjectProperty, const UStruct* Class, const void* ContainerPtr)
    {
        if(!ObjectProperty || !ContainerPtr)
            return nullptr;
        
        const UClass* PropertyClass = ObjectProperty->PropertyClass;
        if (!PropertyClass || (!PropertyClass->IsChildOf(Class)) && PropertyClass != Class)
        {
            return nullptr;
        }
        UObject* ObjectValue = ObjectProperty->GetObjectPropertyValue(ObjectProperty->ContainerPtrToValuePtr<void>(ContainerPtr));
        return Cast<T>(ObjectValue);
    }
};


