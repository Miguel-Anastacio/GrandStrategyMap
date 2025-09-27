// Copyright 2024 An@stacioDev All rights reserved.
#include "BlueprintLibrary/PropertyUtilityFunctionLibrary.h"

inline bool UAtkPropertyUtilityFunctionLibrary::IsPropertyOfType(const FProperty* Property, const UStruct* Class)
{
	const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property);
	if (!ObjectProperty)
	{
		return false; // Not an object property
	}
    
	const UClass* PropertyClass = ObjectProperty->PropertyClass;
	return PropertyClass && (PropertyClass == Class || PropertyClass->IsChildOf(Class));
}

UObject* UAtkPropertyUtilityFunctionLibrary::GetObjectFromProperty(const UObject* Container, const FName& PropertyName, TSubclassOf<UObject> ObjClass)
{
	if(!Container)
		return nullptr;
	
	const FProperty* Property = UAtkStructUtilsFunctionLibrary::FindPropertyByDisplayName(Container->GetClass(), PropertyName);
	return GetPropertyAs<UObject>(Property, ObjClass, Container);
}

UObject* UAtkPropertyUtilityFunctionLibrary::GetObjectFromPropertyStruct(const FInstancedStruct& InstancedStruct,
	const FName& PropertyName, TSubclassOf<class UObject> ObjClass)
{
	const FProperty* Property = UAtkStructUtilsFunctionLibrary::FindPropertyByDisplayName(InstancedStruct.GetScriptStruct(), PropertyName);
	return GetPropertyAs<UObject>(Property, ObjClass, InstancedStruct.GetMemory());
}

FInstancedStruct UAtkPropertyUtilityFunctionLibrary::GetStructFromPropertyInstancedStruct(const FInstancedStruct& InstancedStruct,
	const FName& PropertyName)
{
	if(!InstancedStruct.IsValid())
		return FInstancedStruct();
	
	const FProperty* Property = UAtkStructUtilsFunctionLibrary::FindPropertyByDisplayName(InstancedStruct.GetScriptStruct(), PropertyName);
	return GetStructFromProperty(Property, InstancedStruct.GetMemory());
}

FInstancedStruct UAtkPropertyUtilityFunctionLibrary::GetStructFromPropertyObj(const UObject* Object,
	const FName& PropertyName)
{
	if(!Object)
		return FInstancedStruct();
	
	const FProperty* Property = UAtkStructUtilsFunctionLibrary::FindPropertyByDisplayName(Object->GetClass(), PropertyName);
	return GetStructFromProperty(Property, Object);
}

FInstancedStruct UAtkPropertyUtilityFunctionLibrary::GetStructFromProperty(const FProperty* Property, const void* Object)
{
	if(!Property || !Object)
	{
		return FInstancedStruct();
	}
    
	if(const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		FInstancedStruct StructInstanced;
		const uint8* SourceData = StructProperty->ContainerPtrToValuePtr<uint8>(Object);
		StructInstanced.Reset();
		StructInstanced.InitializeAs(StructProperty->Struct, SourceData);
		return StructInstanced;
	}

	return FInstancedStruct();
}
