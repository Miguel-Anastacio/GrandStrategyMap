// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "VisualProperties.h"
#include "UObject/Object.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "Misc/Paths.h"

#include "MapObject.generated.h"
USTRUCT(BlueprintType)
struct FBaseMapStruct
{
	GENERATED_BODY()

	virtual ~FBaseMapStruct(){};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	int32 ID = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name = "NO NAME";
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Area = "NO AREA";
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Region = "NO REGION";

	virtual  FString ToString() const
	{
		return FString::Printf(TEXT("ID: %d - Name: %s - Area: %s - Region - %s"), ID, *Name, *Area, *Region);
	}
};


USTRUCT(BlueprintType)
struct FLookupEntry
{
	GENERATED_BODY()
    
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Color;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name;

	FString ToString() const
	{
		return FString::Printf(TEXT("ColorInHex: %s, Name: %s"), *Color, *Name);
	}
};

USTRUCT(BlueprintType)
struct FExampleStruct : public FBaseMapStruct
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Population = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Owner = "ProvinceOwner";
};

USTRUCT(BlueprintType)
struct FMapDataStruct : public FBaseMapStruct
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Population = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Country = "POR";
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Religion = "CAT";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	FString Culture = "BAS";
	
	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("ID: %d - Name: %s, Population %d, Owner: %s"), ID, *Name, Population, *Country);
	}
};

DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);

UCLASS()
class SHAREDMODULE_API UMapObject : public UObject
{
	GENERATED_BODY()

	// UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
	virtual void PostLoad() override;
	// ======================================================
public:	
	void LogLookupTable() const ;
	void LogMapData() const;
	void LogVisualProperties() const;

	bool IsTileWater(int ID) const;
	bool IsTileLand(int ID) const;
	void UpdateTile(int Index, const FInstancedStruct& NewData);
	void UpdateTileProperty(int Index, const FString& PropertyName, const FString& NewValue);
	void SaveData() const;
	void LoadDataFromFile();

#if WITH_EDITOR
	void SetLookupTexture(UTexture2D* Texture2D);
	void SetMapDataFilePath(const FString& FilePath, bool LoadFromFile = true);
	void SetLookupFilePath(const FString& FilePath)
	{
		this->LookupFilePath = FPaths::CreateStandardFilename(FilePath);
		LoadLookupMap(FilePath);
	}
#endif
	void LoadLookupMap(const FString& FilePath);
	
	TMap<int32, FInstancedStruct>& GetMapData()
	{
		return MapData;
	}
	
	TArray<FInstancedStruct> GetMapDataValue() const
	{
		TArray<FInstancedStruct> OutArray;
		MapData.GenerateValueArray(OutArray);
        return OutArray;
	}
	
	FString GetFilePath() const
	{
		return this->FilePathMapData;
	}
	int GetIndexOfTileSelected(const FColor& Color);

	FColor GetColorFromUv(const FVector2D& Uv) const;
	
	TMap<FColor, int> GetLookupTable() const
	{
		return LookupTable;
	};

	const TArray<uint8>& GetLookupTextureData() const
	{
		return LookupTextureData;
	}
#if WITH_EDITOR
	void UpdateDataInEditor(const FInstancedStruct& NewData);
#endif
	
public:
	
#if WITH_EDITORONLY_DATA
	/** Data table for visual property types */
	UPROPERTY(EditAnywhere, Category = "Data", DisplayName= "Visual Property Types")
	UDataTable* VisualPropertyTypesDT;

	/** Data table for visual properties */
	UPROPERTY(EditAnywhere, Category = "Data", DisplayName="Visual Properties")
	class UDataTable* VisualPropertiesDT;
#endif
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	UScriptStruct* StructType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	UScriptStruct* OceanStructType;
	
	FOnAssetChanged OnObjectChanged;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lookup")
	class UTexture2D* LookupTexture;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lookup")
	class UMaterialInterface* MaterialOverride;

	const TMap<FVisualPropertyType, FArrayOfVisualProperties>& GetVisualPropertiesMap() const;
	TMap<FName, FArrayOfVisualProperties> GetVisualPropertyNameMap() const;

	FColor GetPropertyColorFromInstancedStruct(const FInstancedStruct& InstancedStruct, const FName& PropertyName, bool& OutResult) const;
	void ReadDataTables();
	FVisualProperty GetVisualProperty(const FName& Type, const FName& Tag, bool& OutResult) const;
	FVisualProperty GetVisualProperty(const FVisualPropertyType& Type, const FName& Tag, bool& OutResult) const;
    
protected:

	// TODO - MAYBE REMOVE THIS
	UPROPERTY()
	class UStaticMesh* Mesh;
	
private:
	bool IsTileOfType(int ID, const UScriptStruct* ScriptStruct) const;
	
	UPROPERTY()
	TMap<FVisualPropertyType, FArrayOfVisualProperties> VisualPropertiesMap;
	
	UPROPERTY()
	TMap<int32, FInstancedStruct> MapData;

	UPROPERTY()
	TMap<FColor, int> LookupTable;
	
	UPROPERTY(VisibleAnywhere, Category = "Lookup")
	FString LookupFilePath;

	UPROPERTY()
	TArray<uint8> LookupTextureData;
	
	UPROPERTY(VisibleAnywhere, Category="Data")
	FString FilePathMapData;
};
