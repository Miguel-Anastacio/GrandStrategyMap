// Copyright 2024 An@stacioDev All rights reserved.
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

UObject* UPropertyUtilityFunctionLibrary::GetObjectFromProperty(const UObject* Container, const FName& PropertyName, TSubclassOf<UObject> ObjClass)
{
	const FProperty* Property = UADStructUtilsFunctionLibrary::FindPropertyByDisplayName(Container->GetClass(), PropertyName);
	return GetPropertyAs<UObject>(Property, ObjClass, Container);
}

UObject* UPropertyUtilityFunctionLibrary::GetObjectFromPropertyStruct(const FInstancedStruct& InstancedStruct,
	const FName& PropertyName, TSubclassOf<class UObject> ObjClass)
{
	const FProperty* Property = UADStructUtilsFunctionLibrary::FindPropertyByDisplayName(InstancedStruct.GetScriptStruct(), PropertyName);
	return GetPropertyAs<UObject>(Property, ObjClass, InstancedStruct.GetMemory());
}

