// Copyright 2024 An@stacioDev All rights reserved.

#pragma once
#include "VisualProperties.h"


FColor UVisualProperty::GetColorForProperty_Implementation(const FInstancedStruct& Data)
{
	return FColor::White;
}

UMaterialInterface* UVisualProperty::GetMaterial() const
{
	UMaterialInterface* Mat = MaterialInstance.Get();
	if (!Mat)
	{
		// need to load it first
		Mat = MaterialInstance.LoadSynchronous();
	}
	return Mat;
}

FColor UNumericVisualProperty::GetColorForProperty_Implementation(const FInstancedStruct& Data)
{
	bool bResult = false;
	const int32 Key = UAtkStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int32>(Data, Name, bResult);
	if (!bResult)
	{
		return DefaultColor;
	}

	// Clamp the value to the min/max range
	const int32 ClampedValue = FMath::Clamp(Key, MinValue, MaxValue);
        
	// Calculate normalized value (0 to 1)
	const float NormalizedValue = (MaxValue - MinValue) > 0 
		? static_cast<float>(ClampedValue - MinValue) / static_cast<float>(MaxValue - MinValue)
		: 0.5f;
        
	// Interpolate saturation based on normalized value
	const float TargetSaturation = FMath::Lerp(MinSaturation, MaxSaturation, NormalizedValue);
        
	// Convert color to HSV
	FLinearColor HSVColor = FLinearColor(PropertyColor).LinearRGBToHSV();
        
	// Apply new saturation (Y component is saturation in HSV)
	HSVColor.G = TargetSaturation;
        
	// Convert back to RGB
	const FLinearColor ResultColor = HSVColor.HSVToLinearRGB();
        
	return ResultColor.ToFColor(true);
}
