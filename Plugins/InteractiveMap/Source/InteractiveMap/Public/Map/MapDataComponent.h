// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "MapObject.h"
#include "Components/ActorComponent.h"
#include "Map/MapUtils.h"
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
#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    const TMap<FVisualPropertyType, FArrayOfVisualProperties>& GetVisualPropertiesMap() const;
    TMap<FName, FArrayOfVisualProperties> GetVisualPropertyNameMap() const;

protected:
    /** Gets the lookup table. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
    TMap<FColor, int> GetLookUpTable() const;

    /** Gets the province data map. */
    FORCEINLINE TMap<int, FInstancedStruct>* GetProvinceDataMap() { return &ProvinceDataMap; };

    /** Gets the province ID. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
    int GetProvinceID(const FColor& Color, bool& bOutResult) const;

    /** Gets province data by name. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
    bool GetProvinceData(int Name, FInstancedStruct& Out_Data) const;
    
    FColor GetPropertyColorFromInstancedStruct(const FInstancedStruct& InstancedStruct, const FName& PropertyName, bool& OutResult) const;

    /** Gets province data by id. */
    FInstancedStruct* GetProvinceData(int ID);

    /** Reads data tables. */
    void ReadDataTables(const UDataTable* VpDataTable, const UDataTable* VpTypeDataTable);

    void SetProvinceDataMap(const TArray<FInstancedStruct>& Data);
    bool SetProvinceData(const FInstancedStruct& NewData, int ID);

    void LoadFromMapObject(const UMapObject* MapObject);

    int* FindId(const FColor& Color);
    FColor GetColor(int ID) const;

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
    
    /** New Lookup table. */
    UPROPERTY(BlueprintReadOnly, Category = "Map Data")
    TMap<FColor, int> NewLookupTable;
    
    friend class AClickableMap;
};
