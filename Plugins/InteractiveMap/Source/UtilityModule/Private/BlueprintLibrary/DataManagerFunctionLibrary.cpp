// Copyright 2024 An@stacioDev All rights reserved.

#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "HAL/PlatformFileManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/FileHelper.h"
#include "UtilityModule.h"
#include "StructUtils/Public/InstancedStruct.h"
void UDataManagerFunctionLibrary::WriteStringToFile(const FString& filePath, const FString& string, bool& outSuccess, FString& outInfoMessage)
{
	if (!FFileHelper::SaveStringToFile(string, *filePath))
	{
		outSuccess = false;
		outInfoMessage = outInfoMessage = FString::Printf(TEXT("Write string to file failed"));
		return;
	}

	outSuccess = true;
	outInfoMessage = outInfoMessage = FString::Printf(TEXT("Write string to file succeeded"));
}

TSharedPtr<FJsonObject> UDataManagerFunctionLibrary::ReadJsonFile(const FString& filePath)
{
	FString jsonString;
	if(!FFileHelper::LoadFileToString(jsonString, *filePath))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Error loading file '%s'"), *filePath);
		return nullptr;
	}
	TSharedPtr<FJsonObject> jsonObject;
	if(!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(jsonString), jsonObject))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Read Json Failed - error parsing file '%s'"), *filePath);
	}
	return jsonObject;
}

TArray<TSharedPtr<FJsonValue>> UDataManagerFunctionLibrary::ReadJsonFileArray(const FString& filePath)
{
	FString jsonString;
	if(!FFileHelper::LoadFileToString(jsonString, *filePath))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Error loading file '%s'"), *filePath);
		return TArray<TSharedPtr<FJsonValue>>();
	}
	TArray<TSharedPtr<FJsonValue>> jsonValueArray;
	if(!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(jsonString), jsonValueArray))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Read Json Failed - error parsing file '%s'"), *filePath);
	}
	return jsonValueArray;
}


TArray<FInstancedStruct> UDataManagerFunctionLibrary::LoadCustomDataFromJson(const FString& FilePath, const UScriptStruct* structType)
{
	TArray<TSharedPtr<FJsonValue>> JsonArray = ReadJsonFileArray(FilePath); 
	TArray<FInstancedStruct> OutArray;
	int32 index = 0;
	FInstancedStruct InstancedStruct;
	for(const auto& JsonValue : JsonArray)
	{
		TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
		if (!JsonObject.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid JSON object in array."));
			continue;
		}

		// Deserialize the object into an FInstancedStruct
		FInstancedStruct NewInstancedStruct;
		if (DeserializeJsonToFInstancedStruct(JsonObject, structType, NewInstancedStruct))
		{
			ObjectHasMissingFields(JsonObject, index, FilePath, structType);
			OutArray.Add(MoveTemp(NewInstancedStruct));
		}

		index++;
	}
	return OutArray;
}

void UDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(const FString& FilePath, const UScriptStruct* StructType,
	const TArray<FInstancedStruct>& Array)
{
	TArray<TSharedPtr<FJsonValue>> JsonValueArray;
	for (const auto& Value : Array)
	{
		TSharedPtr<FJsonObject> StructObject = SerializeInstancedStructToJson(Value, StructType);
		JsonValueArray.Add(MakeShared<FJsonValueObject>(StructObject));
	}

	bool bResult = false;
	FString OutInfoMessage;
	WriteJson(FilePath, JsonValueArray, bResult, OutInfoMessage);
	if(!bResult)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Failed to Save Instanced Struct Array: %s"), *OutInfoMessage);
	}
}

bool UDataManagerFunctionLibrary::DeserializeJsonToFInstancedStruct(const TSharedPtr<FJsonObject> JsonObject, const UScriptStruct* StructType, FInstancedStruct& OutInstancedStruct)
{
	// Initialize the FInstancedStruct with the resolved type
	OutInstancedStruct.InitializeAs(StructType);

	// Convert the JSON data into the struct instance
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructType, OutInstancedStruct.GetMutableMemory(), 0, 0))
	{
		return false;
	}
	return true;
}

TSharedPtr<FJsonObject> UDataManagerFunctionLibrary::SerializeInstancedStructToJson(const FInstancedStruct& Instance, const UScriptStruct* StructType)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	if(FJsonObjectConverter::UStructToJsonObject(StructType, Instance.GetMemory(), JsonObject.ToSharedRef(), 0, 0))
	{
		return JsonObject;
	}

	UE_LOG(LogUtilityModule, Error, TEXT("Failed to serialize instanced struct of type %s"), *StructType->GetName());
	return nullptr;
}

int32 UDataManagerFunctionLibrary::HexToDecimal(const FString& hex, const TMap<TCHAR, int32>& HexMap)
{
	FString temp = hex.Reverse();
	auto chars = temp.GetCharArray();
	temp = temp.ToUpper();
	int32 total = 0;
	for (int i = 0; i < temp.Len(); i++)
	{
		const int32* value = HexMap.Find(temp[i]);
		if (value)
		{
			total += FMath::Pow(16.f, i) * (*value);
		}
		else
		{
			UE_LOG(LogUtilityModule, Warning, TEXT("Invalid Hex value"));
		}
	}
	return total;
}

