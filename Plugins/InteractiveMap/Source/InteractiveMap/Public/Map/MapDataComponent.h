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
    /** Gets the lookup table. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
    TMap<FColor, int32> GetLookUpTable() const;

    /** Gets the province data map. */
    FORCEINLINE TMap<int32, FInstancedStruct>* GetProvinceDataMap() const;

    /** Gets the province ID. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
    int32 GetTileID(const FColor& Color, bool& bOutResult) const;

    /** Gets province data by name. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
    bool GetTileData(int32 ID, FInstancedStruct& Out_Data) const;
    
    /** Gets province data by id. */
    FInstancedStruct* GetTileData(int32 ID) const;

    bool SetTileData(const FInstancedStruct& NewData, int32 ID) const;

    void SetMapObject(UMapObject* MapObject);
    int* FindId(const FColor& Color) const;
    FColor GetColor(int32 ID) const;
    
protected:
    TWeakObjectPtr<class UMapObject> MapObjectRef;
};
