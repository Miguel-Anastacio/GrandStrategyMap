// Copyright 2024 An@stacioDev All rights reserved.
#include "GenericWidget/GenericWidgetDataMap.h"

void UPropGenWidgetMapDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(!PropertyChangedEvent.Property)
		return;
	
	const FName PropertyName =  PropertyChangedEvent.Property->GetFName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UPropGenWidgetMapDataAsset, Class)
		|| (PropertyName == GET_MEMBER_NAME_CHECKED(UPropGenWidgetMapDataAsset, DefaultWidgetType))
		|| (PropertyName == GET_MEMBER_NAME_CHECKED(UPropGenWidgetMapDataAsset, Struct)))
	{
		SetClass();
		Reset();
	}
}

TArray<FString> UPropGenWidgetMapDataAsset::GetPropertyNameOptions() const
{
	TArray<FString> PropertyNames;
	for (TFieldIterator<FProperty> It(ClassType); It; ++It)
	{
		const FProperty* Property = *It;
		if (!Property) continue;
		PropertyNames.Add(Property->GetAuthoredName());
	}
	return PropertyNames;
}

void UPropGenWidgetMapDataAsset::FillPropertyWidgetMap(const UStruct* BaseClass)
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

void UPropGenWidgetMapDataAsset::SetClass()
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

void UPropGenWidgetMapDataAsset::Reset()
{
	FillPropertyWidgetMap(ClassType);
	// Mark the asset as modified so it saves properly
	MarkPackageDirty();
}

void UPropGenWidgetMapDataAsset::CreateFromData(UStruct* BaseStruct, const TSubclassOf<UUserWidget>& WidgetType)
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

bool UPropGenWidgetMapDataAsset::IsValid() const
{
	return ClassType != nullptr;
}

const UStruct* UPropGenWidgetMapDataAsset::GetDataClass() const
{
	return ClassType;
}

const UScriptStruct* UPropGenWidgetMapDataAsset::GetClassAsScriptStruct() const
{
	if(bUObjectBased)
		return nullptr;
	return Cast<UScriptStruct>(ClassType);
}

const UClass* UPropGenWidgetMapDataAsset::GetClassAsUClass() const
{
	if(!bUObjectBased)
		return nullptr;
	return Cast<UClass>(ClassType);
}
