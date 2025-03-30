// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonObjectConverter.h"
#include "Engine/DataTable.h"
#include "InstancedStruct.h"
#include "DataManagerFunctionLibrary.generated.h"

class FJsonObject;
/**
 * Library for managing data functions, such as reading from data tables and writing to JSON files.
 */
UCLASS(DisplayName="Data Manager Function Library")
class UTILITYMODULE_API UAtkDataManagerFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    
    UFUNCTION(BlueprintCallable, BlueprintPure ,Category = "Data")
    static TArray<FInstancedStruct> GetArrayOfInstancedStructs(const UDataTable* DataTable);
    
    UFUNCTION(BlueprintCallable, BlueprintPure ,Category = "Data")
    static TArray<FInstancedStruct> GetArrayOfInstancedStructsSoft(const TSoftObjectPtr<UDataTable> DataTable);
    
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

    UFUNCTION(BlueprintCallable, Category=JsonUtils)
    static void WriteInstancedStructArrayToJson(const FString& FilePath, const TArray<FInstancedStruct>& Array);
    /**
     * Writes a structure to a JSON file.
     *
     * @param FilePath The path to the JSON file.
     * @param Structure The structure to write.
     * @param bOutSuccess Whether the operation was successful.
     * @param OutInfoMessage Information message about the operation.
     */
    template<class T>
    static void WriteStructToJsonFile(const FString& FilePath,const T& Structure, bool& bOutSuccess, FString& OutInfoMessage)
    {
        TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(Structure);
        if (!JsonObject)
        {
            bOutSuccess = false;
            OutInfoMessage = FString::Printf(TEXT("Write struct json failed - not able to convert structure to json object (structure has to be a UStruct) "));
            return;
        }
        WriteJson(FilePath, JsonObject, bOutSuccess, OutInfoMessage);
    }

     /**
     * Writes an array to a JSON file.
     *
     * @param JsonFilePath The path to the JSON file.
     * @param Array The array to write.
     * @param bOutSuccess Whether the operation was successful.
     * @param OutInfoMessage Information message about the operation.
     */
    template<class T>
    static void WriteArrayToJsonFile(const FString& JsonFilePath, const TArray<T>& Array, bool& bOutSuccess, FString& OutInfoMessage)
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
        if (!JsonObject)
        {
            bOutSuccess = false;
            OutInfoMessage = FString::Printf(TEXT("Write struct json failed - not able to convert structure to json object (structure has to be a UStruct)"));
            return;
        }

        TArray<TSharedPtr<FJsonValue>> JsonValues;
        for (const auto& Value : Array)
        {
            TSharedPtr<FJsonObject> StructObject = MakeShared<FJsonObject>();
            Value.SerializeToJson(StructObject);
            JsonValues.Add(MakeShared<FJsonValueObject>(StructObject));
        }

        WriteJson(JsonFilePath, JsonValues, bOutSuccess, OutInfoMessage);
    }
    
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
    static TArray<FInstancedStruct> LoadCustomDataFromJson(const FString& FilePath, const TArray<UScriptStruct*>& StructTypes);
    
    static bool DeserializeJsonToFInstancedStruct(const TSharedPtr<FJsonObject> JsonObject,const UScriptStruct* StructType, FInstancedStruct& OutInstancedStruct);
    static TSharedPtr<FJsonObject> SerializeInstancedStructToJson(const FInstancedStruct& Instance);
    
    /**
     * Writes a string to a file.
     *
     * @param FilePath The path to the file.
     * @param String The string to write.
     * @param bOutSuccess Whether the operation was successful.
     * @param OutInfoMessage Information message about the operation.
     */
    UFUNCTION(BlueprintCallable, Category=JsonUtils)
    static void WriteStringToFile(const FString& FilePath, const FString& String, bool& bOutSuccess, FString& OutInfoMessage);
    
private:
    /**
     * Writes JSON data to a file.
     *
     * @param JsonFilePath The path to the JSON file.
     * @param JsonObject The JSON object to write.
     * @param bOutSuccess Whether the operation was successful.
     * @param OutInfoMessage Information message about the operation.
     */
    static void WriteJson(const FString& JsonFilePath, const TSharedPtr<FJsonObject> JsonObject, bool& bOutSuccess, FString& OutInfoMessage);
    
    /**
     * Writes JSON data to a file.
     *
     * @param JsonFilePath The path to the JSON file.
     * @param JsonValueArray The array of JSON values to write.
     * @param bOutSuccess Whether the operation was successful.
     * @param OutInfoMessage Information message about the operation.
     */
    static void WriteJson(const FString& JsonFilePath, const TArray<TSharedPtr<FJsonValue>>& JsonValueArray, bool& bOutSuccess, FString& OutInfoMessage);

    static TSharedPtr<FJsonObject> ReadJsonFile(const FString& FilePath);
    static TArray<TSharedPtr<FJsonValue>> ReadJsonFileArray(const FString& FilePath);
    static TArray<TSharedPtr<FJsonValue>> ReadJsonFileArrayFromString(const FString& JsonString);
    static bool ObjectHasMissingFields(const TSharedPtr<FJsonObject>& Object, int Index, const FString& FilePath, const UStruct* StructType);
    static void LogReadJsonFailed(const FString& FilePath);
};

