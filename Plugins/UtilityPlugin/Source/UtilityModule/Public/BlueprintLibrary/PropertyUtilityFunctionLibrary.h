// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PropertyUtilityFunctionLibrary.generated.h"

UCLASS()
class UTILITYMODULE_API UPropertyUtilityFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, BlueprintPure, Category=PropertyUtility)
    static UTexture2D* GetTextureFromProperty(const UObject* Object, const FName& PropertyName);
    
    static bool IsPropertyOfType(const FProperty* Property, const UStruct* Class);

    template <typename T>
    static T* GetPropertyAs(const FProperty* Property, const UStruct* Class, const void* ContainerPtr)
    {
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
        return Cast<UTexture2D>(ObjectValue);
    }
};


