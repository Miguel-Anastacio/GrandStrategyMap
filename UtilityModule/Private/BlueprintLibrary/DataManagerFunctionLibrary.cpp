// Copyright 2024 An@stacioDev All rights reserved.

#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "HAL/PlatformFileManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/FileHelper.h"
#include "UtilityModule.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "Engine/DataTable.h"
void UAtkDataManagerFunctionLibrary::WriteStringToFile(const FString& FilePath, const FString& String, bool& bOutSuccess, FString& OutInfoMessage)
{
	if (!FFileHelper::SaveStringToFile(String, *FilePath))
	{
		bOutSuccess = false;
		OutInfoMessage = OutInfoMessage = FString::Printf(TEXT("Write string to file failed"));
		return;
	}

	bOutSuccess = true;
	OutInfoMessage = OutInfoMessage = FString::Printf(TEXT("Write string to file succeeded"));
}

TSharedPtr<FJsonObject> UAtkDataManagerFunctionLibrary::ReadJsonFile(const FString& FilePath)
{
	FString jsonString;
	if(!FFileHelper::LoadFileToString(jsonString, *FilePath))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Error loading file '%s'"), *FilePath);
		return nullptr;
	}
	TSharedPtr<FJsonObject> jsonObject;
	if(!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(jsonString), jsonObject))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Read Json Failed - error parsing file '%s'"), *FilePath);
	}
	return jsonObject;
}

TArray<TSharedPtr<FJsonValue>> UAtkDataManagerFunctionLibrary::ReadJsonFileArray(const FString& FilePath)
{
	FString jsonString;
	if(!FFileHelper::LoadFileToString(jsonString, *FilePath))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Error loading file '%s'"), *FilePath);
		return TArray<TSharedPtr<FJsonValue>>();
	}
	TArray<TSharedPtr<FJsonValue>> jsonValueArray;
	if(!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(jsonString), jsonValueArray))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Read Json Failed - error parsing file '%s'"), *FilePath);
	}
	return jsonValueArray;
}

TArray<TSharedPtr<FJsonValue>> UAtkDataManagerFunctionLibrary::ReadJsonFileArrayFromString(const FString& JsonString)
{
	TArray<TSharedPtr<FJsonValue>> JsonValues;
	if(!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonString), JsonValues))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Read Json Failed - error parsing json string: '%s'"), *JsonString);
	}
	return JsonValues;
}


TArray<FInstancedStruct> UAtkDataManagerFunctionLibrary::GetArrayOfInstancedStructs(const UDataTable* DataTable)
{
	if(!DataTable)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("GetArrayOfInstancedStructs : Data Table is NULL"));
		return TArray<FInstancedStruct>();
	}

	const UScriptStruct* RowStruct = DataTable->GetRowStruct();
	const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();

	TArray<FInstancedStruct> Instances;
	Instances.Reserve(RowMap.Num());
	for(const auto& [Name, Ptr] : RowMap )
	{
		FInstancedStruct Instance;
		Instance.InitializeAs(RowStruct, Ptr);
		Instances.Add(Instance);
	}
	return Instances;
}

TArray<FInstancedStruct> UAtkDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(
	const TSoftObjectPtr<UDataTable> DataTable)
{
	if (const UDataTable* LoadedDataTable = DataTable.LoadSynchronous())
	{
		return GetArrayOfInstancedStructs(LoadedDataTable);
	}
	return TArray<FInstancedStruct>();
}

TArray<FInstancedStruct> UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson(const FString& FilePath, const UScriptStruct* structType)
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
			// ObjectHasMissingFields(JsonObject, index, FilePath, structType);
			OutArray.Add(MoveTemp(NewInstancedStruct));
		}

		index++;
	}
	return OutArray;
}

TArray<FInstancedStruct> UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson(const FString& FilePath,
                                                                             const TArray<const UScriptStruct*>& StructTypes)
{
	TArray<TSharedPtr<FJsonValue>> JsonArray = ReadJsonFileArray(FilePath); 
	TArray<FInstancedStruct> OutArray;
	for(const auto& JsonValue : JsonArray)
	{
		TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
		if (!JsonObject.IsValid())
		{
			UE_LOG(LogUtilityModule, Error, TEXT("Invalid JSON object in array."));
			continue;
		}

		// Deserialize the object into an FInstancedStruct
		FInstancedStruct NewInstancedStruct;
		for(const auto& StructType : StructTypes)
		{
			const bool bResult = DeserializeJsonToFInstancedStruct(JsonObject, StructType, NewInstancedStruct);
			if (bResult && !ObjectHasMissingFields(JsonObject, StructType))
			{
				OutArray.Add(MoveTemp(NewInstancedStruct));
				break;
			}
		}
	}
	return OutArray;
}

void UAtkDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(const FString& FilePath,
	const TArray<FInstancedStruct>& Array)
{
	TArray<TSharedPtr<FJsonValue>> JsonValueArray;
	for (const auto& Value : Array)
	{
		TSharedPtr<FJsonObject> StructObject = SerializeInstancedStructToJson(Value);
		JsonValueArray.Add(MakeShared<FJsonValueObject>(StructObject));
	}

	bool bResult = false;
	FString OutInfoMessage;
	WriteJson(FilePath, JsonValueArray, bResult, OutInfoMessage);	
	if(!bResult)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Failed to Save Instanced Struct Array: %s"), *OutInfoMessage);
		UE_LOG(LogUtilityModule, Error, TEXT("Failed to Save Instanced Struct Array, filePath: %s"), *FilePath);
	}
}

bool UAtkDataManagerFunctionLibrary::DeserializeJsonToFInstancedStruct(const TSharedPtr<FJsonObject> JsonObject, const UScriptStruct* StructType, FInstancedStruct& OutInstancedStruct)
{
	if(!StructType)
		return false;
	// Initialize the FInstancedStruct with the resolved type
	OutInstancedStruct.InitializeAs(StructType);

	// Convert the JSON data into the struct instance
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructType, OutInstancedStruct.GetMutableMemory(), 0, 0))
	{
		return false;
	}
	return true;
}

TSharedPtr<FJsonObject> UAtkDataManagerFunctionLibrary::SerializeInstancedStructToJson(const FInstancedStruct& Instance)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	if(FJsonObjectConverter::UStructToJsonObject(Instance.GetScriptStruct(), Instance.GetMemory(), JsonObject.ToSharedRef(), 0, 0))
	{
		return JsonObject;
	}

	UE_LOG(LogUtilityModule, Error, TEXT("Failed to serialize instanced struct of type %s"), *Instance.GetScriptStruct()->GetName());
	return nullptr;
}

void UAtkDataManagerFunctionLibrary::WriteJson(const FString& JsonFilePath, const TSharedPtr<FJsonObject> JsonObject, bool& bOutSuccess, FString& OutInfoMessage)
{
	FString JSONString;
	
	if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&JSONString, 0)))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Write Json Failed - was not able to serialize the json string"));
		return;
	}

	WriteStringToFile(JsonFilePath, JSONString, bOutSuccess, OutInfoMessage);
	if (!bOutSuccess)
	{
		return;
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Write json succeeded = '%s"), *JsonFilePath);
}

void UAtkDataManagerFunctionLibrary::WriteJson(const FString& JsonFilePath, const TArray<TSharedPtr<FJsonValue>>& JsonValueArray, bool& bOutSuccess, FString& OutInfoMessage)
{
	FString JSONString;
	UE_LOG(LogTemp, Warning, TEXT("Map Num: %d"), JsonValueArray.Num());
	
	// Crashes in 5.5 -> TODO: investigate
	if (!FJsonSerializer::Serialize(JsonValueArray, TJsonWriterFactory<>::Create(&JSONString, 0)))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Write Json Failed - was not able to serialize the json string"));
		return;
	}

	WriteStringToFile(JsonFilePath, JSONString, bOutSuccess, OutInfoMessage);
	if (!bOutSuccess)
	{
		return;
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Write json succeeded = '%s"), *JsonFilePath);
}

bool UAtkDataManagerFunctionLibrary::ObjectHasMissingFields(const TSharedPtr<FJsonObject>& Object, const UStruct* StructType)
{
	bool bResult = false;
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		const FProperty* Property = *It;
		const FString PropertyName = Property->GetAuthoredName();

		// Check if the JSON object contains the field
		if (!Object->HasField(PropertyName))
		{
			// UE_LOG(LogUtilityModule, Warning, 
			// 	TEXT("Missing field '%s' in JSON object at index %d in file '%s'"), 
			// 	*PropertyName, Index, *FilePath);
			bResult = true;
		}
	}
	return bResult;
}

void UAtkDataManagerFunctionLibrary::LogReadJsonFailed(const FString& FilePath)
{
	UE_LOG(LogUtilityModule, Error, TEXT("Read Json Failed - some entries do not match the structure defined '%s'"), *FilePath);
}
