// Copyright 2024 An@stacioDev All rights reserved.

#include "GenericWidgetDataMap.h"
#include "UObject/UnrealTypePrivate.h"

void UWidgetMapDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(!PropertyChangedEvent.Property)
		return;
	
	if (!ClassType)
		return;

	const FName PropertyName =  PropertyChangedEvent.Property->GetFName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UWidgetMapDataAsset, ClassType)
		|| (PropertyName == GET_MEMBER_NAME_CHECKED(UWidgetMapDataAsset, DefaultWidgetType)))
	{
		Reset();
	}
}

void UWidgetMapDataAsset::FillPropertyWidgetMap(const UClass* Class)
{
	if (!Class)
		return;
	PropertyWidgetMap.Empty();
	for (TFieldIterator<FProperty> It(Class); It; ++It)
	{
		const FProperty* Property = *It;
		if (!Property) continue;
		PropertyWidgetMap.Add(FName(Property->GetAuthoredName()), DefaultWidgetType);
	}
}

void UWidgetMapDataAsset::Reset()
{
	FillPropertyWidgetMap(ClassType);
	// Mark the asset as modified so it saves properly
	MarkPackageDirty();
}

void UWidgetMapDataAsset::CreateFromClass(UClass* Class, const TSubclassOf<UUserWidget>& WidgetType)
{
	if (!Class)
		return;
	
	ClassType = Class;
	this->DefaultWidgetType = WidgetType;
	Reset();
}

bool UWidgetMapDataAsset::IsValid() const
{
	return ClassType != nullptr;
}

const UClass* UWidgetMapDataAsset::GetDataClass() const
{
	return ClassType;
}
