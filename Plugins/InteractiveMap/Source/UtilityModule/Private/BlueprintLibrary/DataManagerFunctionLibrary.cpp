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
			// ObjectHasMissingFields(JsonObject, index, FilePath, structType);
			OutArray.Add(MoveTemp(NewInstancedStruct));
		}

		index++;
	}
	return OutArray;
}

TArray<FInstancedStruct> UDataManagerFunctionLibrary::LoadCustomDataFromJson(const FString& FilePath,
	const TArray<UScriptStruct*>& StructTypes)
{
	TArray<TSharedPtr<FJsonValue>> JsonArray = ReadJsonFileArray(FilePath); 
	TArray<FInstancedStruct> OutArray;
	int32 Index = 0;
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
			if (bResult && !ObjectHasMissingFields(JsonObject, Index, FilePath, StructType))
			{
				OutArray.Add(MoveTemp(NewInstancedStruct));
				Index++;
				break;
			}
		}
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

void UDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(const FString& FilePath,
	const TArray<FInstancedStruct>& Array)
{
	TArray<TSharedPtr<FJsonValue>> JsonValueArray;
	for (const auto& Value : Array)
	{
		TSharedPtr<FJsonObject> StructObject = SerializeInstancedStructToJson(Value, Value.GetScriptStruct());
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

TSharedPtr<FJsonObject> UDataManagerFunctionLibrary::SerializeInstancedStructToJson(const FInstancedStruct& Instance)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	if(FJsonObjectConverter::UStructToJsonObject(Instance.GetScriptStruct(), Instance.GetMemory(), JsonObject.ToSharedRef(), 0, 0))
	{
		return JsonObject;
	}

	UE_LOG(LogUtilityModule, Error, TEXT("Failed to serialize instanced struct of type %s"), *Instance.GetScriptStruct()->GetName());
	return nullptr;
}

int32 UDataManagerFunctionLibrary::HexToDecimal(const FString& Hex)
{
	static const TMap<TCHAR, int32> HexMap
	{
		TPair<TCHAR, int32>('0', 0),
		TPair<TCHAR, int32>('1', 1),
		TPair<TCHAR, int32>('2', 2),
		TPair<TCHAR, int32>('3', 3),
		TPair<TCHAR, int32>('4', 4),
		TPair<TCHAR, int32>('5', 5),
		TPair<TCHAR, int32>('6', 6),
		TPair<TCHAR, int32>('7', 7),
		TPair<TCHAR, int32>('8', 8),
		TPair<TCHAR, int32>('9', 9),
		TPair<TCHAR, int32>('A', 10),
		TPair<TCHAR, int32>('B', 11),
		TPair<TCHAR, int32>('C', 12),
		TPair<TCHAR, int32>('D', 13),
		TPair<TCHAR, int32>('E', 14),
		TPair<TCHAR, int32>('F', 15),
   };
	const FString Temp = Hex.Reverse().ToUpper();
	int32 Total = 0;
	for (int i = 0; i < Temp.Len(); i++)
	{
		const int32* Value = HexMap.Find(Temp[i]);
		if (Value)
		{
			Total += FMath::Pow(16.f, i) * (*Value);
		}
		else
		{
			UE_LOG(LogUtilityModule, Warning, TEXT("Invalid Hex value"));
		}
	}
	return Total;
}

FColor UDataManagerFunctionLibrary::ConvertHexStringToRGB(const FString& Color)
{
	if (Color.StartsWith(FString("#")))
	{
		const FString noPrefix = Color.RightChop(1);
		const FString r = noPrefix.LeftChop(6);
		const FString g = noPrefix.Mid(2, 2);
		const FString b = noPrefix.Mid(4, 2);
		const FString alpha = noPrefix.RightChop(6);

		FColor ColorValue;
		ColorValue.R = HexToDecimal(r);
		ColorValue.G = HexToDecimal(g);
		ColorValue.B = HexToDecimal(b);
		ColorValue.A = HexToDecimal(alpha);

		return ColorValue;
	}

	UE_LOG(LogUtilityModule, Error, TEXT("Color is not hexadecimal format"));
	return FColor();
}

void UDataManagerFunctionLibrary::WriteJson(const FString& jsonFilePath, const TSharedPtr<FJsonObject> jsonObject, bool& outSuccess, FString& outInfoMessage)
{
	FString JSONString;

	if (!FJsonSerializer::Serialize(jsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&JSONString, 0)))
	{
		outSuccess = false;
		outInfoMessage = FString::Printf(TEXT("Write Json Failed - was not able to serialize the json string"));
		return;
	}

	WriteStringToFile(jsonFilePath, JSONString, outSuccess, outInfoMessage);
	if (!outSuccess)
	{
		return;
	}

	outSuccess = true;
	outInfoMessage = FString::Printf(TEXT("Write json succeeded = '%s"), *jsonFilePath);
}

void UDataManagerFunctionLibrary::WriteJson(const FString& jsonFilePath, const TArray<TSharedPtr<FJsonValue>>& jsonValueArray, bool& outSuccess, FString& outInfoMessage)
{
	FString JSONString;

	if (!FJsonSerializer::Serialize(jsonValueArray, TJsonWriterFactory<>::Create(&JSONString, 0)))
	{
		outSuccess = false;
		outInfoMessage = FString::Printf(TEXT("Write Json Failed - was not able to serialize the json string"));
		return;
	}

	WriteStringToFile(jsonFilePath, JSONString, outSuccess, outInfoMessage);
	if (!outSuccess)
	{
		return;
	}

	outSuccess = true;
	outInfoMessage = FString::Printf(TEXT("Write json succeeded = '%s"), *jsonFilePath);
}

bool UDataManagerFunctionLibrary::ObjectHasMissingFields(const TSharedPtr<FJsonObject>& Object, int Index, const FString& FilePath, const UStruct* StructType)
{
	bool bResult = false;
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		const FProperty* Property = *It;
		const FString PropertyName = Property->GetAuthoredName();

		// Check if the JSON object contains the field
		if (!Object->HasField(PropertyName))
		{
			UE_LOG(LogUtilityModule, Warning, 
				TEXT("Missing field '%s' in JSON object at index %d in file '%s'"), 
				*PropertyName, Index, *FilePath);
			bResult = true;
		}
	}
	return bResult;
}

void UDataManagerFunctionLibrary::LogReadJsonFailed(const FString& FilePath)
{
	UE_LOG(LogUtilityModule, Error, TEXT("Read Json Failed - some entries do not match the structure defined '%s'"), *FilePath);
}
