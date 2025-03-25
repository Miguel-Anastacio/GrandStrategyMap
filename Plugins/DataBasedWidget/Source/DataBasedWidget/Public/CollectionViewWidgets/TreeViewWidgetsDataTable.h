// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "MutableCollectionViewDataTable.h"
#include "Types/SlateEnums.h"
#include "Components/TreeView.h"
#include "TreeViewWidgetsDataTable.generated.h"

UCLASS(Blueprintable,  BlueprintType, DisplayName=MutableTreeViewDataTable)
class DATABASEDWIDGET_API UWPropGenTreeViewDataTable : public UWPropGenMutableCollectionViewDataTable
{
	GENERATED_BODY()

public:
	virtual UWidget* GetCollectionContainer_Implementation() override
	{
		return TreeView;
	};

protected:
#if WITH_EDITOR
	virtual void SetRootWidget(UWidgetTree* Tree) override
	{
		TreeView = Tree->ConstructWidget<UTreeView>(UTreeView::StaticClass(), FName("TreeView"));;
	};
#endif
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = ListViewDataTable)
	class UTreeView* TreeView;

};
