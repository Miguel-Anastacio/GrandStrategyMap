// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "MapObject.h"
#include "Components/ActorComponent.h"
#include "Map/MapUtils.h"
#include "Map/MapEnums.h"
#include "MapDataComponent.generated.h"


/**
 * Component for managing map data.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTERACTIVEMAP_API UMapDataComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    UMapDataComponent();

protected:
    /** Gets the lookup table. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
    TMap<FColor, int> GetLookUpTable() const;

    /** Gets the province data map. */
    FORCEINLINE TMap<int, FInstancedStruct>* GetProvinceDataMap() { return &ProvinceDataMap; };

    /** Gets the country data map. */
    FORCEINLINE TMap<FName, FCountryData>* GetCountryDataMap() { return &CountryDataMap; };

    /** Gets the visual properties data map. */
    FORCEINLINE TMap<FName, FColoredData>* GetVisualPropertiesDataMap() { return &VisualPropertiesDataMap; };

    /** Gets the province ID. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
    int GetProvinceID(const FColor& Color, bool& bOutResult) const;

    /** Gets province data by name. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
    void GetProvinceData(int name, FInstancedStruct& out_data) const;

    /** Gets province data by name. */
    FInstancedStruct* GetProvinceData(int ID);

    /** Gets data from ID/tag. */
    template<class T>
    const T* GetDataFromID(FName tag, const TMap<FName, T>& mapToSearch)
    {
        const T* data = mapToSearch.Find(tag);
        return data;
    }

     /**
     * Updates province data.
     *
     * @param data The new data to update.
     * @param id The ID of the province.
     * @param out_mapToUpdate Reference to the map mode to update.
     * @param out_newColor Reference to the new color.
     * @return True if the update is successful, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category = "Map Data")
    bool UpdateProvinceData(const FProvinceData& data, int ID, MapMode& out_mapToUpdate, FColor& out_newColor);

    /** Updates country data. */
    UFUNCTION(BlueprintCallable, Category = "Map Data")
    bool UpdateCountryData(const FCountryData& data, FName id);

    /** Creates the lookup table. */
    void CreateLookUpTable();

    /** Reads data tables. */
    void ReadDataTables();

    void SetProvinceDataMap(const TArray<FInstancedStruct>& Data);

    /** Sets country provinces. */
    void SetCountryProvinces();

    /** Gets country color. */
    bool GetCountryColor(const FColor& Color, FColor& out_countryColor) const;

    /** Gets country color. */
    FColor GetCountryColor(const FProvinceData* data) const;

    /** Gets religion color. */
    FColor GetReligionColor(const FProvinceData* data) const;

    /** Gets culture color. */
    FColor GetCultureColor(const FProvinceData* data) const;

    void LoadFromMapObject(const UMapObject* MapObject);

    int* FindId(const FColor& Color);
protected:
    // TODO: REMOVE
    /** Data table for the map. */
    UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* MapDataTable;

    /** Data table for provinces.  TODO: REMOVE */
    UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* ProvinceDataTable;

    /** Data table for countries. */
    UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* CountryDataTable;

    /** Data table for visual properties. */
    UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* VisualPropertiesDataTable;

    /** Lookup table. */
    UPROPERTY(BlueprintReadOnly, Category = "Map Data")
    TMap<FVector, FName> LookUpTable;
    
    /** Province data map. */
    UPROPERTY(BlueprintReadOnly, Category = "Map Data")
    TMap<int, FInstancedStruct> ProvinceDataMap;

    /** Country data map. */
    UPROPERTY(BlueprintReadOnly, Category = "Map Data")
    TMap<FName, FCountryData> CountryDataMap;

    /** Visual properties data map. */
    UPROPERTY(BlueprintReadOnly, Category = "Map Data")
    TMap<FName, FColoredData> VisualPropertiesDataMap;

    friend class AClickableMap;
    //// ------------------------------------------------------------------------
    /** New Lookup table. */
    UPROPERTY(BlueprintReadOnly, Category = "Map Data")
    TMap<FColor, int> NewLookupTable;

    

    
    
};
