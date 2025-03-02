// Copyright 2024 An@stacioDev All rights reserved.

#include "GenericWidgetDataMap.h"

void UWidgetMapDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(!PropertyChangedEvent.Property)
		return;
	
	if (!StructType)
		return;

	const FName PropertyName =  PropertyChangedEvent.Property->GetFName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UWidgetMapDataAsset, StructType) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UWidgetMapDataAsset, DefaultWidgetType))
	{
		Reset();
	}
}

void UWidgetMapDataAsset::Reset()
{
	if (!StructType)
		return;
	PropertyWidgetMap.Empty();
	// Iterate over struct properties
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		const FProperty* Property = *It;
		if (!Property) continue;
		PropertyWidgetMap.Add(FName(Property->GetAuthoredName()), DefaultWidgetType);
	}
	// Mark the asset as modified so it saves properly
	MarkPackageDirty();
}
