// Copyright 2024 An@stacioDev All rights reserved.

#include "FileIO/DataManagerFunctionLibrary.h"
#include "HAL/PlatformFileManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/FileHelper.h"

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

bool UDataManagerFunctionLibrary::LoadProvinceData(const FString& FilePath, UDataTable* TargetDataTable)
{
	FString JsonContent;
	if (FFileHelper::LoadFileToString(JsonContent, *FilePath))
	{
		TArray<FVariantData> Provinces;
		if (ImportVariantData(JsonContent, Provinces))
		{
			PopulateDataTableWithArray(TargetDataTable, Provinces);
		}
	}
	return false;
}

bool UDataManagerFunctionLibrary::ImportVariantData(const FString& JsonContent, TArray<FVariantData>& OutProvinces)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonContent);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		// Iterate through the provinces
		for (auto& ProvinceEntry : JsonObject->Values)
		{
			TSharedPtr<FJsonObject> ProvinceObject = ProvinceEntry.Value->AsObject();
			if (ProvinceObject.IsValid())
			{
				FVariantData ProvinceData;
				for (const auto& Field : ProvinceObject->Values)
				{
					TVariant<int, float, FString, bool> variant;
					// Store each key-value pair in the TMap
					FString Key = Field.Key;
					TSharedPtr<FJsonValue> Value = Field.Value;
					// Handle different JSON value types and store as FVariant
					if (Value->Type == EJson::String)
					{
						variant.Set<FString>(Value->AsString());
					}
					else if (Value->Type == EJson::Number)
					{
						variant.Set<int>(Value->AsNumber());
					}
					else if (Value->Type == EJson::Boolean)
					{
						variant.Set<bool>(Value->AsBool());
					}
					else
					{
						// Default to string for unsupported types
						variant.Set<FString>(Value->AsString());
					}
					ProvinceData.Properties.Add(Key, variant);
				}
				OutProvinces.Add(ProvinceData);
			}
		}
		return true;
	}
	return false;
}