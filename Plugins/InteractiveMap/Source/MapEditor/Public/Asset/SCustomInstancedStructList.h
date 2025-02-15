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
#include "SlateWidgets/InstancedStructList.h"

/**
 * Generic row for a generic list. Pass in structure type.
 */
class SCustomInstancedStructListRow : public SInstancedStructListRow
{
public:

    SLATE_BEGIN_ARGS(SCustomInstancedStructListRow) {}
    SLATE_ARGUMENT(TSet<FName>, PropertiesNotEditable)
    SLATE_ARGUMENT(TSharedPtr<FInstancedStruct>, Item)
    SLATE_EVENT(FItemChangedSignature, OnItemChanged)
    SLATE_END_ARGS()

public:

    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
    {
        NotEditableProperties = InArgs._PropertiesNotEditable;
        SInstancedStructListRow::Construct(
            SInstancedStructListRow::FArguments().Item(InArgs._Item).OnItemChanged(InArgs._OnItemChanged),
            InOwnerTableView);
    }

    /**
     * Generate a widget for the column name. 
     * Will try to convert a property to a string and return STextBlock within an SBox.
     * Override if you want to special case some columns or overhaul the widgets returned.
     */
    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
    {
        // iterate struct properties and generate a widget for it
        for (TFieldIterator<FProperty> It(Item->GetScriptStruct()); It; ++It)
        {
            const FProperty* Property = *It;
            const FName PropertyName = Property->GetFName();
            if (ColumnName != PropertyName)
                continue;
            else if(IsPropertyEditable(PropertyName))
            {
                return SNew(SBox)
                    .Padding(FMargin(4.0f, 0.0f))
                    .VAlign(VAlign_Center)
                    [
                        SNew(SEditableTextCustom)
                        .OnPropertyEdited_Lambda([this](const FName& Name, const FText& Text)
                        {
                            if(UADStructUtilsFunctionLibrary::SetPropertyValueNestedInStructFromString(*Item, Name.ToString(), Text.ToString()))
                            {
                                ItemUpdateDelegate.ExecuteIfBound(*Item);
                            }
                        })
                        .Text(this, &SInstancedStructListRow::GetPropertyValueText, Property)
                        .Name(PropertyName)
                        
                    ];
            }
            else
            {
                return SNew(SBox)
                    .Padding(FMargin(4.0f, 0.0f))
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(this, &SInstancedStructListRow::GetPropertyValueText, Property)
                    ];
            }
        }

        // default to null widget if property cannot be found
        return SNullWidget::NullWidget;
    }

    bool IsPropertyEditable(const FName& PropertyName) const 
    {
        return !NotEditableProperties.Contains(PropertyName);
    }
    
protected:
    TSet<FName> NotEditableProperties;
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
    // called when a struct is edited by the user
    SLATE_EVENT(FItemChangedSignature, OnItemChanged)
    SLATE_END_ARGS()

    SCustomInstancedStructList() : SInstancedStructList()
    {}

    void Construct(const FArguments& InArgs)
    {
        NotEditableProperties = *(InArgs._NotEditableProperties);
        SInstancedStructList::Construct(
            SInstancedStructList::FArguments()
            .ItemHeight(InArgs._ItemHeight)
            .ListSource(InArgs._ListSource)
            .StructTypes(InArgs._StructTypes)
            .OnItemChanged(InArgs._OnItemChanged));
    }

    virtual TSharedRef<class ITableRow> OnGenerateRow(TSharedPtr<FInstancedStruct> Item, const TSharedRef<STableViewBase >& OwnerTable) override
    {
        return SNew(SCustomInstancedStructListRow, OwnerTable)
            .Item(Item)
            .OnItemChanged(ItemUpdateDelegate)
            .PropertiesNotEditable(NotEditableProperties);
    }

protected:
    TSet<FName> NotEditableProperties;
};
