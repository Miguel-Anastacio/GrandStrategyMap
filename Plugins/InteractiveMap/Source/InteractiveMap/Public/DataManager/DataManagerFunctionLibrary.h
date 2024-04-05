// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonObjectConverter.h"
#include "Engine/DataTable.h"
#include "InteractiveMap.h"
#include "DataManagerFunctionLibrary.generated.h"


class FJsonObject;
/**
 * Library for managing data functions, such as reading from data tables and writing to JSON files.
 */
UCLASS()
class INTERACTIVEMAP_API UDataManagerFunctioLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Reads data from a data table into a map.
     *
     * @param dataTable The data table to read from.
     * @param mapToUpdate The map to update with the data.
     * @return True if successful, false otherwise.
     */
    template<class Data, class ID>
    static bool ReadDataTable(UDataTable* dataTable, TMap<ID, Data>& mapToUpdate)
    {
        if (!dataTable)
        {
            //UE_LOG(LogInteractiveMap, Error, TEXT("Data not loaded"));
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

    /**
     * Writes a structure to a JSON file.
     *
     * @param jsonFilePath The path to the JSON file.
     * @param structure The structure to write.
     * @param outSuccess Whether the operation was successful.
     * @param outInfoMessage Information message about the operation.
     */
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

    /**
     * Writes a map structure to a JSON file.
     *
     * @param jsonFilePath The path to the JSON file.
     * @param mapStructure The map structure to write.
     * @param outSuccess Whether the operation was successful.
     * @param outInfoMessage Information message about the operation.
     */
    template<class T>
    static void WriteMapToJsonFile(FString jsonFilePath, TMap<FName, T> mapStructure, bool& outSuccess, FString& outInfoMessage)
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

    /**
         * Writes a string to a file.
         *
         * @param filePath The path to the file.
         * @param string The string to write.
         * @param outSuccess Whether the operation was successful.
         * @param outInfoMessage Information message about the operation.
         */
    static void WriteStringToFile(FString filePath, FString string, bool& outSuccess, FString& outInfoMessage);

    /**
     * Writes JSON data to a file.
     *
     * @param jsonFilePath The path to the JSON file.
     * @param jsonObject The JSON object to write.
     * @param outSuccess Whether the operation was successful.
     * @param outInfoMessage Information message about the operation.
     */
    static void WriteJson(FString jsonFilePath, const TSharedPtr<FJsonObject> jsonObject, bool& outSuccess, FString& outInfoMessage);

    /**
     * Writes JSON data to a file.
     *
     * @param jsonFilePath The path to the JSON file.
     * @param jsonValueArray The array of JSON values to write.
     * @param outSuccess Whether the operation was successful.
     * @param outInfoMessage Information message about the operation.
     */
    static void WriteJson(FString jsonFilePath, TArray<TSharedPtr<FJsonValue>>& jsonValueArray, bool& outSuccess, FString& outInfoMessage);
};

