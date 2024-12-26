// Copyright 2024 An@stacioDev All rights reserved.

#include "FileIO/DataManagerFunctionLibrary.h"
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


TArray<FInstancedStruct> UDataManagerFunctionLibrary::LoadCustomDataFromJson(const FString& FilePath, UScriptStruct* structType)
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
			ObjectHasMissingFiels(JsonObject, index, FilePath, structType);
			OutArray.Add(MoveTemp(NewInstancedStruct));
		}

		index++;
	}
	return OutArray;
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

void* UDataManagerFunctionLibrary::CreateStructInstance(const UStruct* StructType)
{
	if (!StructType)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid StructType"));
		return nullptr;
	}

	// Allocate memory for the struct instance
	void* StructMemory = FMemory::Malloc(StructType->GetStructureSize());
	if (!StructMemory)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to allocate memory for struct"));
		return nullptr;
	}

	// Initialize the struct memory
	StructType->InitializeStruct(StructMemory);

	return StructMemory;
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

TArray<FTestBasic> UDataManagerFunctionLibrary::LoadTestBasic(const FString& FilePath)
{
	return LoadCustomDataFromJson<FTestBasic>(FilePath);
}

TArray<FTestAdvanced> UDataManagerFunctionLibrary::LoadTestAdvanced(const FString& FilePath)
{
	return LoadCustomDataFromJson<FTestAdvanced>(FilePath);
}

void UDataManagerFunctionLibrary::ObjectHasMissingFiels(const TSharedPtr<FJsonObject> Object, int Index, const FString& FilePath, UStruct* StructType)
{
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		FProperty* Property = *It;

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
