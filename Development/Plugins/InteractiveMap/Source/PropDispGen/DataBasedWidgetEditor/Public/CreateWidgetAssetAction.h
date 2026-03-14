// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "CreateWidgetAssetAction.generated.h"
UCLASS()
class DATABASEDWIDGETEDITOR_API UCreateWidgetFromAssetAction : public UAssetActionUtility
{
    GENERATED_BODY()
    UCreateWidgetFromAssetAction();

public:
    /*
     * @param DefaultWidgetForFields (optional)
     * widget used for each property of UObject (this can be modified after on a per property basis)
     */
    UFUNCTION(CallInEditor, Category = CreateWidgetFromAsset)
    virtual void CreateWidgetFromObject(
        UPARAM(meta = (AllowedClasses = "UserWidget", MustImplement = "/Script/DataBasedWidget.PropGenDataDrivenUserWidgetInterface"))
            TSubclassOf<UUserWidget>
                DefaultWidgetForFields = nullptr) const;

    static class UPropGenWidgetMapDataAsset *CreateWidgetMapDataAsset(const FString &PackagePath, const FString &ObjectOriginName);
    static UBlueprint *CreateBlueprintDerivedFromGenericStructWidget(const FString &PackagePath, const FString &AssetName, UPropGenWidgetMapDataAsset *MapDataAsset);

    static UStruct *GetAssetStruct(const UObject *Asset);
};
