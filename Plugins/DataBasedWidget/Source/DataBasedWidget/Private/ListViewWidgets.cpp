// Copyright 2024 An@stacioDev All rights reserved.

#include "ListViewWidgets.h"

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

void UWPropGenListView::InitFromStructs(const TArray<FInstancedStruct>& Structs)
{
	if(ListView)
	{
		SourceObjects.Reserve(Structs.Num());
		for(FInstancedStruct DataStruct: Structs)
		{
			UPropGenStructWrapper* StructWrapper = NewObject<UPropGenStructWrapper>();
			StructWrapper->SetStructInstance(DataStruct);
			SourceObjects.Add(StructWrapper);
		}
		ListView->SetListItems(SourceObjects);
	}
}

void UWPropGenListView::InitFromObjects(const TArray<UObject*>& Objects)
{
	if(ListView)
	{
		SourceObjects = Objects;
		ListView->SetListItems(SourceObjects);
	}
}
#if WITH_EDITOR
void UWPropGenListView::SetEntryWidgetClass(TSubclassOf<UUserWidget> WidgetClass)
{
	if (ListView)
	{
		if (FProperty* EntryWidgetClassProp = UListViewBase::StaticClass()->FindPropertyByName(TEXT("EntryWidgetClass")))
		{
			void* PropertyAddress = EntryWidgetClassProp->ContainerPtrToValuePtr<void>(ListView);
			TSubclassOf<UUserWidget>* EntryWidgetClassPtr = static_cast<TSubclassOf<UUserWidget>*>(PropertyAddress);
			*EntryWidgetClassPtr = WidgetClass;
		}
	}
}

void UWPropGenListView::CreateListView(TSubclassOf<UUserWidget> WidgetClass)
{
	UWidgetTree* MainAssetWidgetTree = UAssetCreatorFunctionLibrary::GetWidgetTree(this);
	if (!MainAssetWidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetTree is null!"));
		return;
	}
	
	if (!MainAssetWidgetTree->RootWidget)
	{
		ListView = MainAssetWidgetTree->ConstructWidget<UListView>(UListView::StaticClass(), FName("ListView"));
		SetEntryWidgetClass(WidgetClass);
		MainAssetWidgetTree->RootWidget = ListView;
		
		MainAssetWidgetTree->Modify();
		UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(this);
	}
}
#endif
