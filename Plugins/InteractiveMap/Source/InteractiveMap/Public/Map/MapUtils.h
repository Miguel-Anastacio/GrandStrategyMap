// Copyright 2024 An@stacioDev All rights reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Dom/JsonObject.h"
#include "Misc/TVariant.h"
#include "MapUtils.generated.h"


static void InitHexMap(TMap<TCHAR, int32>& HexMap)
{
    HexMap.Add('0', 0);
    HexMap.Add('1', 1);
    HexMap.Add('2', 2);
    HexMap.Add('3', 3);
    HexMap.Add('4', 4);
    HexMap.Add('5', 5);
    HexMap.Add('6', 6);
    HexMap.Add('7', 7);
    HexMap.Add('8', 8);
    HexMap.Add('9', 9);
    HexMap.Add('A', 10);
    HexMap.Add('B', 11);
    HexMap.Add('C', 12);
    HexMap.Add('D', 13);
    HexMap.Add('E', 14);
    HexMap.Add('F', 15);
};

// Properties that have a map color associated with 
USTRUCT(BlueprintType)
struct FColoredData : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FName Type;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FString DataName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FColor Color = FColor::Black;

    FColoredData() : DataName(FString("")), Color(FColor::Black) {};

    FColoredData(const FString& name, FColor color) :
        DataName(name), Color(color) {};

    // Serialize to JSON
    void SerializeToJson(TSharedPtr<FJsonObject>& OutJsonObject, FName key) const
    {
        OutJsonObject->SetStringField(TEXT("Name"), key.ToString());
        OutJsonObject->SetStringField(TEXT("Type"), Type.ToString());
        OutJsonObject->SetStringField(TEXT("DataName"), DataName);
        TSharedPtr<FJsonObject> ColorObject = MakeShared<FJsonObject>();
        ColorObject->SetNumberField(TEXT("B"), Color.B);
        ColorObject->SetNumberField(TEXT("G"), Color.G);
        ColorObject->SetNumberField(TEXT("R"), Color.R);
        ColorObject->SetNumberField(TEXT("A"), Color.A);
        OutJsonObject->SetObjectField(TEXT("Color"), ColorObject);
    }
};


USTRUCT(BlueprintType)
struct FProvinceIDData : public FTableRowBase
{
    GENERATED_BODY()
    FProvinceIDData() {};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FString Color = "";
    // assuming format AABBCCTT (AA = R, BB = G, CC = B, TT = Alpha)
    int32 HexToDecimal(const FString& hex, const TMap<TCHAR, int32>& HexMap);
    FColor ConvertHexStringToRGB(const FString& color, const TMap<TCHAR, int32>& HexMap);
};

USTRUCT(BlueprintType)
struct FProvinceData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FString ProvinceName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FName Owner;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    int64 Population;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FName Religion;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FName Culture;
    // Add more province data here

    FProvinceData() : ProvinceName(FString("")), Owner(FName()), Population(0), Religion(FName()) {};

    FProvinceData(const FString& name, FName owner, int64 population, FName religion, FName culture) :
        ProvinceName(name), Owner(owner), Population(population), Religion(religion), Culture(culture) {};

    void PrintProvinceData();

    // Serialize to JSON
    void SerializeToJson(TSharedPtr<FJsonObject>& OutJsonObject, FName key) const
    {
        OutJsonObject->SetStringField(TEXT("Name"), key.ToString());
        OutJsonObject->SetStringField(TEXT("ProvinceName"), ProvinceName);
        OutJsonObject->SetStringField(TEXT("Owner"), Owner.ToString());
        OutJsonObject->SetNumberField(TEXT("Population"), Population);
        OutJsonObject->SetStringField(TEXT("Religion"), Religion.ToString());
        OutJsonObject->SetStringField(TEXT("Culture"), Culture.ToString());
        // Add more fields as needed
    }
};

USTRUCT(BlueprintType)
struct FCountryData : public FTableRowBase
{
    GENERATED_BODY()
    FCountryData() {};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FString CountryName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FColor Color = FColor::Black;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    TArray<FName> Provinces;
    // Add more country data here

    FCountryData(const FString& name, const FColor& color) : CountryName(name), Color(color)
    {};

    // Serialize to JSON
    void SerializeToJson(TSharedPtr<FJsonObject>& OutJsonObject, FName key) const
    {
        OutJsonObject->SetStringField(TEXT("Name"), key.ToString());
        OutJsonObject->SetStringField(TEXT("CountryName"), CountryName);
        TSharedPtr<FJsonObject> ColorObject = MakeShared<FJsonObject>();
        ColorObject->SetNumberField(TEXT("B"), Color.B);
        ColorObject->SetNumberField(TEXT("G"), Color.G);
        ColorObject->SetNumberField(TEXT("R"), Color.R);
        ColorObject->SetNumberField(TEXT("A"), Color.A);
        OutJsonObject->SetObjectField(TEXT("Color"), ColorObject);

        // Populate array with provinces
        TArray<TSharedPtr<FJsonValue>> provinceIDs;
        for (const FName& name : Provinces)
        {
            provinceIDs.Add(MakeShareable(new FJsonValueString(name.ToString())));
        }

        OutJsonObject->SetArrayField(TEXT("Provinces"), provinceIDs);

        // Add more fields as needed
    }
};


USTRUCT(BlueprintType)
struct FVisualPropertyType : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere)
    UMaterialInterface* MaterialInstance = nullptr;

    UPROPERTY(EditAnywhere)
    FName Type;

    bool operator == (const FVisualPropertyType& Other) const
    {
        return Other.Type == Type;
    }
    friend uint32 GetTypeHash (const FVisualPropertyType& Other)
    {
        return GetTypeHash(Other.Type);
    }

    FVisualPropertyType(const FName& Name) : MaterialInstance(nullptr), Type(Name){};
    FVisualPropertyType() : MaterialInstance(nullptr), Type(){};
};


USTRUCT(BlueprintType)
struct FVisualProperty : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FName Type = "Null";
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FName Tag;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FColor Color = FColor::Black;
};
