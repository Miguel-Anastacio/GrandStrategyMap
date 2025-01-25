// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/MapDataComponent.h"
#include "Map/MapEnums.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "InteractiveMap.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"

UMapDataComponent::UMapDataComponent()
{

}

void UMapDataComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	// ReadDataTables();
}

const TMap<FVisualPropertyType, FArrayOfVisualProperties>& UMapDataComponent::GetVisualPropertiesMap() const
{
	return VisualPropertiesMap;
}

TMap<FName, FArrayOfVisualProperties> UMapDataComponent::GetVisualPropertyNameMap() const
{
	TMap<FName, FArrayOfVisualProperties> VisualPropertiesNameMap;
	for(auto& VpType : VisualPropertiesMap)
	{
		VisualPropertiesNameMap.Emplace(VpType.Key.Type, VpType.Value);		
	}
	return VisualPropertiesNameMap;
}

TMap<FColor, int> UMapDataComponent::GetLookUpTable() const
{
	return NewLookupTable;
}

int UMapDataComponent::GetProvinceID(const FColor& Color, bool& bOutResult) const
{
	if (const int* ID = NewLookupTable.Find(Color))
	{
		bOutResult = true;
		return (*ID);
	}

	bOutResult = false;
	return -1;
}

bool UMapDataComponent::GetProvinceData(int ID, FInstancedStruct& Out_Data) const
{
	const FInstancedStruct* data = ProvinceDataMap.Find(ID);
	if (data)
	{
		Out_Data = (*data);
		return true;
	}
	return false;
}

FColor UMapDataComponent::GetPropertyColorFromInstancedStruct(const FInstancedStruct& InstancedStruct, const FName& PropertyName,
	bool& OutResult) const
{
	const FString PropertyValue = UADStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(InstancedStruct, PropertyName.ToString(), OutResult);

	if (!OutResult)
		return FColor::Black;
		
	return GetVisualProperty(PropertyName, FName(*PropertyValue), OutResult).Color;
}

// FInstancedStruct UMapDataComponent::GetProvinceData(int ID, bool& OutResult)
// {
// 	FInstancedStruct* Data = ProvinceDataMap.Find(ID);
// 	if (Data)
// 	{
// 		OutResult = true;
// 		return *Data;
// 	}
// 	OutResult = false;
// 	return FInstancedStruct();
// }

FInstancedStruct* UMapDataComponent::GetProvinceData(int ID)
{
	FInstancedStruct* data = ProvinceDataMap.Find(ID);
	if (data)
	{
		return data;
	}
	return nullptr;
}

bool UMapDataComponent::UpdateProvinceData(const FProvinceData& data, int id, MapMode& out_mapToUpdate, FColor& out_newColor)
{
	FInstancedStruct* province = ProvinceDataMap.Find(id);
	if (!province)
	{
		UE_LOG(LogInteractiveMap, Warning, TEXT("Invalid province id - update not possible"));
		return false;
	}
	return false;
}

bool UMapDataComponent::UpdateCountryData(const FCountryData& data, FName id)
{
	// FCountryData* country = CountryDataMap.Find(id);
	// if (country)
	// {
	// 	if (country->CountryName != data.CountryName)
	// 	{
	// 		(*country) = data;
	// 		return true;
	// 	}
	//
	// 	return false;
	// }
	// else
	// {
	// 	UE_LOG(LogInteractiveMap, Warning, TEXT("Invalid province id - update not possible"));
	// 	return false;
	// }
	return false;
}
#if WITH_EDITOR
void UMapDataComponent::ReadDataTables(const UDataTable* VpDataTable, const UDataTable* VpTypeDataTable)
{
	if(!VpDataTable|| !VpTypeDataTable)
	{
		UE_LOG(LogInteractiveMap, Warning, TEXT("ReadDataTables() failed - null datatables"));
		return;
	}
	
	VisualPropertiesMap.Empty();	
	TArray<FVisualPropertyType*> AllTypes;
	if(UDataManagerFunctionLibrary::ReadDataTableToArray(VpTypeDataTable, AllTypes))
	{
	}
	TArray<FVisualProperty*> VisualProperties;
	UDataManagerFunctionLibrary::ReadDataTableToArray(VpDataTable, VisualProperties);
	
	for(const auto& Type : AllTypes)
	{
		FArrayOfVisualProperties ArrayOf;
		for(const auto& Property : VisualProperties)
		{
			if(Type->Type == Property->Type)
			{
				ArrayOf.VisualProperties.Emplace(*Property);
			}
		}
		VisualPropertiesMap.Emplace(*Type, ArrayOf.VisualProperties);
	}
}
#endif

void UMapDataComponent::SetProvinceDataMap(const TArray<FInstancedStruct>& Data)
{
	ProvinceDataMap.Empty();
	ProvinceDataMap.Reserve(Data.Num());
	for(const auto& StructInstanced : Data)
	{
		bool bResult = false;
		int ID = UADStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int>(StructInstanced, "ID", bResult);
		if(bResult)
		{
			ProvinceDataMap.Emplace(ID, StructInstanced);
		}
	}
}

void UMapDataComponent::SetProvinceData(const FInstancedStruct& NewData, int ID)
{
	if(FInstancedStruct* Current = ProvinceDataMap.Find(ID))
	{
		*Current = NewData;
	}
}

void UMapDataComponent::SetCountryProvinces()
{
	TArray<FName> countriesToIgnore;

	// see which countries already had province data from file
	for (auto& province : ProvinceDataMap)
	{
		// FCountryData* country = CountryDataMap.Find(province.Value.Owner);
		// if (!country)
		// {
		// 	UE_LOG(LogInteractiveMap, Error, TEXT("Map Data Component: Province has an invalid Owner"));
		// 	continue;
		// }
		//
		// if (countriesToIgnore.Contains(province.Value.Owner))
		// 	continue;

		// country->Provinces.Add(province.Key);
	}
}

void UMapDataComponent::LoadFromMapObject(const UMapObject* MapObject)
{
	NewLookupTable = MapObject->GetLookupTable();
	SetProvinceDataMap(MapObject->GetMapDataValue());
	// TODO: Load Visual Properties and Countries
}

int* UMapDataComponent::FindId(const FColor& Color)
{
	return NewLookupTable.Find(Color);
}

FVisualProperty UMapDataComponent::GetVisualProperty(const FName& Type, const FName& Tag, bool& OutResult) const
{
	OutResult = false;
	const FArrayOfVisualProperties* PropertiesOfType = VisualPropertiesMap.Find(FVisualPropertyType(Type));
	if(!PropertiesOfType)
	{
		return FVisualProperty();
	}
	
	for(const FVisualProperty& VisualProperty : PropertiesOfType->VisualProperties)
	{
		if(VisualProperty.Tag == Tag)
		{
			OutResult = true;
			return VisualProperty;
		}
	}

	return FVisualProperty();
}

FVisualProperty UMapDataComponent::GetVisualProperty(const FVisualPropertyType& Type, const FName& Tag, bool& OutResult) const
{
	OutResult = false;
	const FArrayOfVisualProperties* PropertiesOfType = VisualPropertiesMap.Find(Type);
	if(!PropertiesOfType)
	{
		return FVisualProperty();
	}
	
	for(const FVisualProperty& VisualProperty : PropertiesOfType->VisualProperties)
	{
		if(VisualProperty.Tag == Tag)
		{
			OutResult = true;
			return VisualProperty;
		}
	}

	return FVisualProperty();
}
