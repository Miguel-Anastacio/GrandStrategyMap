// Copyright 2024 An@stacioDev All rights reserved.
#include "CollectionViewWidgets/WidgetCollectionInterface.h"
#include "Blueprint/WidgetTree.h"
#if WITH_EDITOR
#include "BlueprintLibrary/WidgetEditorFunctionLibrary.h"
#endif
#include "Components/ListView.h"

#if WITH_EDITOR
void IPropGenWidgetCollectionInterface::CreateCollectionContainer()
{
	UUserWidget* Widget = Cast<UUserWidget>(this);
	UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(Widget);
	if (!MainAssetWidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetTree is null!"));
		return;
	}

	// If there is not a root widget, then create one
	// You must implement SetRootWidget in c++, in BP there is no need you can just add the widget in the editor
	if (!MainAssetWidgetTree->RootWidget)
	{
		SetRootWidget(MainAssetWidgetTree);
		MainAssetWidgetTree->RootWidget = Execute_GetCollectionContainer(Widget);
		
		MainAssetWidgetTree->Modify();
		UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(Widget);
	}
}
#endif

bool IPropGenWidgetCollectionInterface::IsCollectionListView()
{
	UObject* Widget = Cast<UObject>(this);
	return Cast<UListView>(Execute_GetCollectionContainer(Widget)) != nullptr;
}

