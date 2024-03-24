// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
    //UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    //FName Type;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FName DataName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FColor Color;

    FColoredData() : DataName(FString("")), Color(FColor()) {};

    FColoredData(const FString& name, FColor color) :
        DataName(name), Color(color) {};

};

//USTRUCT(BlueprintType)
//struct FReligion : public FColoredData
//{
//    GENERATED_BODY()
//
//    FReligion() : FColoredData() {};
//
//    FReligion(const FString& name, FColor color) :
//        FColoredData(name, color) {};
//
//    // Add more religion data here
//};
//USTRUCT(BlueprintType)
//struct FPopCulture : public FColoredData
//{
//    GENERATED_BODY()
//
//    FPopCulture() : FColoredData() {};
//
//    FPopCulture(const FString& name, FColor color) :
//        FColoredData(name, color) {};
//
//    // Add more culture data here
//};

USTRUCT(BlueprintType)
struct FProvinceIDData : public FTableRowBase
{
    GENERATED_BODY()
    FProvinceIDData() {};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FString Name;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FString Color;
    //TMap<TCHAR, int32> HexMap;
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
    /*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FReligion Religion;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FPopCulture Culture;*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FName Religion;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FName Culture;
    FProvinceData() : ProvinceName(FString("")), Owner(FName()), Population(0), Religion(FName()) {};

    FProvinceData(const FString& name, FName owner, int64 population, FName religion, FName culture) :
        ProvinceName(name), Owner(owner), Population(population), Religion(religion), Culture(culture) {};

    void PrintProvinceData();
    // Add more province data here
};

USTRUCT(BlueprintType)
struct FCountryData : public FTableRowBase
{
    GENERATED_BODY()
    FCountryData() {};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FString CountryName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    FColor Color;
    //UPROPERTY(BlueprintReadWrite, Category = "ID")
    //int64 Population;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ID")
    TArray<int32> Provinces;

    FCountryData(const FString& name, const FColor& color) : CountryName(name), Color(color)
    {};
    // Add more country data here
};
