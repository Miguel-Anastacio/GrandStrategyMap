// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "UtilityModule.h"
#include "UObject/Field.h"
#include "UObject/TextProperty.h"
FString UADStructUtilsFunctionLibrary::GetPropertyValueAsString(const FProperty* Property, const void* StructObject, bool& OutResult)
{
	OutResult = true;
	if (!Property || !StructObject)
	{
		OutResult = false;
		return TEXT("Invalid Property or Instance");
	}
	const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructObject);

	if (const FIntProperty* IntProperty = CastField<FIntProperty>(Property))
	{
		return FString::FromInt(IntProperty->GetPropertyValue(ValuePtr));
	}
	else if (const FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
	{
		return FString::SanitizeFloat(FloatProperty->GetPropertyValue(ValuePtr));
	}
	else if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		return BoolProperty->GetPropertyValue(ValuePtr) ? TEXT("True") : TEXT("False");
	}
	else if (const FStrProperty* StrProperty = CastField<FStrProperty>(Property))
	{
		return StrProperty->GetPropertyValue(ValuePtr);
	}
	else if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
	{
		return NameProperty->GetPropertyValue(ValuePtr).ToString();
	}
	else if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		return TextProperty->GetPropertyValue(ValuePtr).ToString();
	}
	else
	{
		OutResult = false;
		return TEXT("Invalid Property or Instance");
	}
}

FString UADStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(const FInstancedStruct& InstancedStruct,
	const FString& PropertyName, bool& OutResult)
{
	OutResult = false;
	if(!InstancedStruct.IsValid())
	{
		return FString("Invalid Property or Instance");
	}
	
	if(const FProperty* Property = FindPropertyByDisplayName(InstancedStruct.GetScriptStruct(), FName(*PropertyName)))
	{
		return GetPropertyValueAsString(Property, InstancedStruct.GetMemory(), OutResult);
	}
	return FString("Invalid Property or Instance");
}


FInstancedStruct UADStructUtilsFunctionLibrary::SetPropertyValueInStruct(const FInstancedStruct& InstancedStruct,
	const FString& PropertyName, const FString& NewValue, bool& bResult)
{
	FInstancedStruct InstancedStructCopy;
	InstancedStructCopy.InitializeAs(InstancedStruct.GetScriptStruct(), InstancedStruct.GetMemory());
	const FProperty* Property = FindPropertyByDisplayName(InstancedStruct.GetScriptStruct(),  FName(*PropertyName));
	bResult = SetPropertyValue(Property, InstancedStructCopy.GetMutableMemory(), NewValue);
	return InstancedStructCopy;
}

bool UADStructUtilsFunctionLibrary::SetPropertyValueNestedInStructFromString(FInstancedStruct& InstancedStruct,
                                                                             const FString& PropertyName, const FString& NewValue)
{
	const FStructProp Container = GetContainerThatHoldsProperty(PropertyName, InstancedStruct.GetMutableMemory(), InstancedStruct.GetScriptStruct());
	if(Container.IsValid())
	{
		if(SetPropertyValue<FString>(Container.Property, Container.Object, NewValue))
			return true;
		if(SetPropertyValue<int32>(Container.Property, Container.Object, FCString::Atoi(*NewValue)))
			return true;
		if(SetPropertyValue<float>(Container.Property, Container.Object, FCString::Atof(*NewValue)))
			return true;
	}

	return false;
}

FInstancedStruct UADStructUtilsFunctionLibrary::GetStructFromProperty(const FProperty* Property, const uint8* Object,
                                                                      bool& bOutResult)
{
	bOutResult = false;
	if(!Property)
	{
		return FInstancedStruct();
	}
    
	if (!Object)
	{
		return FInstancedStruct();
	}
    
	if(const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		FInstancedStruct StructInstanced;
		const uint8* SourceData = StructProperty->ContainerPtrToValuePtr<uint8>(Object);
		StructInstanced.Reset();
		StructInstanced.InitializeAs(StructProperty->Struct, SourceData);
		bOutResult = true;
		return StructInstanced;
	}

	return FInstancedStruct();
}

FProperty* UADStructUtilsFunctionLibrary::FindPropertyByDisplayName(const UScriptStruct* Struct,const FName& DisplayName )
{
	if(!Struct)
		return nullptr;
	
	for (FProperty* DisplayProperty = Struct->PropertyLink; DisplayProperty != nullptr; DisplayProperty = DisplayProperty->PropertyLinkNext)
	{
		const FString StaticPropertyName = DisplayProperty->GetName();
		if (StaticPropertyName.Contains(DisplayName.ToString()))
		{
			return DisplayProperty;
		}
	}
	return nullptr;
}

FProperty* UADStructUtilsFunctionLibrary::FindPropertyByDisplayName(TArray<const UScriptStruct*> Structs,
	const FName& DisplayName)
{
	for(const UScriptStruct* Struct : Structs)
	{
		if(FProperty* Property = FindPropertyByDisplayName(Struct, DisplayName))
		{
			return Property;
		}
	}
	return nullptr;
}

bool UADStructUtilsFunctionLibrary::IsStructOfType(const FInstancedStruct& InstancedStruct,
                                                   const TArray<UScriptStruct*>& StructTypes)
{
	for(const auto& Type : StructTypes)
	{
		if(InstancedStruct.GetScriptStruct() == Type)
			return true;
	}
	return false;
}

TArray<const FProperty*> UADStructUtilsFunctionLibrary::GetOrderedProperties(const FInstancedStruct& InstancedStruct)
{
	return GetOrderedProperties(InstancedStruct.GetScriptStruct());
}

TArray<const FProperty*> UADStructUtilsFunctionLibrary::GetOrderedProperties(const UScriptStruct* ScriptStruct)
{
	TArray<const FProperty*> ParentProperties;
	TArray<const FProperty*> ChildProperties;
	if(!ScriptStruct)
		return ParentProperties;
	// Iterate through the properties in the struct
	for (TFieldIterator<FProperty> It(ScriptStruct); It; ++It)
	{
		const FProperty* Property = *It;
		// Check if the property is from the base struct (parent) or derived struct (child)
		if (Property->GetOwnerStruct()->GetInheritanceSuper())
		{
			// This is a child property
			ChildProperties.Add(Property);
		}
		else
		{
			// This is a parent property
			ParentProperties.Add(Property);
		}
	}

	TArray<const FProperty*> OrderedProperties;
	OrderedProperties.Append(ParentProperties);
	OrderedProperties.Append(ChildProperties);
	return OrderedProperties;
}

UADStructUtilsFunctionLibrary::FStructProp UADStructUtilsFunctionLibrary::GetContainerThatHoldsProperty(const FString& PropertyName, void* StructMemory,
                                                                                                        const UScriptStruct* StructType)
{
	if(!StructType)
	{
		return FStructProp();
	}
	
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		const FProperty* Property = *It;
		if (!Property)
		{
			continue;
		}
		
		if(PropertyName == Property->GetAuthoredName())
		{
			return FStructProp(StructType, StructMemory, Property);
		}
		
		if(const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			void* SourceData = StructProperty->ContainerPtrToValuePtr<void>(StructMemory);
			return GetContainerThatHoldsProperty(PropertyName, SourceData, StructProperty->Struct);
		}
	}

	return FStructProp();
}



