// Copyright 2024 An@stacioDev All rights reserved.

#include "CollectionViewWidgets/StaticCollectionViewDataTable.h"
#include "GenericWidget/GenericStructWidget.h"
#include "GenericWidget/DataDrivenUserWidgetInterface.h"
#if WITH_EDITOR
#include "BlueprintLibrary/WidgetEditorFunctionLibrary.h"
#endif

void UWPropGenStaticCollectionViewDataTable::NativeOnInitialized()
{
	Init();
	Super::NativeOnInitialized();
}

void UWPropGenStaticCollectionViewDataTable::Init()
{
	const TArray<FInstancedStruct> DataTableItems = UAtkDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable);
	if(const UVerticalBox* VerticalBox = Cast<UVerticalBox>(Execute_GetCollectionContainer(this)))
	{
		int Index = 0;
		TArray<UWidget*> Widgets;
		WidgetTree->GetAllWidgets(Widgets);
		for(UWidget* Widget : Widgets)
		{
			if(UWPropGenGeneric* GenericStructWidget = Cast<UWPropGenGeneric>(Widget))
			{
				if(Index >= DataTableItems.Num())
				{
					return;
				}
				GenericStructWidget->InitFromStruct(DataTableItems[Index]);
				Index++;
			}
		}
	}
}

void UWPropGenStaticCollectionViewDataTable::SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> MembersWidgetClass)
{
	WidgetClass = MembersWidgetClass;
}

void UWPropGenStaticCollectionViewDataTable::SetDataTable_Implementation(UDataTable* NewDataTable)
{
	DataTable = NewDataTable;
#if WITH_EDITOR
	RefreshContainer();
#endif
}

#if WITH_EDITOR
void UWPropGenStaticCollectionViewDataTable::RefreshContainer() const
{
	UVerticalBox* AssetVerticalBox = Cast<UVerticalBox>(UAtkWidgetEditorFunctionLibrary::GetPanelWidget(this, FName("Container")));
	UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(this);
	if(!AssetVerticalBox)
	{
		return;
	}
	if(!MainAssetWidgetTree)
	{
		return;
	}
	AssetVerticalBox->ClearChildren();
	AssetVerticalBox->Modify();
	UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(this);
	for(const FInstancedStruct& Struct : UAtkDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable))
	{
		if(UUserWidget* NewWidget = MainAssetWidgetTree->ConstructWidget<UUserWidget>(WidgetClass))
		{
			if(UWPropGenGeneric* GenericStructWidget = Cast<UWPropGenGeneric>(NewWidget))
			{
				GenericStructWidget->InitFromStruct(Struct);
			}
			AssetVerticalBox->AddChildToVerticalBox(NewWidget);
		}
	}
	AssetVerticalBox->Modify();
	UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(this);
}
#endif





