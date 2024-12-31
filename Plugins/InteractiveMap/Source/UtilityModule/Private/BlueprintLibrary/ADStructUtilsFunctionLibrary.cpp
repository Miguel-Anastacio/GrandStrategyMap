// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "UtilityModule.h"

FString UADStructUtilsFunctionLibrary::GetPropertyValueAsString(FProperty* Property, const void* StructObject, bool& OutResult)
{
	OutResult = true;
	if (!Property || !StructObject)
	{
		OutResult = false;
		return TEXT("Invalid Property or Instance");
	}
	const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructObject);

	if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
	{
		return FString::FromInt(IntProperty->GetPropertyValue(ValuePtr));
	}
	else if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
	{
		return FString::SanitizeFloat(FloatProperty->GetPropertyValue(ValuePtr));
	}
	else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		return BoolProperty->GetPropertyValue(ValuePtr) ? TEXT("True") : TEXT("False");
	}
	else if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
	{
		return StrProperty->GetPropertyValue(ValuePtr);
	}
	else if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
	{
		return NameProperty->GetPropertyValue(ValuePtr).ToString();
	}
	else if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		return TextProperty->GetPropertyValue(ValuePtr).ToString();
	}
	else
	{
		OutResult = false;
		return TEXT("Invalid Property or Instance");
	}
}

