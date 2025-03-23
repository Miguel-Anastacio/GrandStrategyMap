// Copyright 2024 An@stacioDev All rights reserved.

#include "CollectionViewWidgets/MutableCollectionStructsView.h"

#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "CollectionViewWidgets/MutableCollectionViewDataTable.h"
#include "CollectionViewWidgets/MutableCollectionObjectsView.h"
#include "Components/ListView.h"
#include "ContainerWrappers/ManagerStructsArray.h"

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
		StructManager->OnArrayCleared.AddDynamic(this, &UWPropGenMutableCollectionStructsView::OnCleared);
	}
}

void UWPropGenMutableCollectionStructsView::NativeDestruct()
{
	if (StructManager.IsValid())
	{
		StructManager->OnStructAdded.RemoveDynamic(this, &UWPropGenMutableCollectionStructsView::OnStructAdded);
		StructManager->OnStructRemoved.RemoveDynamic(this, &UWPropGenMutableCollectionStructsView::OnStructRemoved);
		StructManager->OnArraySet.RemoveDynamic(this, &UWPropGenMutableCollectionStructsView::OnInit);
		StructManager->OnArrayCleared.RemoveDynamic(this, &UWPropGenMutableCollectionStructsView::OnCleared);
	}
    
	Super::NativeDestruct();
}

void UWPropGenMutableCollectionStructsView::OnStructAdded(const FInstancedStruct& DataStruct)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		UPropGenStructWrapper* StructWrapper = NewObject<UPropGenStructWrapper>();
		StructWrapper->SetStructInstance(DataStruct);
		View->AddItem(StructWrapper);
	}
}

void UWPropGenMutableCollectionStructsView::OnStructRemoved(const FInstancedStruct& DataStruct)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		TArray<UObject*> Objects = View->GetListItems();
		for(int i = 0; i < Objects.Num(); i++)
		{
			UPropGenStructWrapper* Wrapper = Cast<UPropGenStructWrapper>(Objects[i]);
			if(Wrapper && Wrapper->GetStructInstance() == DataStruct)
			{
				View->RemoveItem(Wrapper);
				return;
			}
		}
	}
}

void UWPropGenMutableCollectionStructsView::OnInit(const TArray<FInstancedStruct>& Structs)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		StructWrappers.Empty();
		for(FInstancedStruct DataStruct: Structs)
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
