// Copyright 2024 An@stacioDev All rights reserved.

#include "UserWidgets/GenericWidgetDataMap.h"

void UStructWidgetMapDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(!PropertyChangedEvent.Property)
		return;
	
	if (!StructType)
		return;

	const FName PropertyName =  PropertyChangedEvent.Property->GetFName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UStructWidgetMapDataAsset, StructType) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UStructWidgetMapDataAsset, DefaultWidgetType))
	{
		Reset();
	}
}

void UStructWidgetMapDataAsset::Reset()
{
	if (!StructType)
		return;
	PropertyWidgetMap.Empty();
	// Iterate over struct properties
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		const FProperty* Property = *It;
		if (!Property) continue;
		PropertyWidgetMap.Add(Property->GetFName(), DefaultWidgetType);
	}
	// Mark the asset as modified so it saves properly
	MarkPackageDirty();
}
