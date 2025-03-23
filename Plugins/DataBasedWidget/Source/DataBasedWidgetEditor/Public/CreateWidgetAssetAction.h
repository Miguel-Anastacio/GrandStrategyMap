// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "GenericWidget/GenericStructWidget.h"
#include "CreateWidgetAssetAction.generated.h"
UCLASS()
class DATABASEDWIDGETEDITOR_API UCreateWidgetFromAssetAction  : public UAssetActionUtility
{
    GENERATED_BODY()
    UCreateWidgetFromAssetAction();
public:
    
    UFUNCTION(CallInEditor, Category=CreateWidgetFromAsset)
    virtual void CreateWidgetFromObject(
        UPARAM(meta=(MustImplement="UserObjectListEntry")) TSubclassOf<UUserWidget> DefaultWidgetForFields = nullptr) const ;

protected:    
    
    static class UWidgetMapDataAsset* CreateWidgetMapDataAsset(const FString& PackagePath, const FString& ObjectOriginName);
    UBlueprint* CreateBlueprintDerivedFromGenericStructWidget(const FString& PackagePath, const FString& AssetName, UWidgetMapDataAsset* MapDataAsset) const;

    static UStruct* GetAssetStruct(const UObject* Asset);
    
    
};
