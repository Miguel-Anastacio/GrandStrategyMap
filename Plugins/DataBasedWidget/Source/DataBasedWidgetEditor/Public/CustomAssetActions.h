// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "CustomAssetActions.generated.h"
UCLASS()
class UCreateWidgetFromAssetAction  : public UAssetActionUtility
{
    GENERATED_BODY()
    UCreateWidgetFromAssetAction();
public:
    // Widget used by default for each field of UObject/Struct
    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> DefaultWidgetForFields;
    UFUNCTION(CallInEditor)
    void CreateWidgetFromObject() const ;

protected:    
    static class UWidgetMapDataAsset* CreateWidgetMapDataAsset(const FString& PackagePath, const FString& ObjectOriginName);
    static UBlueprint* CreateBlueprintDerivedFromGenericStructWidget(const FString& PackagePath, const FString& AssetName, UWidgetMapDataAsset* MapDataAsset);

    static UStruct* GetAssetStruct(const UObject* Asset);
    
    
};
