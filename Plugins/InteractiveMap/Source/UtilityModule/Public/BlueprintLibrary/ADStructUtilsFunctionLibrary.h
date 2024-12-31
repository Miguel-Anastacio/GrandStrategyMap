// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InstancedStruct.h"
#include "ADStructUtilsFunctionLibrary.generated.h"

template<typename T, typename Enable = void>
struct TPropertyTraits
{
    using Type = void; // Default to void for unsupported types
};

template<typename  T>
struct TPropertyTraits<T, std::enable_if_t<std::is_integral_v<T>>>
{
    using Type = FIntProperty;
};

template<>
struct TPropertyTraits<uint32>
{
    using Type = FIntProperty;
};

template<>
struct TPropertyTraits<float>
{
    using Type = FFloatProperty;
};

template<>
struct TPropertyTraits<FString>
{
    using Type = FStrProperty;
};

template<>
struct TPropertyTraits<bool>
{
    using Type = FBoolProperty;
};

template<typename T>
struct TIsDerivedFromStruct
{
    static constexpr bool Value = TIsDerivedFrom<T, UStruct>::Value;
};

template<>
struct TPropertyTraits<UScriptStruct>
{
    using Type = FStructProperty;
};

UCLASS()
class UTILITYMODULE_API UADStructUtilsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    static FString GetPropertyValueAsString(FProperty* Property, const void* StructObject, bool& OutResult);
    /// TODO - Add Specializations and expose them to Blueprints
    
    template<typename T>
    static T GetPropertyValueFromStruct(const FInstancedStruct& InstancedStruct, const FString& PropertyName, bool& OutResult)
    {
        OutResult = false;
        if(const FProperty* Property = InstancedStruct.GetScriptStruct()->FindPropertyByName(FName(*PropertyName)))
        {
            T PropertyValue = GetPropertyValue<T>(Property, InstancedStruct.GetMemory(), OutResult);
            if(OutResult)
            {
                return PropertyValue;
            }
        }
        return T();
    }

    template<typename T>
    static bool StructHasPropertyWithTypeCompatible(const UScriptStruct* StructType, FName MustHavePropertyName)
    {
        if(const FProperty* Property = StructType->FindPropertyByName(MustHavePropertyName))
        {
            return IsTypeCompatible<T>(Property);
        }
        return false;
    }

    static void ProcessAllPropertiesOfStruct(const FInstancedStruct& StructInstance);
    
    template<typename T>
    static bool SetPropertyValueInStruct(FInstancedStruct& InstancedStruct, const FString& PropertyName, const T& NewValue)
    {
        if(FProperty* Property = InstancedStruct.GetScriptStruct()->FindPropertyByName(FName(*PropertyName)))
        {
            return SetPropertyValue<T>(Property, InstancedStruct.GetMutableMemory(), NewValue);
        }
        
        return false;
    }
    
    template<typename T, typename V>
    static T GetPropertyValue(const FProperty* Property, const V* Object, bool& bOutResult)
    {
        if(!Object)
        {
            bOutResult = false;
            return T();
        }
        
        if(!Property)
        {
            bOutResult = false;
            return T();
        }
        
        if(!IsTypeCompatible<T>(Property))
        {
            bOutResult = false;
            return T();
        }
        
        bOutResult = true;
        const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);
        T Result = TPropertyTypeFundamentals<T>::GetPropertyValue(ValuePtr);
        return Result;
    }

    template<typename T, typename  V>
    static bool SetPropertyValue(FProperty* Property, V* Object, const T& NewValue)
    {
        if(!Object)
        {
            return false;
        }
        if(!Property)
        {
            return false;
        }
        if(!IsTypeCompatible<T>(Property))
        {
            return false;
        }
        
        void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);
        TPropertyTypeFundamentals<T>::SetPropertyValue(ValuePtr, NewValue);
        return true;
    }
    
private:
    // Generalized compatibility check
    template<typename T>
    static bool IsTypeCompatible(const FProperty* Property)
    {
        if (!Property)
            return false;

        using ExpectedPropertyType = typename TPropertyTraits<T>::Type;
        if constexpr (std::is_same_v<ExpectedPropertyType, FStructProperty>)
        {
            const FStructProperty* StructProperty = CastField<FStructProperty>(Property);
            if (StructProperty)
            {
                // Ensure the struct type matches
                return StructProperty->Struct->GetFName() == T::StaticStruct()->GetFName();
            }
            return false;
        }
        else if constexpr (!std::is_same_v<ExpectedPropertyType, void>)
        {
            // For other property types (e.g., FIntProperty, FFloatProperty)
            return CastField<ExpectedPropertyType>(Property) != nullptr;
        }
        else
        {
            return false;
        }
    }
};

