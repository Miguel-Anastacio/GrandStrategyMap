// Copyright 2024 An@stacioDev All rights reserved.

#include "CollectionViewWidgets/CollectionViewWidgets.h"
#include "Components/ListView.h"
#include "ContainerWrappers/ManagerObjectsArray.h"

void UWPropGenMutableObjectsCollectionView::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWPropGenMutableObjectsCollectionView::SetObjectManager(UTkManagerObjectsArray* ManagerObjectsArray)
{
	ObjectManager = ManagerObjectsArray;
	if(ObjectManager.IsValid())
	{
		ObjectManager->OnObjectAdded.AddDynamic(this, &UWPropGenMutableObjectsCollectionView::OnObjectAdded);
		ObjectManager->OnObjectRemoved.AddDynamic(this, &UWPropGenMutableObjectsCollectionView::OnObjectRemoved);
		ObjectManager->OnArraySet.AddDynamic(this, &UWPropGenMutableObjectsCollectionView::OnInit);
		ObjectManager->OnArrayCleared.AddDynamic(this, &UWPropGenMutableObjectsCollectionView::OnCleared);
	}
}

void UWPropGenMutableObjectsCollectionView::NativeDestruct()
{
	if (ObjectManager.IsValid())
	{
		ObjectManager->OnObjectAdded.RemoveDynamic(this, &UWPropGenMutableObjectsCollectionView::OnObjectAdded);
		ObjectManager->OnObjectRemoved.RemoveDynamic(this, &UWPropGenMutableObjectsCollectionView::OnObjectRemoved);
		ObjectManager->OnArraySet.RemoveDynamic(this, &UWPropGenMutableObjectsCollectionView::OnInit);
		ObjectManager->OnArrayCleared.RemoveDynamic(this, &UWPropGenMutableObjectsCollectionView::OnCleared);

		OnInit(ObjectManager->GetObjects());
	}
    
	Super::NativeDestruct();
}

void UWPropGenMutableObjectsCollectionView::OnObjectAdded(UObject* Object)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		View->AddItem(Object);
	}
}

void UWPropGenMutableObjectsCollectionView::OnObjectRemoved(UObject* Object)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		View->RemoveItem(Object);
	}
}

void UWPropGenMutableObjectsCollectionView::OnInit(const TArray<UObject*>& Objects)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		View->SetListItems(Objects);
	}
}

void UWPropGenMutableObjectsCollectionView::OnCleared()
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		View->ClearListItems();
	}
}

// void UWPropGenCollectionView::InitFromStructs_Implementation(const TArray<FInstancedStruct>& Structs)
// {
// 	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
// 	{
// 		// SourceObjects.Reserve(Structs.Num());
// 		// for(FInstancedStruct DataStruct: Structs)
// 		// {
// 		// 	UPropGenStructWrapper* StructWrapper = NewObject<UPropGenStructWrapper>();
// 		// 	StructWrapper->SetStructInstance(DataStruct);
// 		// 	SourceObjects.Add(StructWrapper);
// 		// }
// 		// View->SetListItems(SourceObjects);
// 	}
// }

void UWPropGenMutableObjectsCollectionView::SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> WidgetClass)
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
