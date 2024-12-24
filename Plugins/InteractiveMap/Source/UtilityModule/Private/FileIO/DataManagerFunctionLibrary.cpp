// Copyright 2024 An@stacioDev All rights reserved.

#include "FileIO/DataManagerFunctionLibrary.h"
#include "HAL/PlatformFileManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/FileHelper.h"
#include "UtilityModule.h"
#include "UObject/UnrealTypePrivate.h"

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


TArray<void*> UDataManagerFunctionLibrary::LoadCustomDataFromJson(const FString& FilePath, UStruct* structType)
{
	TArray<TSharedPtr<FJsonValue>> JsonArray = ReadJsonFileArray(FilePath); 
	TArray<void*> outArray;
	int32 index = 0;
	for(const auto& jsonValue : JsonArray)
	{
		UObject* Object = NewObject<UObject>();
		FProperty* property = Object->GetClass()->FindPropertyByName("Test");
		FStructProperty* structProperty = Cast<FStructProperty>(property);

		if (jsonValue->Type == EJson::Object)
		{
			void* StructInstance = CreateStructInstance(structType);
			TSharedPtr<FJsonObject> jsonObject = jsonValue->AsObject();
			if (FJsonObjectConverter::JsonObjectToUStruct(jsonObject.ToSharedRef(), structType, &StructInstance, 0, 0, true))
			{
			    ObjectHasMissingFiels(jsonObject, index, FilePath, structType);
			    outArray.Emplace(&StructInstance);
			}
			else
			{
			    // if you do not get this error but the array is not filled correctly
			    // make sure that the struct members are marked as UPROPERTY
			    UE_LOG(LogUtilityModule, Error, TEXT("Read Json Failed - some entries do not match the structure defined '%s'"), *FilePath);
			    outArray.Empty();
			}
			index++;
		}
	}
	return outArray;
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
