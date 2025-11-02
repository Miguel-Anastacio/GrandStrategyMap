// Copyright 2024 An@stacioDev All rights reserved.
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
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 4, 4)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"

DECLARE_DELEGATE_TwoParams(FPropertyEditedSignature, const FName &, const FText &);
// Custom Editable text with an identifier so that we know which property was changed
class SEditableTextCustom : public SEditableText
{
public:
    SLATE_BEGIN_ARGS(SEditableTextCustom) {}
    SLATE_ARGUMENT(FName, Name)
    SLATE_ATTRIBUTE(FText, Text)
    SLATE_EVENT(FOnTextChanged, OnTextChanged)
    SLATE_EVENT(FOnTextCommitted, OnTextCommitted)
    SLATE_EVENT(FPropertyEditedSignature, OnPropertyEdited) // Custom delegate
    SLATE_END_ARGS()

    FPropertyEditedSignature PropertyEdited;

    void Construct(const FArguments &InArgs)
    {
        Name = InArgs._Name;
        PropertyEdited = InArgs._OnPropertyEdited;
        SEditableText::Construct(
            SEditableText::FArguments()
                .Text(InArgs._Text)
                .OnTextChanged(this, &SEditableTextCustom::HandleTextChanged)
                .OnTextCommitted(this, &SEditableTextCustom::HandleTextCommitted));
    }

protected:
    virtual void HandleTextChanged(const FText &NewText) {};
    virtual void HandleTextCommitted(const FText &NewText, ETextCommit::Type CommitType)
    {
        if (PropertyEdited.IsBound() && CommitType == ETextCommit::Type::OnEnter)
        {
            PropertyEdited.Execute(Name, NewText);
        }
    }

    FName Name = NAME_None;
};

DECLARE_DELEGATE_OneParam(FItemChangedSignature, const FInstancedStruct &);
/**
 * Generic row for a Generic List. Usable for instancedStructs.
 */
class SInstancedStructListRow : public SMultiColumnTableRow<TSharedPtr<FString>>
{
public:
    SLATE_BEGIN_ARGS(SInstancedStructListRow) {}
    SLATE_ARGUMENT(TSharedPtr<FInstancedStruct>, Item)
    SLATE_EVENT(FItemChangedSignature, OnItemChanged)
    SLATE_END_ARGS()

public:
    void Construct(const FArguments &InArgs, const TSharedRef<STableViewBase> &InOwnerTableView)
    {
        Item = InArgs._Item;
        ItemChanged = InArgs._OnItemChanged;
        SMultiColumnTableRow<TSharedPtr<FString>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
    }

    /**
     * Convert a property pointer use the item to convert the property to a string.
     */
    virtual FText GetPropertyValueText(const FProperty *Property) const
    {
        FString PropertyValueStr;
        void *Data = Item.Get()->GetMutableMemory();
        const uint8 *PropertyAddr = Property->ContainerPtrToValuePtr<uint8>(Data);
        Property->ExportTextItem_Direct(PropertyValueStr, PropertyAddr, nullptr, nullptr, PPF_None);
        return FText::FromString(PropertyValueStr);
    }

    virtual TSharedRef<SWidget> DisplayEditableProperty(const FProperty *Property)
    {
        return SNew(SBox)
            .Padding(FMargin(4.0f, 0.0f))
            .VAlign(VAlign_Center)
                [SNew(SEditableTextCustom)
                     .OnPropertyEdited_Lambda([this](const FName &Name, const FText &Text)
                                              {
                   if(UAtkStructUtilsFunctionLibrary::SetPropertyValueNestedInStructFromString(*Item, Name.ToString(), Text.ToString()))
                   {
                       ItemChanged.ExecuteIfBound(*Item);
                   } })
                     .Text(this, &SInstancedStructListRow::GetPropertyValueText, Property)
                     .Name(Property->GetFName())

        ];
    }

    virtual TSharedRef<SWidget> DisplayNotEditableProperty(const FProperty *Property)
    {
        return SNew(SBox)
            .Padding(FMargin(4.0f, 0.0f))
            .VAlign(VAlign_Center)
                [SNew(STextBlock)
                     .Text(this, &SInstancedStructListRow::GetPropertyValueText, Property)];
    }

    /**
     * Generate a widget for the column name.
     * Override if you want to special case some columns or overhaul the widgets returned.
     */
    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName &ColumnName) override
    {
        // iterate struct properties and generate a widget for it
        for (TFieldIterator<FProperty> It(Item->GetScriptStruct()); It; ++It)
        {
            const FProperty *Property = *It;
            const FText PropertyNameText = Property->GetDisplayNameText();
            const FName PropertyName(PropertyNameText.ToString());
            if (ColumnName == PropertyName)
            {
                return DisplayEditableProperty(Property);
            }
        }
        // default to null widget if property cannot be found
        return SNullWidget::NullWidget;
    }

    FItemChangedSignature ItemChanged;

protected:
    TSharedPtr<FInstancedStruct> Item;
};

/**
 * Display a collection of InstancedStructs as a list a structure to view.
 * Allows the structs to be edited
 */
