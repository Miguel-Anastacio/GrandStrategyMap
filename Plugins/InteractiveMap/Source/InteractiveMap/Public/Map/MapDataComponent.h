// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "MapObject.h"
#include "Components/ActorComponent.h"
#include "Map/MapUtils.h"
#include "Map/MapEnums.h"
#include "MapDataComponent.generated.h"

USTRUCT(BlueprintType)
struct FArrayOfVisualProperties
{
    GENERATED_BODY()
    UPROPERTY()
    TArray<FVisualProperty> VisualProperties;
};
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

    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

    const TMap<FVisualPropertyType, FArrayOfVisualProperties>& GetVisualPropertiesMap() const;
    TMap<FName, FArrayOfVisualProperties> GetVisualPropertyNameMap() const;
    

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
    void GetProvinceData(int Name, FInstancedStruct& Out_Data) const;
    
    // UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
    // FInstancedStruct GetProvinceDataBP(int ID, bool& OutResult);

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

    /** Reads data tables. */
    void ReadDataTables();

    void SetProvinceDataMap(const TArray<FInstancedStruct>& Data);

    /** Sets country provinces. */
    void SetCountryProvinces();

    void LoadFromMapObject(const UMapObject* MapObject);

    int* FindId(const FColor& Color);

    FVisualProperty GetVisualProperty(const FName& Type, const FName& Tag, bool& OutResult) const;
    FVisualProperty GetVisualProperty(const FVisualPropertyType& Type, const FName& Tag, bool& OutResult) const;
    
protected:
#if WITH_EDITORONLY_DATA
    /** Data table for visual property types */
    UPROPERTY(EditAnywhere, Category = "Data", DisplayName= "Visual Property Types")
    UDataTable* VisualPropertyTypesDT;

    /** Data table for visual properties */
    UPROPERTY(EditAnywhere, Category = "Data", DisplayName="Visual Properties")
    UDataTable* VisualPropertiesDT;
#endif
    UPROPERTY()
    TMap<FVisualPropertyType, FArrayOfVisualProperties> VisualPropertiesMap;

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
