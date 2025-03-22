// Copyright 2024 An@stacioDev All rights reserved.

#include "CollectionViewWidgets/MutableCollectionObjectsView.h"

#include "CollectionViewWidgets/CollectionViewWidgetsDataTable.h"
#include "Components/ListView.h"
#include "ContainerWrappers/ManagerObjectsArray.h"

void UWPropGenMutableCollectionObjectsView::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWPropGenMutableCollectionObjectsView::SetObjectManager(UTkManagerObjectsArray* ManagerObjectsArray)
{
	ObjectManager = ManagerObjectsArray;
	if(ObjectManager.IsValid())
	{
		ObjectManager->OnObjectAdded.AddDynamic(this, &UWPropGenMutableCollectionObjectsView::OnObjectAdded);
		ObjectManager->OnObjectRemoved.AddDynamic(this, &UWPropGenMutableCollectionObjectsView::OnObjectRemoved);
		ObjectManager->OnArraySet.AddDynamic(this, &UWPropGenMutableCollectionObjectsView::OnInit);
		ObjectManager->OnArrayCleared.AddDynamic(this, &UWPropGenMutableCollectionObjectsView::OnCleared);
		OnInit(ObjectManager->GetArray_BP());
	}
}

void UWPropGenMutableCollectionObjectsView::NativeDestruct()
{
	if (ObjectManager.IsValid())
	{
		ObjectManager->OnObjectAdded.RemoveDynamic(this, &UWPropGenMutableCollectionObjectsView::OnObjectAdded);
		ObjectManager->OnObjectRemoved.RemoveDynamic(this, &UWPropGenMutableCollectionObjectsView::OnObjectRemoved);
		ObjectManager->OnArraySet.RemoveDynamic(this, &UWPropGenMutableCollectionObjectsView::OnInit);
		ObjectManager->OnArrayCleared.RemoveDynamic(this, &UWPropGenMutableCollectionObjectsView::OnCleared);
	}
    
	Super::NativeDestruct();
}

void UWPropGenMutableCollectionObjectsView::OnObjectAdded(UObject* Object)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		View->AddItem(Object);
	}
}

void UWPropGenMutableCollectionObjectsView::OnObjectRemoved(UObject* Object)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		View->RemoveItem(Object);
	}
}

void UWPropGenMutableCollectionObjectsView::OnInit(const TArray<UObject*>& Objects)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		View->SetListItems(Objects);
	}
}

void UWPropGenMutableCollectionObjectsView::OnCleared()
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		View->ClearListItems();
	}
}

void UWPropGenMutableCollectionObjectsView::SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> WidgetClass)
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
