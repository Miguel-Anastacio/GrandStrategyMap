// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InstancedStruct.h"
#include "Serialization/CompactBinary.h"
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
struct TPropertyTraits<uint8>
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
    // Iterates through all properties of a struct, pass function to perform for each property
    template<typename FuncType>
    static void ForEachProperty(const UStruct* StructType, FuncType Func)
    {
        for (FProperty* Property = StructType->PropertyLink; Property != nullptr; Property = Property->PropertyLinkNext)
        {
            if (!Property)
            {
                continue;
            }
            Func(Property);
        }
    }
    // Iterates through all properties of a struct, pass function to perform for each property
    template<typename FuncType>
    static void ForEachProperty(const FInstancedStruct& InstancedStruct, FuncType Func)
    {
        const UStruct* Struct = InstancedStruct.GetScriptStruct();
        if(!Struct)
            return;
        ForEachProperty(Struct, Func);
    }
    
    static FString GetPropertyValueAsString(const FProperty* Property, const void* StructObject, bool& OutResult);
    /**
     * Get the value of a property of a InstancedStruct as a string
     * @param InstancedStruct 
     * @param PropertyName 
     * @param OutResult 
     * @return 
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category= "Instanced Struct Utils")
    static FString GetPropertyValueAsStringFromStruct(const FInstancedStruct& InstancedStruct, const FString& PropertyName, bool& OutResult);

    /// TODO - Add Specializations and expose them to Blueprints
    template<typename T>
    static T GetPropertyValueFromStruct(const FInstancedStruct& InstancedStruct, const FString& PropertyName, bool& OutResult)
    {
        OutResult = false;
        if(const FProperty* Property = FindPropertyByDisplayName(InstancedStruct.GetScriptStruct(), FName(*PropertyName)))
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
    static bool SetPropertyValueInStruct(FInstancedStruct& InstancedStruct, const FString& PropertyName, const T& NewValue)
    {
        if(const FProperty* Property = FindPropertyByDisplayName(InstancedStruct.GetScriptStruct(), FName(*PropertyName)))
        {
            return SetPropertyValue<T>(Property, InstancedStruct.GetMutableMemory(), NewValue);
        }
        
        return false;
    }
    UFUNCTION(BlueprintCallable, Category="Struct Utils")
    static FInstancedStruct SetPropertyValueInStruct(const FInstancedStruct& InstancedStruct, const FString& PropertyName, const FString& NewValue, bool& bResult);
    
    UFUNCTION(BlueprintCallable, Category="Struct Utils")
    static bool SetPropertyValueNestedInStructFromString(FInstancedStruct& InstancedStruct, const FString& PropertyName, const FString& NewValue);
    
    static FInstancedStruct GetStructFromProperty(const FProperty* Property,  const uint8* Object, bool& bOutResult);

    static FProperty* FindPropertyByDisplayName(const UStruct* Struct, const FName& DisplayName);
    static FProperty* FindPropertyByDisplayName(const TArray<const UStruct*>& Structs, const FName& DisplayName);
    
    static bool IsStructOfType(const FInstancedStruct& InstancedStruct, const TArray<UScriptStruct*>& StructTypes);

    static TArray<const FProperty*> GetOrderedProperties(const FInstancedStruct& InstancedStruct);
    static TArray<const FProperty*> GetOrderedProperties(const UScriptStruct* ScriptStruct);
    
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
    static bool SetPropertyValue(const FProperty* Property, V* Object, const T& NewValue)
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
    
    static FString GetPropertyValueAsString(const FProperty* Property, const void* Data);
    
    template<typename T>
    static FString GetPropertyValueAsString(const T* Data, FName PropertyName)
    {
        const FProperty* Property = FindPropertyByDisplayName(Data->GetClass(), PropertyName);
        return GetPropertyValueAsString(Property, Data);
    }

    UFUNCTION(BlueprintCallable, Category=StructUtils)
    static TArray<FInstancedStruct> CreateInstancedStructArray(int32 Size, const FInstancedStruct& Default);
    
private:
    struct FStructProp
    {
        const UScriptStruct* StructType;
        void* Object;
        const FProperty* Property;

        FStructProp(const UScriptStruct* structType, void* _Object, const FProperty* _Property) :
                        StructType(structType), Object(_Object), Property(_Property) {};
        FStructProp() : StructType(nullptr), Object(nullptr), Property(nullptr) {};

        bool IsValid() const
        {
            return StructType && Object;
        }
    };
    static FStructProp GetContainerThatHoldsProperty(const FString& PropertyName, void* StructMemory, const UScriptStruct* StructType);
    // Generalized compatibility check
    template<typename T>
    static bool IsTypeCompatible(const FProperty* Property)
    {
        if (!Property)
            return false;

        using ExpectedPropertyType = typename TPropertyTraits<T>::Type;
        if constexpr (std::is_same_v<ExpectedPropertyType, FStructProperty>)
        {
            if ( const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
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

