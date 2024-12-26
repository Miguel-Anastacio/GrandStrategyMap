// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "UObject/Object.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "MapObject.generated.h"

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
		this->FilePath = filePath;
	}
	FString GetFilePath() const
	{
		return this->FilePath;
	}

	void ClearMapData()
	{
		MapData.Empty();
		StructType = nullptr;
	}

private:
	UPROPERTY(VisibleAnywhere)
	TArray<FInstancedStruct> MapData;
	UPROPERTY(VisibleAnywhere)
	FString FilePath;
};
