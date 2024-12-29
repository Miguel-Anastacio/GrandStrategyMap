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
        TSharedPtr<FJsonObject> jsonObject = MakeShared<FJsonObject>();
        if (!jsonObject)
        {
            outSuccess = false;
            outInfoMessage = FString::Printf(TEXT("Write struct json failed - not able to convert structure to json object (structure has to be a UStruct)"));
            return;
        }

        TArray<TSharedPtr<FJsonValue>> jsonValueArray;
        for (const auto& value : array)
        {
            TSharedPtr<FJsonObject> structObject = MakeShared<FJsonObject>();
            value.SerializeToJson(structObject);
            jsonValueArray.Add(MakeShared<FJsonValueObject>(structObject));
        }

        WriteJson(jsonFilePath, jsonValueArray, outSuccess, outInfoMessage);
    }



    ///// ONLY FOR TESTING PURPOSES -> TODO: Remove
    UFUNCTION(BlueprintCallable, Category = "Data Loader")
    static  TArray<FTestBasic> LoadTestBasic(const FString& FilePath);
    UFUNCTION(BlueprintCallable, Category = "Data Loader")
    static  TArray<FTestAdvanced> LoadTestAdvanced(const FString& FilePath);
    
    template<class T>
    static TArray<T> LoadCustomDataFromJson(const FString& FilePath)
    {
        TArray<TSharedPtr<FJsonValue>> JsonArray = ReadJsonFileArray(FilePath); 
        TArray<T> OutArray;
        int32 Index = 0;
        for(const auto& jsonValue : JsonArray)
        {
            if (jsonValue->Type == EJson::Object)
            {
                T StructInstance;
                TSharedPtr<FJsonObject> jsonObject = jsonValue->AsObject();
                if (FJsonObjectConverter::JsonObjectToUStruct(jsonObject.ToSharedRef(), T::StaticStruct(), &StructInstance, 0, 0, true))
                {
                    ObjectHasMissingFiels(jsonObject, Index, FilePath, T::StaticStruct());
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
    // ------------------------------------------------------------------
    // INSTANCE STRUCT HELPER
    static TArray<FInstancedStruct> LoadCustomDataFromJson(const FString& FilePath, const UScriptStruct* StructType);
    static void WriteInstancedStructArrayToJson(const FString& FilePath, const UScriptStruct* StructType, const TArray<FInstancedStruct>& Array);
    
    static bool DeserializeJsonToFInstancedStruct(const TSharedPtr<FJsonObject> JsonObject,const UScriptStruct* StructType, FInstancedStruct& OutInstancedStruct);
    static TSharedPtr<FJsonObject> SerializeInstancedStructToJson(const FInstancedStruct& Instance, const UScriptStruct* StructType);
    static void* CreateStructInstance(const UStruct* type);

    // static const FInstancedStruct* GetSructWithProperty(const FString& PropertyName)

    static FString GetPropertyValueAsString(FProperty* Property, const void* StructObject, bool& OutResult);

    static FProperty* GetPropertyByName(const UScriptStruct* StructType, const FString& PropertyName);

    template<typename T>
    static T GetPropertyValueFromStruct(const UScriptStruct* StructType, const FInstancedStruct& InstancedStruct, const FString& PropertyName, bool& OutResult)
    {
        OutResult = false;
        const FProperty* Property = GetPropertyByName(StructType, PropertyName);
        if(Property)
        {
            T PropertyValue = GetPropertyValueAsType<T>(Property, InstancedStruct.GetMemory(), OutResult);
            if(OutResult)
            {
                return PropertyValue;
            }
        }
        return T();
    }
    
    template<typename T>
    static T GetPropertyValueAsType(const FProperty* Property, const void* StructObject, bool& OutResult)
    {
        OutResult = true;
        if (!Property || !StructObject)
        {
            OutResult = false;
        }
        const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructObject);

        if (const FIntProperty* IntProperty = CastField<FIntProperty>(Property))
        {
            return static_cast<T>(IntProperty->GetPropertyValue(ValuePtr));
        }
        else if (const FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
        {
            return static_cast<T>(FloatProperty->GetPropertyValue(ValuePtr)); 
        }
        else if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
        {
            return static_cast<T>(BoolProperty->GetPropertyValue(ValuePtr)); 
        }
        else if (const FStrProperty* StrProperty = CastField<FStrProperty>(Property))
        {
            if constexpr (std::is_same_v<T, FString>)
            {
                return StrProperty->GetPropertyValue(ValuePtr);
            }
            OutResult = false;
            return T();
        }
        else if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
        {
            if constexpr (std::is_same_v<T, FName>)
            {
                return NameProperty->GetPropertyValue(ValuePtr);
            }
            OutResult = false;
            return T();
        }
        else if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
        {
            if constexpr (std::is_same_v<T, FText>)
            {
                return TextProperty->GetPropertyValue(ValuePtr);
            }
            OutResult = false;
            return T();
        }
        else if(const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
        {
            if constexpr (std::is_same_v<T, void*>)
            {
                // For structs, we can return the raw pointer or specialize further
                return static_cast<T>(ValuePtr);
            }
            OutResult = false;
            return T();
        }
        else
        {
            OutResult = false;
            return T();
        }
    }

    template<typename T, typename V>
    static T GetPropertyValue(FProperty* Property, const V* object)
    {
        void* ValuePtr = Property->ContainerPtrToValuePtr<void>(object);
        T Result = TPropertyTypeFundamentals<T>::GetPropertyValue(ValuePtr);
        return Result;
    }

    // --------------------------------------------------------------------
    // COLOR STUFF -> Move somewhere else
    static FColor ConvertHexStringToRGB(const FString& Color);
private:
    static auto HexToDecimal(const FString& hex, const TMap<TCHAR, int32>& HexMap) -> int32;
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

    static void PopulateDataTableWithArray(UDataTable* DataTable, const TArray<FVariantData>& Array);
    static void ObjectHasMissingFiels(const TSharedPtr<FJsonObject> Object, int Index, const FString& FilePath, const UStruct* StructType);
    static void LogReadJsonFailed(const FString& FilePath);
};

