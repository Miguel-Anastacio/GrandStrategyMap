// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "BlueprintLibrary/PropertyUtilityFunctionLibrary.h"

inline bool UPropertyUtilityFunctionLibrary::IsPropertyOfType(const FProperty* Property, const UStruct* Class)
{
	const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property);
	if (!ObjectProperty)
	{
		return false; // Not an object property
	}
    
	// Then check if the property class is UTexture2D or a child of UTexture2D
	const UClass* PropertyClass = ObjectProperty->PropertyClass;
	return PropertyClass && (PropertyClass == Class || PropertyClass->IsChildOf(Class));
}

UTexture2D* UPropertyUtilityFunctionLibrary::GetTextureFromProperty(const UObject* Object,
	const FName& PropertyName)
{
	const FProperty* Property = Object->GetClass()->FindPropertyByName(PropertyName);
	return GetPropertyAs<UTexture2D>(Property, UTexture2D::StaticClass(), Object);
}
