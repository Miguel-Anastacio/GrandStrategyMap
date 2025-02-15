// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/MapDataComponent.h"
#include "Map/MapEnums.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "InteractiveMap.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"

UMapDataComponent::UMapDataComponent()
{

}
#if WITH_EDITOR
void UMapDataComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

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

FInstancedStruct* UMapDataComponent::GetProvinceData(int ID)
{
	FInstancedStruct* data = ProvinceDataMap.Find(ID);
	if (data)
	{
		return data;
	}
	return nullptr;
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

bool UMapDataComponent::SetProvinceData(const FInstancedStruct& NewData, int ID)
{
	FInstancedStruct* CurrentData = GetProvinceData(ID);
	if(!CurrentData)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("ID does not exist in Map Data"))
		return false;
	}
	if(CurrentData->GetScriptStruct() != NewData.GetScriptStruct())
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("New struct type does not match current"))
		return false;
	}
	
	*CurrentData = NewData;
	return true;
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

FColor UMapDataComponent::GetColor(int ID) const
{
	for(const auto& [Color, Id] : GetLookUpTable())
    {
    	if(ID == Id)
    	{
    		return Color;
    	}
    }
	return FColor(0, 0, 0, 0);
}
