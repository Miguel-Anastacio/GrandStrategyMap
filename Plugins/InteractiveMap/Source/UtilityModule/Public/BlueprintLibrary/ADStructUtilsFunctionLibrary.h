// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InstancedStruct.h"
#include "ADStructUtilsFunctionLibrary.generated.h"

UCLASS()
class UTILITYMODULE_API UADStructUtilsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    static FString GetPropertyValueAsString(FProperty* Property, const void* StructObject, bool& OutResult);

    static FProperty* GetPropertyByName(const UScriptStruct* StructType, const FString& PropertyName);

    template<typename T>
    static T GetPropertyValueFromStruct(const UScriptStruct* StructType, const FInstancedStruct& InstancedStruct, const FString& PropertyName, bool& OutResult)
    {
        OutResult = false;
        if(const FProperty* Property = GetPropertyByName(StructType, PropertyName))
        {
            T PropertyValue = GetPropertyValueAsType<T>(Property, InstancedStruct.GetMemory(), OutResult);
            if(OutResult)
            {
                return PropertyValue;
            }
        }
        return T();
    }
    
    template<typename T>
    static T GetPropertyValueAsType(const FProperty* Property, const void* StructObject, bool& OutResult)
    {
        OutResult = true;
        if (!Property || !StructObject)
        {
            OutResult = false;
        }
        const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructObject);

        if (const FIntProperty* IntProperty = CastField<FIntProperty>(Property))
        {
            return static_cast<T>(IntProperty->GetPropertyValue(ValuePtr));
        }
        else if (const FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
        {
            return static_cast<T>(FloatProperty->GetPropertyValue(ValuePtr)); 
        }
        else if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
        {
            return static_cast<T>(BoolProperty->GetPropertyValue(ValuePtr)); 
        }
        else if (const FStrProperty* StrProperty = CastField<FStrProperty>(Property))
        {
            if constexpr (std::is_same_v<T, FString>)
            {
                return StrProperty->GetPropertyValue(ValuePtr);
            }
            OutResult = false;
            return T();
        }
        else if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
        {
            if constexpr (std::is_same_v<T, FName>)
            {
                return NameProperty->GetPropertyValue(ValuePtr);
            }
            OutResult = false;
            return T();
        }
        else if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
        {
            if constexpr (std::is_same_v<T, FText>)
            {
                return TextProperty->GetPropertyValue(ValuePtr);
            }
            OutResult = false;
            return T();
        }
        else if(const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
        {
            if constexpr (std::is_same_v<T, void*>)
            {
                // For structs, we can return the raw pointer or specialize further
                return static_cast<T>(ValuePtr);
            }
            OutResult = false;
            return T();
        }
        else
        {
            OutResult = false;
            return T();
        }
    }

    template<typename T, typename V>
    static T GetPropertyValue(FProperty* Property, const V* object)
    {
        void* ValuePtr = Property->ContainerPtrToValuePtr<void>(object);
        T Result = TPropertyTypeFundamentals<T>::GetPropertyValue(ValuePtr);
        return Result;
    }
   
};

