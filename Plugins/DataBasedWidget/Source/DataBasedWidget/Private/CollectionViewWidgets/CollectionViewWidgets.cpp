// Copyright 2024 An@stacioDev All rights reserved.

#include "CollectionViewWidgets/CollectionViewWidgets.h"

#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "Components/ListView.h"

void UPropGenStructWrapper::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		// if(PropertyChangedEvent.Property->GetName() == FName("StructInstance"))
		// {
		// 	InstancePtr = &StructInstance;
		// }
	}
}

void UWPropGenCollectionView::InitFromStructs(const TArray<FInstancedStruct>& Structs)
{
	if(UListView* View = GetListView())
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

void UWPropGenCollectionView::InitFromObjects(const TArray<UObject*>& Objects)
{
	if(UListView* View = GetListView())
	{
		SourceObjects = Objects;
		View->SetListItems(SourceObjects);
	}
}

class UListView* UWPropGenCollectionView::GetListView()
{
	return nullptr;
}
#if WITH_EDITOR
void UWPropGenCollectionView::SetEntryWidgetClass(TSubclassOf<UUserWidget> WidgetClass)
{
	if(UListView* View = GetListView())
	{
		if (const FProperty* EntryWidgetClassProp = UListViewBase::StaticClass()->FindPropertyByName(TEXT("EntryWidgetClass")))
		{
			void* PropertyAddress = EntryWidgetClassProp->ContainerPtrToValuePtr<void>(View);
			TSubclassOf<UUserWidget>* EntryWidgetClassPtr = static_cast<TSubclassOf<UUserWidget>*>(PropertyAddress);
			*EntryWidgetClassPtr = WidgetClass;
		}
	}
}

void UWPropGenCollectionView::CreateListView(TSubclassOf<UUserWidget> WidgetClass)
{
	UWidgetTree* MainAssetWidgetTree = UAssetCreatorFunctionLibrary::GetWidgetTree(this);
	if (!MainAssetWidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetTree is null!"));
		return;
	}
	
	if (!MainAssetWidgetTree->RootWidget)
	{
		SetRootWidget(MainAssetWidgetTree);
		SetEntryWidgetClass(WidgetClass);
		MainAssetWidgetTree->RootWidget = GetListView();
		
		MainAssetWidgetTree->Modify();
		UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(this);
	}
}

void UWPropGenCollectionView::SetRootWidget(UWidgetTree* Tree)
{
}
#endif

