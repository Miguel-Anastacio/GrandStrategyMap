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
#if UE_VERSION_NEWER_THAN(5, 5, 0)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "SlateWidgets/DropDownSelectorWidget.h"
#include "SlateWidgets/InstancedStructList.h"
#include "MapObject.h"

/**
 * Generic row for a generic list. Pass in structure type.
 */
// template <typename  T>
class SCustomInstancedStructListRow : public SInstancedStructListRow
{
public:
    SLATE_BEGIN_ARGS(SCustomInstancedStructListRow) {}
    SLATE_ARGUMENT(const TSet<FName> *, PropertiesNotEditable)
    SLATE_ARGUMENT(const TSet<FName> *, PropertiesWithDropDown)
    SLATE_ARGUMENT(TSharedPtr<FInstancedStruct>, Item)
    SLATE_ARGUMENT(TWeakObjectPtr<UMapObject>, MapObject)
    // SLATE_ARGUMENT(T*, DataObject)
    SLATE_EVENT(FItemChangedSignature, OnItemChanged)
    SLATE_END_ARGS()

public:
    virtual TSharedRef<SWidget> DisplayDropdownProperty(const FProperty *Property)
    {
        const TArray<FName> AvailableTagsOfType = MapObject->GetVisualPropertiesNamesOfType(Property->GetFName());
        return SNew(SBox)
            .Padding(FMargin(4.0f, 0.0f))
            .VAlign(VAlign_Center)
                [SNew(SDropDownSelectorWidget)
                     .AvailableTags(AvailableTagsOfType)
                     .OnTagChanged_Lambda([this](const FName &Name, const FName &NewTag)
                                          {
                                             if(UAtkStructUtilsFunctionLibrary::SetPropertyValueNestedInStructFromString(*Item, Name.ToString(), NewTag.ToString()))
                                             {
                                                 ItemChanged.ExecuteIfBound(*Item);
                                             }
                                          })
                     .Text(this, &SInstancedStructListRow::GetPropertyValueText, Property)
                     .PropertyName(Property->GetFName())];
    }

    void Construct(const FArguments &InArgs, const TSharedRef<STableViewBase> &InOwnerTableView)
    {
        if (InArgs._PropertiesNotEditable)
        {
            NotEditableProperties = *(InArgs._PropertiesNotEditable);
        }
        if (InArgs._PropertiesWithDropDown)
        {
            PropertiesWithDropDown = *(InArgs._PropertiesWithDropDown);
        }
        MapObject = InArgs._MapObject;
        SInstancedStructListRow::Construct(
            SInstancedStructListRow::FArguments().Item(InArgs._Item).OnItemChanged(InArgs._OnItemChanged),
            InOwnerTableView);
    }

    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName &ColumnName) override
    {
        // iterate struct properties and generate a widget for it
        for (TFieldIterator<FProperty> It(Item->GetScriptStruct()); It; ++It)
        {
            const FProperty *Property = *It;
            const FText PropertyNameText = Property->GetDisplayNameText();
            const FName PropertyName(PropertyNameText.ToString());

            if (ColumnName != PropertyName)
                continue;
            else if (IsPropertyDropDown(PropertyName))
            {
                return DisplayNotEditableProperty(Property);
            }
            else if (IsPropertyEditable(PropertyName))
            {
                // return DisplayEditableProperty(Property);
                return DisplayNotEditableProperty(Property);
            }
            else
            {
                return DisplayNotEditableProperty(Property);
            }
        }

        // default to null widget if property cannot be found
        return SNullWidget::NullWidget;
    }

    bool IsPropertyEditable(const FName &PropertyName) const
    {
        // if(NotEditableProperties)
        //     return true;
        return !NotEditableProperties.Contains(PropertyName);
    }

    bool IsPropertyDropDown(const FName &PropertyName) const
    {
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
DECLARE_DELEGATE_TwoParams(FSelectionChanged, const TArray<int32>&, const ESelectInfo::Type);
class SCustomInstancedStructList : public SInstancedStructList
{
public:
    SLATE_BEGIN_ARGS(SCustomInstancedStructList)
        : _ListSource(nullptr), _StructTypes(), _ItemHeight(20.0f)
    {
    }
    SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_ARGUMENT(TArray<TSharedPtr<FInstancedStruct>> *, ListSource)
    SLATE_ARGUMENT(TArray<UScriptStruct *> *, StructTypes)
    SLATE_ARGUMENT(float, ItemHeight)
    SLATE_ARGUMENT(const TSet<FName> *, NotEditableProperties)
    SLATE_ARGUMENT(const TSet<FName> *, PropertiesWithDropdown)
    SLATE_ARGUMENT(TWeakObjectPtr<UMapObject>, MapObject)

    // called when a struct is edited by the user
    SLATE_EVENT(FItemChangedSignature, OnItemChanged)
    SLATE_EVENT(FSelectionChanged, OnSelectionChanged)
    SLATE_END_ARGS()

    SCustomInstancedStructList() : SInstancedStructList()
    {
    }

    void Construct(const FArguments &InArgs)
    {
        NotEditableProperties = *(InArgs._NotEditableProperties);
        PropertiesWithDropDown = *(InArgs._PropertiesWithDropdown);
        MapObject = InArgs._MapObject;
        OnSelectionChanged = InArgs._OnSelectionChanged;
        SInstancedStructList::Construct(
            SInstancedStructList::FArguments()
                .ItemHeight(InArgs._ItemHeight)
                .ListSource(InArgs._ListSource)
                .StructTypes(InArgs._StructTypes)
                .OnItemChanged(InArgs._OnItemChanged));
    }
    // override if you wish to change the SWidget used for the Row
    virtual TSharedRef<class ITableRow> OnGenerateRow(TSharedPtr<FInstancedStruct> Item, const TSharedRef<STableViewBase> &OwnerTable) override
    {
        return SNew(SCustomInstancedStructListRow, OwnerTable)
            .Item(Item)
            .OnItemChanged(ItemUpdateDelegate)
            .PropertiesNotEditable(&NotEditableProperties)
            .PropertiesWithDropDown(&PropertiesWithDropDown)
            .MapObject(MapObject);
    }

    virtual void HandleSelectionChanged(TSharedPtr<FInstancedStruct> Selection, ESelectInfo::Type SelectInfo) override
    {
        SInstancedStructList::HandleSelectionChanged(Selection, SelectInfo);
        if (!Selection.IsValid())
            return;
        bool bOutResult;
        TArray<int32> SelectedIDs;
        for(const auto& item : ListView->GetSelectedItems())
        {
            const int32 ID = UAtkStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int32>(*item, "ID", bOutResult);
            SelectedIDs.Add(ID);
        }
        if (bOutResult)
            OnSelectionChanged.ExecuteIfBound(SelectedIDs, SelectInfo);
    }

protected:
    TSet<FName> NotEditableProperties;
    TSet<FName> PropertiesWithDropDown;
    TWeakObjectPtr<UMapObject> MapObject;
    FSelectionChanged OnSelectionChanged;
};
