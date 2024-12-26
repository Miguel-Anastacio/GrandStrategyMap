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
    template<class T>
    static void WriteArrayToJsonFile(const FString& jsonFilePath, const TArray<T>& Array, bool& outSuccess, FString& outInfoMessage)
    {
        TSharedPtr<FJsonObject> jsonObject = MakeShared<FJsonObject>();
        if (!jsonObject)
        {
            outSuccess = false;
            outInfoMessage = FString::Printf(TEXT("Write struct json failed - not able to convert structure to json object (structure has to be a UStruct)"));
            return;
        }

        TArray<TSharedPtr<FJsonValue>> jsonValueArray;
        for (const auto& value : Array)
        {
            TSharedPtr<FJsonObject> structObject = MakeShared<FJsonObject>();
            value.SerializeToJson(structObject);
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
    static void WriteStringToFile(const FString& filePath, const FString& string, bool& outSuccess, FString& outInfoMessage);

    static TSharedPtr<FJsonObject> ReadJsonFile(const FString& filePath);
    static TArray<TSharedPtr<FJsonValue>> ReadJsonFileArray(const FString& filePath);

    UFUNCTION(BlueprintCallable, Category = "Data Loader")
    static  TArray<FTestBasic> LoadTestBasic(const FString& FilePath);
    UFUNCTION(BlueprintCallable, Category = "Data Loader")
    static  TArray<FTestAdvanced> LoadTestAdvanced(const FString& FilePath);
    
    template<class T>
    static TArray<T> LoadCustomDataFromJson(const FString& FilePath)
    {
        TArray<TSharedPtr<FJsonValue>> JsonArray = ReadJsonFileArray(FilePath); 
        TArray<T> outArray;
        int32 index = 0;
        for(const auto& jsonValue : JsonArray)
        {
            if (jsonValue->Type == EJson::Object)
            {
                T StructInstance;
                TSharedPtr<FJsonObject> jsonObject = jsonValue->AsObject();
                if (FJsonObjectConverter::JsonObjectToUStruct(jsonObject.ToSharedRef(), T::StaticStruct(), &StructInstance, 0, 0, true))
                {
                    ObjectHasMissingFiels(jsonObject, index, FilePath, T::StaticStruct());
                    outArray.Emplace(StructInstance);
                }
                else
                {
                    // if you do not get this error but the array is not filled correctly
                    // make sure that the struct members are marked as UPROPERTY
                    LogReadJsonFailed(FilePath);
                    outArray.Empty();
                }
                index++;
            }
        }
        return outArray;
    }

    static TArray<FInstancedStruct> LoadCustomDataFromJson(const FString& FilePath, UScriptStruct* structType);

    static bool DeserializeJsonToFInstancedStruct(const TSharedPtr<FJsonObject> JsonObject,const UScriptStruct* StructType, FInstancedStruct& OutInstancedStruct);

    static void* CreateStructInstance(const UStruct* type);
    
private:
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


    static void PopulateDataTableWithArray(UDataTable* DataTable, const TArray<FVariantData>& Array);

    // template<typename T>
    // static void ObjectHasMissingFiels(const TSharedPtr<FJsonObject> Object, int Index, const FString& FilePath)
    // {
    //     for (TFieldIterator<FProperty> It(T::StaticStruct()); It; ++It)
    //     {
    //         FProperty* Property = *It;
    //         
    //
    //         // Get property name
    //         FString PropertyName = Property->GetName();
    //
    //         // Check if the JSON object contains the field
    //         if (!Object->HasField(PropertyName))
    //         {
    //             UE_LOG(LogUtilityModule, Warning, 
    //                 TEXT("Missing field '%s' in JSON object at index %d in file '%s'"), 
    //                 *PropertyName, Index, *FilePath);
    //             return;
    //         }
    //     }
    // }

    static void ObjectHasMissingFiels(const TSharedPtr<FJsonObject> Object, int Index, const FString& FilePath, UStruct* StructType);
    static void LogReadJsonFailed(const FString& FilePath);
};

