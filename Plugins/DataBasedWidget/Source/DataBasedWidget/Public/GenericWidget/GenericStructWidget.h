// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "Types/SlateEnums.h"
#include "GenericStructWidget.generated.h"

class UPropGenWidgetMapDataAsset;
class UWCustomEditableText;
class UVerticalBox;
class UPropGenWidgetMapDataAsset;
/**
 * A generic widget class that can display structured data in a grid layout
 * This widget can be initialized from either structured data or UObjects
 */
UCLASS(BlueprintType, DisplayName=GenericWidget)
class DATABASEDWIDGET_API UWPropGenGeneric : public UUserWidget, public IUserObjectListEntry
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    
    virtual void NativePreConstruct() override;
    
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    
    /** Returns the UStruct class of the data this widget is displaying */
    const UStruct* GetDataClass() const;
    
#if WITH_EDITOR
    /**
     * Creates widget elements based on a data asset mapping
     * @param DataAssetWidgetMap - Asset containing property-to-widget mappings
     */
    void CreateGenericWidget(UPropGenWidgetMapDataAsset* DataAssetWidgetMap);
#endif

    /**
     * Blueprint-exposed function to initialize the widget from an instanced struct
     * @param InstancedStruct - The struct data to display in this widget
     */
    UFUNCTION(BlueprintCallable, Category = "Generic Struct Widget")
    virtual void InitFromStruct(const FInstancedStruct& InstancedStruct);
    
    /**
     * Blueprint-exposed function to initialize the widget from a UObject
     * @param Object - The object whose properties will be displayed in this widget
     */
    UFUNCTION(BlueprintCallable, Category = "Generic Struct Widget")
    virtual void InitFromObject(const UObject* Object);

    /** Number of columns to use in the grid layout */
    UPROPERTY(EditAnywhere, Category = "Generic Struct Widget")
    int Columns = 1;
    
    /** Grid panel that contains all the property widgets */
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Struct Panel Display")
    class UGridPanel* MainPanel;

#if WITH_EDITORONLY_DATA
    /** Data asset that maps property types to widget classes */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Generic Struct Widget")
    class UPropGenWidgetMapDataAsset* DataAssetWidgetMap;
#endif
    
    /** Map of property names to their corresponding widgets */
    UPROPERTY(BlueprintReadOnly, Category = "Generic Struct Widget")
    TMap<FName, UUserWidget*> WidgetFields;

protected:
#if WITH_EDITOR
    /**
     * Editor function to create panel slots for the property widgets
     * Can be called from the editor to refresh the layout
     */
    UFUNCTION(CallInEditor, Category="Generic Struct Widget")
    void CreatePanelSlots() const;
    
    /** Creates the main grid panel for the widget */
    void CreateMainPanel() const;
#endif

    // IUserObjectListEntry interface implementation
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
    // End of IUserObjectListEntry interface

    /**
     * Updates grid position indices for the next widget
     * @param ColumnIndex - Current column index (will be updated)
     * @param RowIndex - Current row index (will be updated)
     */
    void UpdateGridPosition(uint8& ColumnIndex, uint8& RowIndex) const;
    
    /**
     * Core function that handles widget initialization from data
     * @param ClassType - Type information for the data
     * @param Data - Pointer to the actual data
     */
    void InitFromData(const UStruct* ClassType, const void* Data);
    
private:    
    /** Initializes WidgetFields Map*/
    void InitializeWidgetFields();
};