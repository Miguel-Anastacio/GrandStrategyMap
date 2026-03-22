#include "MapDataValidation.h"

#include "MapEditor.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"

bool MapDataValidation::ValidateMapData(const uint32 LookupTableSize, const TArray<FInstancedStruct>& MapData, const TArray<const UScriptStruct*>& StructTypes, const FString& FilePath)
{
	// Validate file entries number and IDs
	if(MapData.Num() != LookupTableSize)
	{
		// error map data does not match size of lookup table
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Entries in LookupTable %d, Entries in map file %d"), LookupTableSize,  MapData.Num());
		const FText Message = FText::FromString(FilePath + TEXT("has a different number of entries compared to lookup"));
		ErrorOnLoadMapDataFilePath(Message);
		return false;
	}
	
	// Validate that all entries structs have StructType or OceanStructType and IDs are unique and in order
	int32 expectedId = 0;
	for(const auto& InstancedStruct : MapData)
	{
		// Will never execute most likely, keep for debug and logging purposes anyway might be useful
		if(!MapDataValidation::IsStructTypeValid(StructTypes, InstancedStruct.GetScriptStruct()))
		{
			// error struct type not valid
			bool bResult = false;
			const int32 errorId = UAtkStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int32>(InstancedStruct, "ID", bResult);
			UE_LOG(LogInteractiveMapEditor, Error, TEXT("ID of struct: %d"), errorId);
			const FText Message = FText::FromString(TEXT("Data entry in file ") + FilePath + TEXT("is not of type StructType nor OceanStructType"));
			ErrorOnLoadMapDataFilePath(Message);
			return false;
		}

		// check IDs are in order
		bool bResult = false;
		const int32 id = UAtkStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int32>(InstancedStruct, "ID", bResult);
		if(!bResult || expectedId != id)
		{
			UE_LOG(LogInteractiveMapEditor, Error, TEXT("Expected ID %d got id %d"), expectedId, id);
			const FText Message = FText::FromString(TEXT("Data entries in file ") + FilePath + TEXT(" are not ordered by ID"));
			ErrorOnLoadMapDataFilePath(Message);
			return false;
		}
		expectedId++;
	}
	
	return true;
}

void MapDataValidation::ErrorOnLoadMapDataFilePath(const FText& Message)
{
	const FText Title = FText::FromString(TEXT("Error on load Map Data File"));
	FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
}

bool MapDataValidation::IsStructTypeValid(const TArray<const UScriptStruct*>& Structs, const UScriptStruct* StructType)
{
	for(const auto& Struct : Structs)
	{
		if(StructType == Struct)
		{
			return true;
		}
	}
	return false;	
}