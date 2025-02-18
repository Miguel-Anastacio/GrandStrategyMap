// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/MapDataComponent.h"
#include "Map/MapEnums.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "InteractiveMap.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"


TMap<FColor, int32> UMapDataComponent::GetLookUpTable() const
{
	return MapObjectRef->GetLookupTable();
}

TMap<int32, FInstancedStruct>* UMapDataComponent::GetProvinceDataMap() const
{
	return &MapObjectRef->GetMapData();
}

int UMapDataComponent::GetTileID(const FColor& Color, bool& bOutResult) const
{
	bOutResult = false;
	const int32* ID = MapObjectRef->FindIDPtr(Color);
	if(ID)
	{
		bOutResult = true;
		return *ID;
	}
	else
	{
		bOutResult = false;
		return -1;
	}
}

bool UMapDataComponent::GetTileData(int32 ID, FInstancedStruct& Out_Data) const
{
	if (const FInstancedStruct* Data = MapObjectRef->GetTileData(ID))
	{
		Out_Data = (*Data);
		return true;
	}
	return false;
}

FInstancedStruct* UMapDataComponent::GetTileData(int32 ID) const
{
	if (FInstancedStruct* Data = MapObjectRef->GetTileData(ID))
	{
		return Data;
	}
	return nullptr;
}
bool UMapDataComponent::SetTileData(const FInstancedStruct& NewData, int32 ID) const
{
	FInstancedStruct* CurrentData = GetTileData(ID);
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

void UMapDataComponent::SetMapObject(UMapObject* MapObject)
{
	MapObjectRef = MapObject;
}

int* UMapDataComponent::FindId(const FColor& Color) const
{
	return MapObjectRef->FindIDPtr(Color);
}

FColor UMapDataComponent::GetColor(int32 ID) const
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
