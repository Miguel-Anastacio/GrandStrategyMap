// Copyright 2024 An@stacioDev All rights reserved.
#include "CollectionViewWidgets/WidgetCollectionInterface.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ListView.h"

void IWidgetCollectionInterface::CreateCollectionContainer_Implementation()
{
	UUserWidget* Widget = Cast<UUserWidget>(this);
	UWidgetTree* MainAssetWidgetTree = UAssetCreatorFunctionLibrary::GetWidgetTree(Widget);
	if (!MainAssetWidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetTree is null!"));
		return;
	}
	
	if (!MainAssetWidgetTree->RootWidget)
	{
		Execute_SetRootWidget(Widget, MainAssetWidgetTree);
		MainAssetWidgetTree->RootWidget = Execute_GetCollectionContainer(Widget);
		
		MainAssetWidgetTree->Modify();
		UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(Widget);
	}
}

bool IWidgetCollectionInterface::IsCollectionListView()
{
	UObject* Widget = Cast<UObject>(this);
	return Cast<UListView>(Execute_GetCollectionContainer(Widget)) != nullptr;
}

