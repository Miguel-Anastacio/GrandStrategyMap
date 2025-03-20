// Copyright 2024 An@stacioDev All rights reserved.

#include "CollectionViewWidgets/CollectionViewWidgets.h"
#include "Components/ListView.h"

void UWPropGenCollectionView::AddObject_Implementation(UObject* Object)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		SourceObjects.Add(Object);
		View->AddItem(Object);
	}
}

void UWPropGenCollectionView::RemoveObject_Implementation(UObject* Object)
{
	IWidgetCollectionInterface::RemoveObject_Implementation(Object);
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		View->RemoveItem(Object);
	}
}

void UWPropGenCollectionView::InitFromStructs_Implementation(const TArray<FInstancedStruct>& Structs)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		SourceObjects.Reserve(Structs.Num());
		for(FInstancedStruct DataStruct: Structs)
		{
			UPropGenStructWrapper* StructWrapper = NewObject<UPropGenStructWrapper>();
			StructWrapper->SetStructInstance(DataStruct);
			SourceObjects.Add(StructWrapper);
		}
		View->SetListItems(SourceObjects);
	}
}

void UWPropGenCollectionView::InitFromObjects_Implementation(const TArray<UObject*>& Objects)
{
	if(UListView* View = Cast<UListView>(Execute_GetCollectionContainer(this)))
	{
		SourceObjects = Objects;
		View->SetListItems(SourceObjects);
	}
}

void UWPropGenCollectionView::SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> WidgetClass)
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
