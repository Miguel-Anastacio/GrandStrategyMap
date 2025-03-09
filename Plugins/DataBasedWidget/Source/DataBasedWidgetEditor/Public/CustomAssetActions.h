// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "CustomAssetActions.generated.h"
UCLASS()
class UMyAssetAction  : public UAssetActionUtility
{
    GENERATED_BODY()
    UMyAssetAction();
public:

    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> DefaultWidgetForFields;
    // Specify the asset class you are extending
    UFUNCTION(CallInEditor)
    void CreateWidgetFromObject() const ;

    static class UWidgetMapDataAsset* CreateWidgetMapDataAsset(const FString& PackagePath, const FString& ObjectOriginName);
    static class UGenericStructWidget* CreateWidgetFromObject(const FString& PackagePath, const FString& ObjectOriginName);
    static UBlueprint* CreateBlueprintDerivedFromGenericStructWidget(const FString& PackagePath, const FString& AssetName, UWidgetMapDataAsset* MapDataAsset);
    static void SetupBlueprintAfterCreation(UBlueprint* CreatedBlueprint, UWidgetMapDataAsset* WidgetMap);
};
