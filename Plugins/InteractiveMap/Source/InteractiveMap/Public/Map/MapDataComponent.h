// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Map/MapUtils.h"
#include "Map/MapEnums.h"
#include "MapDataComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTERACTIVEMAP_API UMapDataComponent : public UActorComponent
{
	GENERATED_BODY()

	UMapDataComponent();

protected:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	TMap<FVector, FName> GetLookUpTable() const;

	FORCEINLINE TMap<FName, FProvinceData>* GetProvinceDataMap() { return &ProvinceDataMap; };
	FORCEINLINE TMap<FName, FCountryData>* GetCountryDataMap() { return &CountryDataMap; };
	FORCEINLINE TMap<FName, FColoredData>* GetVisualPropertiesDataMap() { return &VisualPropertiesDataMap; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	FName GetProvinceID(const FVector& color, bool out_result) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	void GetProvinceData(FName name, FProvinceData& out_data) const;
	FProvinceData* GetProvinceData(FName name);

	// Get data from tag/id
	template<class T>
	const T* GetDataFromID(FName tag, const TMap<FName, T>& mapToSearch)
	{
		const T* data = mapToSearch.Find(tag);

		return data;
	}

	// Update Data
	// returns true if the the map texture needs to be updated
	UFUNCTION(BlueprintCallable, Category = "Map Data")
	bool UpdateProvinceData(const FProvinceData& data, FName id, MapMode& out_mapToUpdate, FColor& out_newColor);
	UFUNCTION(BlueprintCallable, Category = "Map Data")
	bool UpdateCountryData(const FCountryData& data, FName id);

	void CreateLookUpTable();
	void ReadDataTables();
	void SetCountryProvinces();

	bool GetCountryColor(const FVector& color, FColor& out_countryColor) const;
	FColor GetCountryColor(const FProvinceData* data) const;

	FColor GetReligionColor(const FProvinceData* data) const;
	FColor GetCultureColor(const FProvinceData* data) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* MapDataTable;
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* ProvinceDataTable;
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* CountryDataTable;
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* VisualPropertiesDataTable;

	// Data Populated by the data tables
	UPROPERTY(BlueprintReadOnly, Category = "Map Data")
	TMap<FVector, FName> LookUpTable;
	UPROPERTY(BlueprintReadOnly, Category = "Map Data")
	TMap<FName, FProvinceData> ProvinceDataMap;
	UPROPERTY(BlueprintReadOnly, Category = "Map Data")
	TMap<FName, FCountryData> CountryDataMap;

	// Province properties that have a map color associated
	UPROPERTY(BlueprintReadOnly, Category = "Map Data")
	TMap<FName, FColoredData> VisualPropertiesDataMap;


	friend class AInteractiveMap;
};
