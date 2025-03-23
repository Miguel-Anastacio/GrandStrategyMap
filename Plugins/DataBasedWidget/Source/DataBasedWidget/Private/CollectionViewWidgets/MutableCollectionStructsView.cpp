// Copyright 2024 An@stacioDev All rights reserved.

#include "CollectionViewWidgets/MutableCollectionStructsView.h"

#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "CollectionViewWidgets/MutableCollectionViewDataTable.h"
#include "CollectionViewWidgets/MutableCollectionObjectsView.h"
#include "Components/ListView.h"
#include "ContainerWrappers/ManagerStructsArray.h"
#include "GenericWidget/GenericStructWidget.h"

void UWPropGenMutableCollectionStructsView::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWPropGenMutableCollectionStructsView::SetManager(UTkManagerStructsArray* ManagerStructsArray)
{
	StructManager = ManagerStructsArray;
	if(StructManager.IsValid())
	{
		OnInit(StructManager->GetArray_BP());
		
		StructManager->OnStructAdded.AddDynamic(this, &UWPropGenMutableCollectionStructsView::OnStructAdded);
		StructManager->OnStructRemoved.AddDynamic(this, &UWPropGenMutableCollectionStructsView::OnStructRemoved);
		StructManager->OnArraySet.AddDynamic(this, &UWPropGenMutableCollectionStructsView::OnInit);
		StructManager->OnStructChanged.AddDynamic(this, &UWPropGenMutableCollectionStructsView::OnStructChanged);
		StructManager->OnArrayCleared.AddDynamic(this, &UWPropGenMutableCollectionStructsView::OnCleared);
	}
}

void UWPropGenMutableCollectionStructsView::NativeDestruct()
{
	ClearManager();
	Super::NativeDestruct();
}

void UWPropGenMutableCollectionStructsView::OnStructAdded(const FInstancedStruct& DataStruct)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		UPropGenStructWrapper* StructWrapper = NewObject<UPropGenStructWrapper>();
		StructWrapper->SetStructInstance(DataStruct);
		StructWrappers.Add(StructWrapper);
		View->AddItem(StructWrapper);
	}
}

void UWPropGenMutableCollectionStructsView::OnStructRemoved(const FInstancedStruct& DataStruct)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		TArray<UObject*> Objects = View->GetListItems();
		const int32 Index = StructWrappers.IndexOfByPredicate([DataStruct](const UPropGenStructWrapper* Element)
		{
			return Element->GetStructInstance() == DataStruct;
		});
		
		if(Index != INDEX_NONE)
		{
			View->RemoveItem(StructWrappers[Index]);
			StructWrappers.RemoveAt(Index);
		}
	}
}

void UWPropGenMutableCollectionStructsView::OnInit(const TArray<FInstancedStruct>& Structs)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		StructWrappers.Empty();
		for(const FInstancedStruct& DataStruct: Structs)
		{
			UPropGenStructWrapper* StructWrapper = NewObject<UPropGenStructWrapper>();
			StructWrapper->SetStructInstance(DataStruct);
			StructWrappers.Add(StructWrapper);
		}
		View->SetListItems(StructWrappers);
		View->RequestRefresh();
	}
}

void UWPropGenMutableCollectionStructsView::OnCleared()
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		View->ClearListItems();
		StructWrappers.Empty();
	}
}

void UWPropGenMutableCollectionStructsView::OnStructChanged(const FInstancedStruct& Prev, const FInstancedStruct& New)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		const int32 Index = StructWrappers.IndexOfByPredicate([Prev](const UPropGenStructWrapper* Element)
		{
			return Element->GetStructInstance() == Prev;
		});

		if(Index != INDEX_NONE)
		{
			UPropGenStructWrapper* Wrapper = StructWrappers[Index];
			UUserWidget* EntryWidget = View->GetEntryWidgetFromItem(Wrapper);
			Wrapper->SetStructInstance(New);
			// IUserObjectListEntry::Execute_OnListItemObjectSet(EntryWidget, Wrapper);
			if(UWPropGenGeneric* Widget = Cast<UWPropGenGeneric>(EntryWidget))
			{
				Widget->InitFromStruct(New);
			}
			// View->RequestRefresh();
		}
	}
}

void UWPropGenMutableCollectionStructsView::SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> WidgetClass)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		if (const FProperty* EntryWidgetClassProp = UListViewBase::StaticClass()->FindPropertyByName(TEXT("EntryWidgetClass")))
		{
			void* PropertyAddress = EntryWidgetClassProp->ContainerPtrToValuePtr<void>(View);
			TSubclassOf<UUserWidget>* EntryWidgetClassPtr = static_cast<TSubclassOf<UUserWidget>*>(PropertyAddress);
			*EntryWidgetClassPtr = WidgetClass;
		}
	}
}

void UWPropGenMutableCollectionStructsView::ClearManager()
{
	if (StructManager.IsValid())
	{
		StructManager->OnStructAdded.RemoveDynamic(this, &UWPropGenMutableCollectionStructsView::OnStructAdded);
		StructManager->OnStructRemoved.RemoveDynamic(this, &UWPropGenMutableCollectionStructsView::OnStructRemoved);
		StructManager->OnArraySet.RemoveDynamic(this, &UWPropGenMutableCollectionStructsView::OnInit);
		StructManager->OnArrayCleared.RemoveDynamic(this, &UWPropGenMutableCollectionStructsView::OnCleared);
		StructManager->OnStructChanged.RemoveDynamic(this, &UWPropGenMutableCollectionStructsView::OnStructChanged);
		StructManager = nullptr;
	}
}
