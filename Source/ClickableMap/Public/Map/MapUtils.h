// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapUtils.generated.h"

USTRUCT(BlueprintType)
struct FProvinceIDData : public FTableRowBase
{
    GENERATED_BODY()
    FProvinceIDData() 
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

    // This is the tooltip for our test variable.
    UPROPERTY(BlueprintReadWrite, Category = "ID")
    FString Name;
    UPROPERTY(BlueprintReadWrite, Category = "ID")
    FString Country;
    UPROPERTY(BlueprintReadWrite, Category = "ID")
    FString Color;

    TMap<TCHAR, int32> HexMap;
    // assuming format AABBCCTT (AA = R, BB = G, CC = B, TT = Alpha)
    int32 HexToDecimal(const FString& hex);
    FColor ConvertHexStringToRGB(FString color);
};



//class UTextureRenderTarget;
//UCLASS()
//class CLICKABLEMAP_API AInteractiveMap : public AActor
//{
//	GENERATED_BODY()
//	
//public:	
//	// Sets default values for this actor's properties
//	AInteractiveMap();
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//	UFUNCTION(BlueprintCallable, BlueprintPure)
//	UTextureRenderTarget* GetMapRenderTarget() const;
//
//	UFUNCTION(BlueprintCallable, BlueprintPure)
//	TMap<FVector, FString> GetLookUpTable() const;
//
//	UFUNCTION(BlueprintCallable, BlueprintPure)
//	FString GetProvinceID(const FVector& color) const;
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//	void CreateLookUpTable();
//protected:
//	UPROPERTY(EditAnywhere, Category = "Data")
//	UDataTable* MapDataTable;
//
//	UPROPERTY(BlueprintReadOnly)
//	TObjectPtr<UTextureRenderTarget> MapRenderTarget;
//
//	UPROPERTY(BlueprintReadOnly)
//	TMap<FVector, FString> LookUpTable;
//};
