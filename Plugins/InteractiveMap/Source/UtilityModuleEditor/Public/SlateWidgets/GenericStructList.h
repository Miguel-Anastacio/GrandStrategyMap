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

/**
 * Generic row for a generic list. Pass in structure type.
 */
template<typename StructType>
class SGenericStructListRow : public SMultiColumnTableRow<TSharedPtr<FString>>
{
public:

    SLATE_BEGIN_ARGS(SGenericStructListRow) {}
    SLATE_ARGUMENT(TSharedPtr<StructType>, Item)
    SLATE_END_ARGS()

public:

    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
    {
        Item = InArgs._Item;
        SMultiColumnTableRow<TSharedPtr<FString> >::Construct(FSuperRowType::FArguments(), InOwnerTableView);
    }

    /**
     * Convert a property pointer use the item to conver the property to a string.
     */
    virtual FText GetPropertyValueText(const FProperty* Property) const
    {
        FString PropertyValueStr;
        void* Data = (void*)Item.Get();
        const uint8* PropertyAddr = Property->ContainerPtrToValuePtr<uint8>(Data);
        Property->ExportTextItem_InContainer(PropertyValueStr, PropertyAddr, nullptr, nullptr, PPF_None);
        return FText::FromString(PropertyValueStr);
    }

    /**
     * Generate a widget for the column name. 
     * Will try to convert a property to a string and return STextBlock within an SBox.
     * Override if you want to special case some columns or overhaul the widgets returned.
     */
    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
    {
        // iterate struct properties and generate a widget for it
        for (TFieldIterator<FProperty> It(StructType::StaticStruct()); It; ++It)
        {
            const FProperty* Property = *It;
            const FName PropertyName = Property->GetFName();
            if (ColumnName == PropertyName)
            {
                return SNew(SBox)
                    .Padding(FMargin(4.0f, 0.0f))
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(this, &SGenericStructListRow::GetPropertyValueText, Property)
                    ];
            }
        }

        // default to null widget if property cannot be found
        return SNullWidget::NullWidget;
    }

protected:
    TSharedPtr<StructType> Item;
};

/**
 * Generic list templated on a structure to view.
 * Second template parameter is the row class to use which default to the above.
 */
template<typename StructType, class RowClass = SGenericStructListRow<StructType>>
class SGenericStructList : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SGenericStructList<StructType>)
        : _ListSource(nullptr)
        , _ColumnsSource(nullptr)
        , _ItemHeight(20.0f)
    {}
    SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_ARGUMENT(const TArray<TSharedPtr<StructType>>*, ListSource)
    SLATE_ARGUMENT(const TArray<SHeaderRow::FColumn>*, ColumnsSource)
    SLATE_ARGUMENT(float, ItemHeight)
    SLATE_END_ARGS()

    SGenericStructList() 
        : List() 
        , ListView(nullptr)
    {}

    void Construct(const FArguments& InArgs)
    {
        if (InArgs._ListSource != nullptr)
        {
            List = *(InArgs._ListSource);
        }

        // create header
        // if pointer to array of columns is provided then copy the columns
        HeaderRow = SNew(SHeaderRow);
        if (InArgs._ColumnsSource != nullptr)
        {
            for (auto It = InArgs._ColumnsSource->CreateConstIterator(); It; ++It)
            {
                SHeaderRow::FColumn Column = *It;
                HeaderRow->AddColumn(Column);
            }
        }
        else
        {
            //if no column array is provided then generate from property names
            for (TFieldIterator<FProperty> It(StructType::StaticStruct()); It; ++It)
            {
                const FProperty* Property = *It;
                const FName PropertyName = Property->GetFName();
                HeaderRow->AddColumn(SHeaderRow::Column(PropertyName).DefaultLabel(FText::FromName(PropertyName)));
            }
        }
      
        ListView = SNew(SListView<TSharedPtr<StructType>>)
                    .ItemHeight(20.0f)
                    .ListItemsSource(&List)
                    .OnGenerateRow(this, &SGenericStructList<StructType, RowClass>::OnGenerateRow)
                    .OnSelectionChanged(this, &SGenericStructList<StructType, RowClass>::HandleSelectionChanged)
                    .OnMouseButtonDoubleClick(this, &SGenericStructList<StructType, RowClass>::HandleMouseButtonDoubleClick)
                    .OnContextMenuOpening(this, &SGenericStructList<StructType, RowClass>::HandleContextMenuOpening)
                    .SelectionMode(ESelectionMode::Single)
                    .HeaderRow(HeaderRow);
        
        // construct the list view
        ChildSlot.AttachWidget(ListView.ToSharedRef());
    }

    virtual TSharedRef<class ITableRow> OnGenerateRow(TSharedPtr<StructType> Item, const TSharedRef<STableViewBase >& OwnerTable)
    {
        return SNew(RowClass, OwnerTable).Item(Item);
    }
    virtual void HandleSelectionChanged(TSharedPtr<StructType> Selection, ESelectInfo::Type SelectInfo)
    {
        // NO OP
    }
    virtual void HandleMouseButtonDoubleClick(TSharedPtr<StructType> Item)
    {
        // NO OP
    }
    virtual TSharedPtr<SWidget> HandleContextMenuOpening()
    {
        // NO OP

        return SNullWidget::NullWidget;
    }

protected:
    TArray<TSharedPtr<StructType>> List;
    TSharedPtr<SListView<TSharedPtr<StructType>>> ListView;
    TSharedPtr<SHeaderRow> HeaderRow;
};
