// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "GenericStructWidget.h"
#include "InstancedStruct.h"
#include "CollectionViewWidgets/ListViewWidgetsDataTable.h"
#include "CreateWidgetListAssetAction.generated.h"
UCLASS()
class DATABASEDWIDGETEDITOR_API UCreateWidgetListAssetAction  : public UAssetActionUtility
{
    GENERATED_BODY()
    UCreateWidgetListAssetAction();
public:

    UFUNCTION(CallInEditor, Category=CreateWidgetFromAsset)
    virtual void CreateWidgetListFromDataTable(
        UPARAM(meta=(MustImplement="UserObjectListEntry")) TSubclassOf<UUserWidget> WidgetForListItems = nullptr,
        UPARAM(meta=(AllowedClasses="WPropGenCollectionViewDataTable")) TSubclassOf<UUserWidget> BaseViewWidget = UWPropGenListViewDataTable::StaticClass()) const;
protected:    
    // Widget used by default for each field of UObject/Struct
    UPROPERTY(EditAnywhere, Category=CreateWidgetListFromDataTable, meta=(MustImplement="/Script/DataBasedWidget.GenericUserWidgetInterface"))
    TSubclassOf<UUserWidget> DefaultWidgetForFields;

    static class UWidgetMapDataAsset* CreateWidgetMapDataAsset(const FString& PackagePath, const FString& ObjectOriginName);
    static UBlueprint* CreateWidgetListBlueprint(const FString& PackagePath, const FString& ObjectOriginName,
                                                    TSubclassOf<UUserWidget> WidgetListBaseClass, TSubclassOf<UUserWidget> WidgetItemClass,
                                                   const UDataTable* ListItems);
    UBlueprint* CreateBlueprintDerivedFromGenericStructWidget(const FString& PackagePath, const FString& AssetName, UWidgetMapDataAsset* MapDataAsset) const;

    static UStruct* GetAssetStruct(const UObject* Asset);
    
    
};
