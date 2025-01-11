// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/MapDataComponent.h"
#include "Map/MapEnums.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "InteractiveMap.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"

UMapDataComponent::UMapDataComponent()
{

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

void UMapDataComponent::GetProvinceData(int ID, FInstancedStruct& out_data) const
{
	const FInstancedStruct* data = ProvinceDataMap.Find(ID);
	if (data)
	{
		out_data = (*data);
		out_data = (*data);
	}
}

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
	// if (province->Owner != data.Owner)
	// {
	// 	FCountryData* newOwnerCountry = CountryDataMap.Find(data.Owner);
	// 	if (!newOwnerCountry)
	// 	{
	// 		UE_LOG(LogInteractiveMap, Warning, TEXT("Invalid owner - update not possible"));
	// 		return false;
	// 	}
	//
	// 	out_newColor = newOwnerCountry->Color;
	// 	out_mapToUpdate = MapMode::POLITICAL;
	//
	// 	newOwnerCountry->Provinces.Add(id);
	// 	FCountryData* currentOwner = CountryDataMap.Find(province->Owner);
	// 	if(currentOwner)
	// 		currentOwner->Provinces.RemoveSingle(id);
	//
	// 	(*province) = data;
	//
	// 	return true;
	// }
	// else if (province->Religion != data.Religion)
	// {
	// 	out_newColor = GetReligionColor(&data);
	// 	if (out_newColor == FColor(0, 0, 0, 0))
	// 	{
	// 		UE_LOG(LogInteractiveMap, Warning, TEXT("Invalid religion - update not possible"));
	// 		return false;
	// 	}
	//
	// 	(*province) = data;
	// 	out_mapToUpdate = MapMode::RELIGIOUS;
	//
	// 	return true;
	// }
	// else if (province->Culture != data.Culture)
	// {
	// 	out_newColor = GetCultureColor(&data);
	// 	if (out_newColor == FColor(0, 0, 0, 0))
	// 	{
	// 		UE_LOG(LogInteractiveMap, Warning, TEXT("Invalid culture - update not possible"));
	// 		return false;
	// 	}
	//
	// 	(*province) = data;
	// 	out_mapToUpdate = MapMode::CULTURAL;
	// 	return true;
	// }
	// else
	// {
	// 	(*province) = data;
	// 	return true;
	// }

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


void UMapDataComponent::CreateLookUpTable()
{
	if (!MapDataTable)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Map Data Table not set. Make sure it is assigned in the Interactive Map Data Component"));
		return;
	}

	TArray<FName> RowNames = MapDataTable->GetRowNames();
	TMap<TCHAR, int32> HexMap;
	InitHexMap(HexMap);
	for (auto& name : RowNames)
	{
		// Color in HEX as a string
		FProvinceIDData* Item = MapDataTable->FindRow<FProvinceIDData>(name, "");
		if (Item)
		{
			FString temp = Item->Color;
			temp = temp.EndsWith(FString("FF")) ? temp : temp.Append(TEXT("FF"));

			FColor sRGBColor = Item->ConvertHexStringToRGB(temp, HexMap);

			LookUpTable.Add(FVector(sRGBColor.R, sRGBColor.G, sRGBColor.B), name);
			continue;
		}

		// Color in FColor format
		FProvinceIDDataRGB* itemRGB = MapDataTable->FindRow<FProvinceIDDataRGB>(name, "");
		if (itemRGB)
		{
			LookUpTable.Add(FVector(itemRGB->Color.R, itemRGB->Color.G, itemRGB->Color.B), name);
		}
	}
}

void UMapDataComponent::ReadDataTables()
{
	// if (!UDataManagerFunctionLibrary::ReadDataTable(ProvinceDataTable, ProvinceDataMap))
		// UE_LOG(LogInteractiveMap, Error, TEXT("Province Data Table not set. Make sure it is assigned in the Interactive Map Data Component"));
	if(!UDataManagerFunctionLibrary::ReadDataTable(CountryDataTable, CountryDataMap))
		UE_LOG(LogInteractiveMap, Error, TEXT("Country Data Table not set. Make sure it is assigned in the Interactive Map Data Component"));
	
	if(!UDataManagerFunctionLibrary::ReadDataTable(VisualPropertiesDataTable, VisualPropertiesDataMap))
		UE_LOG(LogInteractiveMap, Error, TEXT("Visual Properties Data Table not set. Make sure it is assigned in the Interactive Map Data Component"));
}

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

bool UMapDataComponent::GetCountryColor(const FColor& Color, FColor& out_countryColor) const
{
	const int* id = NewLookupTable.Find(Color);
	if (id)
	{
		const FInstancedStruct* province = ProvinceDataMap.Find(*id);
		if (!province)
			return false;
		// const FCountryData* country = CountryDataMap.Find(province->Owner);
		// if (!country)
		// 	return false;
		//
		// out_countryColor = country->Color;
		return true;
	}
	else
	{
		return false;
	}
}

FColor UMapDataComponent::GetCountryColor(const FProvinceData* data) const
{
	FColor color = FColor(0, 0, 0, 0);
	if (data)
	{
		const FCountryData* country = CountryDataMap.Find(data->Owner);
		if (!country)
			return color;

		return country->Color;
	}
	return color;
}

FColor UMapDataComponent::GetReligionColor(const FProvinceData* data) const
{
	FColor color = FColor(0, 0, 0, 0);

	if (data)
	{
		const FColoredData* religion = VisualPropertiesDataMap.Find(data->Religion);
		if (!religion)
		{
			return FColor(0, 0, 0, 0);
		}

		if (religion->Type != FName("Religion"))
			return FColor(0, 0, 0, 0);


		return religion->Color;
	}
	return FColor(0, 0, 0, 0);
}

FColor UMapDataComponent::GetCultureColor(const FProvinceData* data) const
{
	FColor color = FColor(0, 0, 0, 0);
	if (data)
	{
		const FColoredData* culture = VisualPropertiesDataMap.Find(data->Culture);
		if (!culture)
		{
			return FColor(0, 0, 0, 0);
		}

		if (culture->Type != FName("Culture"))
			return FColor(0, 0, 0, 0);

		return culture->Color;
	}
	return FColor(0, 0, 0, 0);
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
