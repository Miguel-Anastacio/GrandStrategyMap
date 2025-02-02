// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonObjectConverter.h"
#include "Engine/DataTable.h"
#include "DataStructs.h"
#include "InstancedStruct.h"
#include "DataManagerFunctionLibrary.generated.h"

class FJsonObject;
/**
 * Library for managing data functions, such as reading from data tables and writing to JSON files.
 */
UCLASS()
class UTILITYMODULE_API UDataManagerFunctionLibrary : public UBlueprintFunctionLibrary
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
            return false;
        }
        for (auto& RowName : dataTable->GetRowNames())
        {
            Data* Item = dataTable->FindRow<Data>(RowName, "");
            if (Item)
            {
                mapToUpdate.Emplace(RowName, (*Item));
            }
        }
        return true;
    }
    
    /**
   * Reads data from a data table into a map.
   *
   * @param DataTable The data table to read from.
   * @param ArrayToUpdate The map to update with the data.
   * @return True if successful, false otherwise.
   */
    template<class T>
    static bool ReadDataTableToArray(const UDataTable* DataTable, TArray<T>& ArrayToUpdate)
    {
        if (!DataTable)
        {
            return false;
        }
        ArrayToUpdate.Empty();
        DataTable->GetAllRows("", ArrayToUpdate);
        return !ArrayToUpdate.IsEmpty();
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
    static void WriteStructToJsonFile(const FString& jsonFilePath,const T& structure, bool& outSuccess, FString& outInfoMessage)
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
    static void WriteMapToJsonFile(const FString& jsonFilePath, const TMap<FName, T>& mapStructure, bool& outSuccess, FString& outInfoMessage)
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
     * Writes an array to a JSON file.
     *
     * @param jsonFilePath The path to the JSON file.
     * @param array The array to write.
     * @param outSuccess Whether the operation was successful.
     * @param outInfoMessage Information message about the operation.
     */
    template<class T>
    static void WriteArrayToJsonFile(const FString& jsonFilePath, const TArray<T>& array, bool& outSuccess, FString& outInfoMessage)
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
        if (!JsonObject)
        {
            outSuccess = false;
            outInfoMessage = FString::Printf(TEXT("Write struct json failed - not able to convert structure to json object (structure has to be a UStruct)"));
            return;
        }

        TArray<TSharedPtr<FJsonValue>> JsonValues;
        for (const auto& Value : array)
        {
            TSharedPtr<FJsonObject> StructObject = MakeShared<FJsonObject>();
            Value.SerializeToJson(StructObject);
            JsonValues.Add(MakeShared<FJsonValueObject>(StructObject));
        }

        WriteJson(jsonFilePath, JsonValues, outSuccess, outInfoMessage);
    }
    
    ///// ONLY FOR TESTING PURPOSES -> TODO: Remove
    UFUNCTION(BlueprintCallable, Category = "Data Loader")
    static  TArray<FTestBasic> LoadTestBasic(const FString& FilePath);
    UFUNCTION(BlueprintCallable, Category = "Data Loader")
    static  TArray<FTestAdvanced> LoadTestAdvanced(const FString& FilePath);
    ////////////////////////////////////////////////////////////////

    template<class T>
    static TArray<T> LoadCustomDataFromJson(const FString& FilePath)
    {
        TArray<TSharedPtr<FJsonValue>> JsonArray = ReadJsonFileArray(FilePath); 
        TArray<T> OutArray;
        int32 Index = 0;
        for(const auto& JsonValue : JsonArray)
        {
            if (JsonValue->Type == EJson::Object)
            {
                T StructInstance;
                TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
                if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), T::StaticStruct(), &StructInstance, 0, 0, true))
                {
                    ObjectHasMissingFields(JsonObject, Index, FilePath, T::StaticStruct());
                    OutArray.Emplace(StructInstance);
                }
                else
                {
                    // if you do not get this error but the array is not filled correctly
                    // make sure that the struct members are marked as UPROPERTY
                    LogReadJsonFailed(FilePath);
                    OutArray.Empty();
                }
                Index++;
            }
        }
        return OutArray;
    }

    static TArray<FInstancedStruct> LoadCustomDataFromJson(const FString& FilePath, const UScriptStruct* StructType);
    static void WriteInstancedStructArrayToJson(const FString& FilePath, const UScriptStruct* StructType, const TArray<FInstancedStruct>& Array);
    
    static bool DeserializeJsonToFInstancedStruct(const TSharedPtr<FJsonObject> JsonObject,const UScriptStruct* StructType, FInstancedStruct& OutInstancedStruct);
    static TSharedPtr<FJsonObject> SerializeInstancedStructToJson(const FInstancedStruct& Instance, const UScriptStruct* StructType);

    // --------------------------------------------------------------------
    // COLOR STUFF -> Move somewhere else
    static FColor ConvertHexStringToRGB(const FString& Color);
private:
    static auto HexToDecimal(const FString& hex, const TMap<TCHAR, int32>& HexMap) -> int32;

    /**
     * Writes a string to a file.
     *
     * @param filePath The path to the file.
     * @param string The string to write.
     * @param outSuccess Whether the operation was successful.
     * @param outInfoMessage Information message about the operation.
     */
    static void WriteStringToFile(const FString& filePath, const FString& string, bool& outSuccess, FString& outInfoMessage);
    
    /**
     * Writes JSON data to a file.
     *
     * @param jsonFilePath The path to the JSON file.
     * @param jsonObject The JSON object to write.
     * @param outSuccess Whether the operation was successful.
     * @param outInfoMessage Information message about the operation.
     */
    static void WriteJson(const FString& jsonFilePath, const TSharedPtr<FJsonObject> jsonObject, bool& outSuccess, FString& outInfoMessage);
    
    /**
     * Writes JSON data to a file.
     *
     * @param jsonFilePath The path to the JSON file.
     * @param jsonValueArray The array of JSON values to write.
     * @param outSuccess Whether the operation was successful.
     * @param outInfoMessage Information message about the operation.
     */
    static void WriteJson(const FString& jsonFilePath, TArray<TSharedPtr<FJsonValue>>& jsonValueArray, bool& outSuccess, FString& outInfoMessage);

    static TSharedPtr<FJsonObject> ReadJsonFile(const FString& filePath);
    static TArray<TSharedPtr<FJsonValue>> ReadJsonFileArray(const FString& filePath);
    static void ObjectHasMissingFields(const TSharedPtr<FJsonObject>& Object, int Index, const FString& FilePath, const UStruct* StructType);
    static void PopulateDataTableWithArray(UDataTable* DataTable, const TArray<FVariantData>& Array);
    static void LogReadJsonFailed(const FString& FilePath);
};

