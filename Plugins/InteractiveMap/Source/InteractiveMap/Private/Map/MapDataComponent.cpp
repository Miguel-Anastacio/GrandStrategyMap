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
	ReadDataTables();
}

const TMap<FVisualPropertyType, TArray<FVisualProperty>>& UMapDataComponent::GetVisualPropertiesMap() const
{
	return VisualPropertiesMap;
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

void UMapDataComponent::GetProvinceData(int ID, FInstancedStruct& Out_Data) const
{
	const FInstancedStruct* data = ProvinceDataMap.Find(ID);
	if (data)
	{
		Out_Data = (*data);
		Out_Data = (*data);
	}
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
	FCountryData* country = CountryDataMap.Find(id);
	if (country)
	{
		if (country->CountryName != data.CountryName)
		{
			(*country) = data;
			return true;
		}

		return false;
	}
	else
	{
		UE_LOG(LogInteractiveMap, Warning, TEXT("Invalid province id - update not possible"));
		return false;
	}
}
#if WITH_EDITOR
void UMapDataComponent::ReadDataTables()
{
	TArray<FVisualPropertyType*> AllTypes;
	if(UDataManagerFunctionLibrary::ReadDataTableToArray(VisualPropertyTypesDT, AllTypes))
	{
	}
	TArray<FVisualProperty*> VisualProperties;
	UDataManagerFunctionLibrary::ReadDataTableToArray(VisualPropertiesDT, VisualProperties);
	
	for(const auto& Type : AllTypes)
	{
		TArray<FVisualProperty> VisualPropertiesOfType;
		for(const auto& Property : VisualProperties)
		{
			if(Type->Type == Property->Type)
			{
				VisualPropertiesOfType.Emplace(*Property);
			}
		}
		VisualPropertiesMap.Emplace(*Type, VisualPropertiesOfType);
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

void UMapDataComponent::SetCountryProvinces()
{
	TArray<FName> countriesToIgnore;

	// see which countries already had province data from file
	for (auto& country : CountryDataMap)
	{
		country.Value.Provinces.Empty();
		if (country.Value.Provinces.Num() > 0)
			countriesToIgnore.Emplace(country.Key);
	}

	// if all countries then exit function
	if (countriesToIgnore.Num() == CountryDataMap.Num())
	{
		return;
	}

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
