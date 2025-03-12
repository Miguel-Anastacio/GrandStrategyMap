// Copyright 2024 An@stacioDev All rights reserved.
#include "GenericWidgetDataMap.h"

void UWidgetMapDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(!PropertyChangedEvent.Property)
		return;
	
	// if (!ClassType)
	// 	return;

	const FName PropertyName =  PropertyChangedEvent.Property->GetFName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UWidgetMapDataAsset, Class)
		|| (PropertyName == GET_MEMBER_NAME_CHECKED(UWidgetMapDataAsset, DefaultWidgetType))
		|| (PropertyName == GET_MEMBER_NAME_CHECKED(UWidgetMapDataAsset, Struct)))
	{
		SetClass();
		Reset();
	}
}

void UWidgetMapDataAsset::FillPropertyWidgetMap(const UStruct* BaseClass)
{
	if (!BaseClass)
		return;
	PropertyWidgetMap.Empty();
	for (TFieldIterator<FProperty> It(BaseClass); It; ++It)
	{
		const FProperty* Property = *It;
		if (!Property) continue;
		PropertyWidgetMap.Add(FName(Property->GetAuthoredName()), DefaultWidgetType);
	}
}

void UWidgetMapDataAsset::SetClass()
{
	if(bUObjectBased)
	{
		Struct = nullptr;
		ClassType = Class;
	}
	else if(Struct)
	{
		Class = nullptr;
		ClassType = Struct;
	}
}

// bool UWidgetMapDataAsset::IsScriptStruct(const UStruct* Struct) const
// {
// 	return false;
// }

void UWidgetMapDataAsset::Reset()
{
	FillPropertyWidgetMap(ClassType);
	// Mark the asset as modified so it saves properly
	MarkPackageDirty();
}

void UWidgetMapDataAsset::CreateFromData(UStruct* BaseStruct, const TSubclassOf<UUserWidget>& WidgetType)
{
	if (!BaseStruct)
		return;

	bUObjectBased = BaseStruct->IsA(UClass::StaticClass());
	ClassType = BaseStruct;
	
	if(bUObjectBased)
	{
		Struct = nullptr;
		Class = Cast<UClass>(BaseStruct);
	}
	else
	{
		Class = nullptr;
		Struct = Cast<UScriptStruct>(BaseStruct);
	}
	
	this->DefaultWidgetType = WidgetType;
	Reset();
}

bool UWidgetMapDataAsset::IsValid() const
{
	return ClassType != nullptr;
}

const UStruct* UWidgetMapDataAsset::GetDataClass() const
{
	return ClassType;
}

const UScriptStruct* UWidgetMapDataAsset::GetClassAsScriptStruct() const
{
	if(bUObjectBased)
		return nullptr;
	return Cast<UScriptStruct>(ClassType);
}

const UClass* UWidgetMapDataAsset::GetClassAsUClass() const
{
	if(!bUObjectBased)
		return nullptr;
	return Cast<UClass>(ClassType);
}
