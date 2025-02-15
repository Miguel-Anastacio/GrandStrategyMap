// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "MapObject.h"
#include "Components/ActorComponent.h"
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
#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
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
    

    /** Gets province data by id. */
    FInstancedStruct* GetProvinceData(int ID);

    void SetProvinceDataMap(const TArray<FInstancedStruct>& Data);
    bool SetProvinceData(const FInstancedStruct& NewData, int ID);

    void LoadFromMapObject(const UMapObject* MapObject);

    int* FindId(const FColor& Color);
    FColor GetColor(int ID) const;
    
protected:

    /** Province data map. */
    UPROPERTY(BlueprintReadOnly, Category = "Map Data")
    TMap<int, FInstancedStruct> ProvinceDataMap;
    
    /** New Lookup table. */
    UPROPERTY(BlueprintReadOnly, Category = "Map Data")
    TMap<FColor, int> NewLookupTable;
    
    friend class AClickableMap;
};