class SInstancedStructList : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SInstancedStructList)
        : _ListSource(nullptr), _StructTypes(), _ItemHeight(20.0f)
    {
    }
    SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_ARGUMENT(TArray<TSharedPtr<FInstancedStruct>> *, ListSource)
    SLATE_ARGUMENT(TArray<UScriptStruct *> *, StructTypes)
    SLATE_ARGUMENT(float, ItemHeight)
    // called when a struct is edited by the user
    SLATE_EVENT(FItemChangedSignature, OnItemChanged)
    SLATE_END_ARGS()

    SInstancedStructList() : List(),
                             ListView(nullptr)
    {
    }

    void Construct(const FArguments &InArgs)
    {
        if (InArgs._ListSource != nullptr)
        {
            List = (InArgs._ListSource);
        }
        ItemUpdateDelegate = InArgs._OnItemChanged;

        // create header
        HeaderRow = SNew(SHeaderRow);
        // Create columns dependent on struct data passed
        if (InArgs._StructTypes != nullptr)
        {
            for (const auto &Struct : *InArgs._StructTypes)
            {
                TArray<const FProperty *> OrderedProperties = UAtkStructUtilsFunctionLibrary::GetOrderedProperties(Struct);
                for (const FProperty *Property : OrderedProperties)
                {
                    const FText PropertyNameText = Property->GetDisplayNameText();
                    const FName PropertyName(PropertyNameText.ToString());
                    if (!HeaderRow->IsColumnVisible(PropertyName))
                    {
                        HeaderRow->AddColumn(SHeaderRow::Column(PropertyName).DefaultLabel(PropertyNameText).ManualWidth(100.0f));
                    }
                }
            }
        }
#if UE_VERSION_NEWER_THAN(5, 4, 4)
        ListView = SNew(SListView<TSharedPtr<FInstancedStruct>>)
               .ListItemsSource(List)
               .OnGenerateRow(this, &SInstancedStructList::OnGenerateRow)
               .OnSelectionChanged(this, &SInstancedStructList::HandleSelectionChanged)
               .OnMouseButtonDoubleClick(this, &SInstancedStructList::HandleMouseButtonDoubleClick)
               .OnContextMenuOpening(this, &SInstancedStructList::HandleContextMenuOpening)
               .SelectionMode(ESelectionMode::Multi)
               .HeaderRow(HeaderRow);
        
#else
        ListView = SNew(SListView<TSharedPtr<FInstancedStruct>>)
                       .ItemHeight(20.0f)
                       .ListItemsSource(List)
                       .OnGenerateRow(this, &SInstancedStructList::OnGenerateRow)
                       .OnSelectionChanged(this, &SInstancedStructList::HandleSelectionChanged)
                       .OnMouseButtonDoubleClick(this, &SInstancedStructList::HandleMouseButtonDoubleClick)
                       .OnContextMenuOpening(this, &SInstancedStructList::HandleContextMenuOpening)
                       .SelectionMode(ESelectionMode::Multi)
                       .HeaderRow(HeaderRow);
#endif
        // construct the list view
        ChildSlot.AttachWidget(ListView.ToSharedRef());
    }

    virtual TSharedRef<class ITableRow> OnGenerateRow(TSharedPtr<FInstancedStruct> Item, const TSharedRef<STableViewBase> &OwnerTable)
    {
        return SNew(SInstancedStructListRow, OwnerTable).Item(Item).OnItemChanged(ItemUpdateDelegate);
    }
    virtual void HandleSelectionChanged(TSharedPtr<FInstancedStruct> Selection, ESelectInfo::Type SelectInfo)
    {
        // NO OP
    }
    virtual void HandleMouseButtonDoubleClick(TSharedPtr<FInstancedStruct> Item)
    {
        // NO OP
    }
    virtual TSharedPtr<SWidget> HandleContextMenuOpening()
    {
        // NO OP
        return SNullWidget::NullWidget;
    }
    virtual void RefreshList()
    {
        if (ListView != nullptr)
        {
            ListView->RequestListRefresh();
        }
    }

    virtual void ClearSelection()
    {
        if (ListView != nullptr)
        {
            ListView->ClearSelection();
        }
    }

    virtual void SetSelection(const TArray<int32>& Indexes)
    {
        // used only for map plugin maybe adapt
        TArray<TSharedPtr<FInstancedStruct>> ItemsSelected;
        for(const int32 Index : Indexes)
        {
            for(const TSharedPtr<FInstancedStruct> Item : *List)
            {
                bool bResult = false;
                if(UAtkStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int>(*Item.Get(), FString("ID"), bResult) == Index)
                {
                    ItemsSelected.Add(Item);
                    break;
                }
            }
        }
        
        ListView->ClearSelection();
        if (ensure(ListView.IsValid()) && ItemsSelected.Num() > 0)
        {
            ListView->SetItemSelection(ItemsSelected, true, ESelectInfo::Type::Direct);
            ListView->RequestScrollIntoView(ItemsSelected.Last());
        }
    }

protected:
    TArray<TSharedPtr<FInstancedStruct>>* List;
    TSharedPtr<SListView<TSharedPtr<FInstancedStruct>>> ListView;
    TSharedPtr<SHeaderRow> HeaderRow;
    FItemChangedSignature ItemUpdateDelegate;
};
