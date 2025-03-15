// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "GenericStructWidget.h"
#include "CreateWidgetAssetAction.generated.h"
UCLASS()
class UCreateWidgetFromAssetAction  : public UAssetActionUtility
{
    GENERATED_BODY()
    UCreateWidgetFromAssetAction();
public:

    
    UFUNCTION(CallInEditor, Category=CreateWidgetFromAsset)
    void CreateWidgetFromObject() const ;

protected:    
    // Widget used by default for each field of UObject/Struct
    UPROPERTY(EditAnywhere, meta=(MustImplement="/Script/DataBasedWidget.GenericUserWidgetInterface"))
    TSubclassOf<UUserWidget> DefaultWidgetForFields;
    
    // UPROPERTY(EditAnywhere)
    TSubclassOf<UGenericStructWidget> BaseGenericWidget;
    
    static class UWidgetMapDataAsset* CreateWidgetMapDataAsset(const FString& PackagePath, const FString& ObjectOriginName);
    UBlueprint* CreateBlueprintDerivedFromGenericStructWidget(const FString& PackagePath, const FString& AssetName, UWidgetMapDataAsset* MapDataAsset) const;

    static UStruct* GetAssetStruct(const UObject* Asset);
    
    
};