FColor UDataManagerFunctionLibrary::ConvertHexStringToRGB(const FString& Color)
{
	TMap<TCHAR, int32> HexMap;
	HexMap.Reserve(16);
	HexMap.Add('0', 0);
	HexMap.Add('1', 1);
	HexMap.Add('2', 2);
	HexMap.Add('3', 3);
	HexMap.Add('4', 4);
	HexMap.Add('5', 5);
	HexMap.Add('6', 6);
	HexMap.Add('7', 7);
	HexMap.Add('8', 8);
	HexMap.Add('9', 9);
	HexMap.Add('A', 10);
	HexMap.Add('B', 11);
	HexMap.Add('C', 12);
	HexMap.Add('D', 13);
	HexMap.Add('E', 14);
	HexMap.Add('F', 15);

	if (Color.StartsWith(FString("#")))
	{
		const FString noPrefix = Color.RightChop(1);
		const FString r = noPrefix.LeftChop(6);
		const FString g = noPrefix.Mid(2, 2);
		const FString b = noPrefix.Mid(4, 2);
		const FString alpha = noPrefix.RightChop(6);

		FColor ColorValue;
		ColorValue.R = HexToDecimal(r, HexMap);
		ColorValue.G = HexToDecimal(g, HexMap);
		ColorValue.B = HexToDecimal(b, HexMap);
		ColorValue.A = HexToDecimal(alpha, HexMap);

		return ColorValue;
	}

	UE_LOG(LogUtilityModule, Error, TEXT("Color is not hexadecimal format"));
	return FColor();
}

void UDataManagerFunctionLibrary::WriteJson(const FString& jsonFilePath, const TSharedPtr<FJsonObject> jsonObject, bool& outSuccess, FString& outInfoMessage)
{
	FString jsonString;

	if (!FJsonSerializer::Serialize(jsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&jsonString, 0)))
	{
		outSuccess = false;
		outInfoMessage = FString::Printf(TEXT("Write Json Failed - was not able to serialize the json string"));
		return;
	}

	WriteStringToFile(jsonFilePath, jsonString, outSuccess, outInfoMessage);
	if (!outSuccess)
	{
		return;
	}

	outSuccess = true;
	outInfoMessage = FString::Printf(TEXT("Write json succeeded = '%s"), *jsonFilePath);
}

void UDataManagerFunctionLibrary::WriteJson(const FString& jsonFilePath, TArray<TSharedPtr<FJsonValue>>& jsonValueArray, bool& outSuccess, FString& outInfoMessage)
{
	FString jsonString;

	if (!FJsonSerializer::Serialize(jsonValueArray, TJsonWriterFactory<>::Create(&jsonString, 0)))
	{
		outSuccess = false;
		outInfoMessage = FString::Printf(TEXT("Write Json Failed - was not able to serialize the json string"));
		return;
	}

	WriteStringToFile(jsonFilePath, jsonString, outSuccess, outInfoMessage);
	if (!outSuccess)
	{
		return;
	}

	outSuccess = true;
	outInfoMessage = FString::Printf(TEXT("Write json succeeded = '%s"), *jsonFilePath);
}

void UDataManagerFunctionLibrary::PopulateDataTableWithArray(UDataTable* DataTable, const TArray<FVariantData>& Provinces)
{
	if (!DataTable) return;

	for (int32 Index = 0; Index < Provinces.Num(); ++Index)
	{
		FString RowName = FString::Printf(TEXT("Province_%d"), Index);
		DataTable->AddRow(FName(*RowName), Provinces[Index]);
	}
}

// FInstanced Struct method is more versatile
TArray<FTestBasic> UDataManagerFunctionLibrary::LoadTestBasic(const FString& FilePath)
{
	return LoadCustomDataFromJson<FTestBasic>(FilePath);
}

TArray<FTestAdvanced> UDataManagerFunctionLibrary::LoadTestAdvanced(const FString& FilePath)
{
	return LoadCustomDataFromJson<FTestAdvanced>(FilePath);
}

void UDataManagerFunctionLibrary::ObjectHasMissingFields(const TSharedPtr<FJsonObject>& Object, int Index, const FString& FilePath, const UStruct* StructType)
{
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		const FProperty* Property = *It;

		// Get property name
		FString PropertyName = Property->GetName();

		// Check if the JSON object contains the field
		if (!Object->HasField(PropertyName))
		{
			UE_LOG(LogUtilityModule, Warning, 
				TEXT("Missing field '%s' in JSON object at index %d in file '%s'"), 
				*PropertyName, Index, *FilePath);
			return;
		}
	}
}

void UDataManagerFunctionLibrary::LogReadJsonFailed(const FString& FilePath)
{
	UE_LOG(LogUtilityModule, Error, TEXT("Read Json Failed - some entries do not match the structure defined '%s'"), *FilePath);
}
