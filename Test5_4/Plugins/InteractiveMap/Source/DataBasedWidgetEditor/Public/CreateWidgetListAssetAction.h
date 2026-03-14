// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "CollectionViewWidgets/ListViewWidgetsDataTable.h"
#include "CreateWidgetListAssetAction.generated.h"

class UPropGenWidgetMapDataAsset;
UCLASS()
class DATABASEDWIDGETEDITOR_API UCreateWidgetListAssetAction : public UAssetActionUtility
{
    GENERATED_BODY()
    UCreateWidgetListAssetAction();

public:
    /*
     * @param WidgetForListItems (optional) - the widget to be used for each DataTable entry,
     * if you do not specify one it will be created one that is a child of GenericStructWidget
     * @param BaseViewWidget - base class for the Widget that will display all entries of the DT
     */
    UFUNCTION(CallInEditor, Category = CreateWidgetFromAsset)
    virtual void CreateWidgetListFromDataTable(
        TSubclassOf<UUserWidget> WidgetForListItems = nullptr,
        UPARAM(meta = (MustImplement = "/Script/DataBasedWidget.PropGenWidgetCollectionInterface")) TSubclassOf<UUserWidget> BaseViewWidget = nullptr) const;

protected:
    // Widget used by default for each field of UObject/Struct
    UPROPERTY(EditAnywhere, Category = CreateWidgetListFromDataTable, meta = (MustImplement = "/Script/DataBasedWidget.GenericUserWidgetInterface"))
    TSubclassOf<UUserWidget> DefaultWidgetForFields;

    static UPropGenWidgetMapDataAsset *CreateWidgetMapDataAsset(const FString &PackagePath, const FString &ObjectOriginName);
    static UBlueprint *CreateWidgetListBlueprint(const FString &PackagePath, const FString &ObjectOriginName,
                                                 TSubclassOf<UUserWidget> WidgetListBaseClass, TSubclassOf<UUserWidget> WidgetItemClass,
                                                 const UDataTable *ListItems);
    UBlueprint *CreateBlueprintDerivedFromGenericStructWidget(const FString &PackagePath, const FString &AssetName, UPropGenWidgetMapDataAsset *MapDataAsset) const;

    static UStruct *GetAssetStruct(const UObject *Asset);

    static bool ValidateContainerClassAndMemberWidget(const TSubclassOf<UUserWidget> &WidgetListBaseClass, const TSubclassOf<UUserWidget> &WidgetItemClass);
};
