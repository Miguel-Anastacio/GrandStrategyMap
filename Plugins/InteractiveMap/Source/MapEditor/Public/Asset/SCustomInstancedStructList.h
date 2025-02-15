#pragma once

#include "CoreMinimal.h"
#include "Layout/Margin.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/SWidget.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/SListView.h"
#include "InstancedStruct.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "SlateWidgets/DropDownSelectorWidget.h"
#include "SlateWidgets/InstancedStructList.h"

/**
 * Generic row for a generic list. Pass in structure type.
 */
class SCustomInstancedStructListRow : public SInstancedStructListRow
{
public:

    SLATE_BEGIN_ARGS(SCustomInstancedStructListRow) {}
    SLATE_ARGUMENT(const TSet<FName>*, PropertiesNotEditable)
    SLATE_ARGUMENT(const TSet<FName>*, PropertiesWithDropDown)
    SLATE_ARGUMENT(TSharedPtr<FInstancedStruct>, Item)
    SLATE_ARGUMENT(TWeakObjectPtr<UMapObject>, MapObject)
    SLATE_EVENT(FItemChangedSignature, OnItemChanged)
    SLATE_END_ARGS()

public:
     virtual TSharedRef<SWidget> DisplayDropdownProperty(const FProperty* Property)
    {
         const TArray<FName> AvailableTagsOfType = MapObject->GetVisualPropertiesNamesOfType(Property->GetFName());
         return SNew(SBox)
         .Padding(FMargin(4.0f, 0.0f))
         .VAlign(VAlign_Center)
         [
             SNew(SDropDownSelectorWidget)
             .AvailableTags(AvailableTagsOfType)
             .OnTagChanged_Lambda([this](const FName& Name, const FName& NewTag)
             {
                 if(UADStructUtilsFunctionLibrary::SetPropertyValueNestedInStructFromString(*Item, Name.ToString(), NewTag.ToString()))
                 {
                     ItemChanged.ExecuteIfBound(*Item);
                 }
             })
             .Text(this, &SInstancedStructListRow::GetPropertyValueText, Property)
             .PropertyName(Property->GetFName())
         ];
    }

    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
    {
        if(InArgs._PropertiesNotEditable)
        {
            NotEditableProperties = *(InArgs._PropertiesNotEditable);
        }
        if(InArgs._PropertiesWithDropDown)
        {
            PropertiesWithDropDown = *(InArgs._PropertiesWithDropDown);
        }
        MapObject = InArgs._MapObject;
        SInstancedStructListRow::Construct(
        SInstancedStructListRow::FArguments().Item(InArgs._Item).OnItemChanged(InArgs._OnItemChanged),
        InOwnerTableView);
    }

    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
    {
        // iterate struct properties and generate a widget for it
        for (TFieldIterator<FProperty> It(Item->GetScriptStruct()); It; ++It)
        {
            const FProperty* Property = *It;
            const FName PropertyName = Property->GetFName();
            if (ColumnName != PropertyName)
                continue;
            else if(IsPropertyDropDown(PropertyName))
            {
                return DisplayDropdownProperty(Property);
            }
            else if(IsPropertyEditable(PropertyName))
            {
                return DisplayEditableProperty(Property);
            }
            else 
            {
               return DisplayNotEditableProperty(Property);
            }
        }

        // default to null widget if property cannot be found
        return SNullWidget::NullWidget;
    }

    bool IsPropertyEditable(const FName& PropertyName) const 
    {
         // if(NotEditableProperties)
         //     return true;
        return !NotEditableProperties.Contains(PropertyName);
    }

    bool IsPropertyDropDown(const FName& PropertyName) const 
     {
         // // if(!PropertiesWithDropDown)
         // return false;
         return PropertiesWithDropDown.Contains(PropertyName);
     }
    
protected:
    TSet<FName> NotEditableProperties;
    TSet<FName> PropertiesWithDropDown;
    TWeakObjectPtr<UMapObject> MapObject;
};

/**
 * Display a collection of InstancedStructs as a list a structure to view.
 * Allows the structs to be edited, and some properties can be marked as not editable
 */
class SCustomInstancedStructList : public SInstancedStructList
{
public:
    SLATE_BEGIN_ARGS(SCustomInstancedStructList)
        : _ListSource(nullptr)
        , _StructTypes()
        , _ItemHeight(20.0f)
    {}
    SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_ARGUMENT(const TArray<TSharedPtr<FInstancedStruct>>*, ListSource)
    SLATE_ARGUMENT(const TArray<UScriptStruct*>*, StructTypes)
    SLATE_ARGUMENT(float, ItemHeight)
    SLATE_ARGUMENT(const TSet<FName>*, NotEditableProperties)
    SLATE_ARGUMENT(const TSet<FName>*, PropertiesWithDropdown)
    SLATE_ARGUMENT(TWeakObjectPtr<UMapObject>, MapObject)
        
    // called when a struct is edited by the user
    SLATE_EVENT(FItemChangedSignature, OnItemChanged)
    SLATE_END_ARGS()

    SCustomInstancedStructList() : SInstancedStructList()
    {}

    void Construct(const FArguments& InArgs)
    {
        NotEditableProperties = *(InArgs._NotEditableProperties);
        PropertiesWithDropDown = *(InArgs._PropertiesWithDropdown);
        MapObject = InArgs._MapObject;
        SInstancedStructList::Construct(
            SInstancedStructList::FArguments()
            .ItemHeight(InArgs._ItemHeight)
            .ListSource(InArgs._ListSource)
            .StructTypes(InArgs._StructTypes)
            .OnItemChanged(InArgs._OnItemChanged));
    }
    // override if you wish to change the SWidget used for the Row
    virtual TSharedRef<class ITableRow> OnGenerateRow(TSharedPtr<FInstancedStruct> Item, const TSharedRef<STableViewBase >& OwnerTable) override
    {
        return SNew(SCustomInstancedStructListRow, OwnerTable)
            .Item(Item)
            .OnItemChanged(ItemUpdateDelegate)
            .PropertiesNotEditable(&NotEditableProperties)
            .PropertiesWithDropDown(&PropertiesWithDropDown)
            .MapObject(MapObject);
    }

protected:
    TSet<FName> NotEditableProperties;
    TSet<FName> PropertiesWithDropDown;
    TWeakObjectPtr<UMapObject> MapObject;
};
