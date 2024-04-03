// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonObjectConverter.h"
#include "DataManagerFunctionLibrary.generated.h"


//template<class Data>
//USTRUCT(BlueprintType)
//struct FJsonFileStruct
//{
//	GENERATED_BODY()
//
//	//UPROPERTY()
//	//TArray<FName> name;
//	UPROPERTY()
//	TMap<FName,Data> StructData;
//
//	FJsonFileStruct(TMap<FName, Data>& data) : StructData(data) {};
//};
/**
 * 
 */
class FJsonObject;
UCLASS()
class INTERACTIVEMAP_API UDataManagerFunctioLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	template<class Data, class ID>
	static bool ReadDataTable(UDataTable* dataTable, TMap<ID, Data>& mapToUpdate)
	{
		if (!dataTable)
		{
			UE_LOG(LogTemp, Error, TEXT("Data not loaded"));
			return false;
		}
		TArray<FName> RowNames = dataTable->GetRowNames();
		for (auto& name : RowNames)
		{
			Data* Item = dataTable->FindRow<Data>(name, "");
			if (Item)
			{
				mapToUpdate.Emplace(name, (*Item));
			}
		}
		return true;
	}

	//UFUNCTION()
	template<class T>
	static void WriteStructToJsonFile(FString jsonFilePath, T structure, bool& outSuccess, FString& outInfoMessage)
	{
		TSharedPtr<FJsonObject> jsonObject = FJsonObjectConverter::UStructToJsonObject(structure);
		if (!jsonObject)
		{
			outSuccess = false;
			outInfoMessage = FString::Printf(TEXT("Write struct json failed - not able to convert structure to json object (structure has to be a UStruct) "));
			return;
		}
		WriteJson(jsonFilePath, jsonObject, outSuccess, outInfoMessage);
	}

	template<class T>
	static void WriteMapToJsonFile(FString jsonFilePath, TMap<FName,T> mapStructure, bool& outSuccess, FString& outInfoMessage)
	{
		TSharedPtr<FJsonObject> jsonObject = MakeShared<FJsonObject>();
		if (!jsonObject)
		{
			outSuccess = false;
			outInfoMessage = FString::Printf(TEXT("Write struct json failed - not able to convert structure to json object (structure has to be a UStruct) "));
			return;
		}

		TArray<TSharedPtr<FJsonValue>> jsonValueArray;
		for (const auto& pair : mapStructure)
		{
			TSharedPtr<FJsonObject> structObject = MakeShared<FJsonObject>();
			pair.Value.SerializeToJson(structObject, pair.Key);
			jsonValueArray.Add(MakeShared<FJsonValueObject>(structObject));
		}

		WriteJson(jsonFilePath, jsonValueArray, outSuccess, outInfoMessage);
	}

	static void WriteStringToFile(FString filePath, FString string, bool& outSuccess, FString& outInfoMessage);

	static TSharedPtr<FJsonObject> ReadJson(FString jSonFilePath, bool& outSuccess, FString& outInfoMessage);

	static void WriteJson(FString jsonFilePath, const TSharedPtr<FJsonObject> jsonObject, bool& outSuccess, FString& outInfoMessage);
	static void WriteJson(FString jsonFilePath, TArray<TSharedPtr<FJsonValue>>& jsonValueArray, bool& outSuccess, FString& outInfoMessage);

};
