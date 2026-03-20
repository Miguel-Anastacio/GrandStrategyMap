#pragma once

class MapDataValidation
{
public:	
	static bool ValidateMapData(const uint32 LookupTableSize, const TArray<struct FInstancedStruct>& MapData, const TArray<const UScriptStruct*>& StructTypes, const FString& FilePath);
	static void ErrorOnLoadMapDataFilePath(const FText& Message);
	static bool IsStructTypeValid(const TArray<const UScriptStruct*>& Structs, const UScriptStruct* StructType);
};
