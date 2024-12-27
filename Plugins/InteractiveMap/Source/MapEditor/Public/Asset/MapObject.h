// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "UObject/Object.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "MapObject.generated.h"

USTRUCT(BlueprintType)
struct FLookupEntry
{
	GENERATED_BODY()
    
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString Color;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString Name;

	FString ToString() const
	{
		return FString::Printf(TEXT("ColorInHex: %s, Name: %s"), *Color, *Name);
	}
};


DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);

UCLASS()
class MAPEDITOR_API UMapObject : public UObject
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	UScriptStruct* StructType;
	FOnAssetChanged OnObjectChanged;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lookup")
	UTexture2D* LookupTexture;
	UPROPERTY(EditAnywhere, Category = "Lookup")
	UStaticMesh* Mesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lookup")
	UMaterialInterface* MaterialOverride;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lookup")
	FString LookupFilePath = "D:\\Dev\\Unreal\\GrandStrategyMap\\Content\\MapEditor\\Maps\\lookup.json";
public:
	void UpdateTile(int Index, const FInstancedStruct& NewData);
	void UpdateTileProperty(int Index, const FString& PropertyName, const FString& NewValue);
	void SaveData() const;
	void LoadDataFromFile();
	void SetMapData(const TArray<FInstancedStruct>& NewData);
	
	TArray<FInstancedStruct>& GetMapData()
	{
		return MapData;
	}
	
	void SetFilePath(const FString& filePath)
	{
		this->FilePathMapData = filePath;
	}
	
	FString GetFilePath() const
	{
		return this->FilePathMapData;
	}

	void ClearMapData()
	{
		MapData.Empty();
		StructType = nullptr;
	}

private:
	UPROPERTY()
	TArray<FInstancedStruct> MapData;

	TMap<FColor, int> LookupTable;
	
	UPROPERTY(VisibleAnywhere, Category="Data")
	FString FilePathMapData;
};
