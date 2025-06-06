// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "MutableCollectionViewDataTable.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ListView.h"
#include "Types/SlateEnums.h"
#include "ListViewWidgetsDataTable.generated.h"

UCLASS(Blueprintable, BlueprintType, DisplayName=MutableListViewDataTable)
class DATABASEDWIDGET_API UWPropGenListViewDataTable : public UWPropGenMutableCollectionViewDataTable
{
	GENERATED_BODY()

public:

	virtual UWidget* GetCollectionContainer_Implementation() override
	{
		return ListView;
	};

protected:
#if WITH_EDITOR
	virtual void SetRootWidget(UWidgetTree* Tree) override
	{
		ListView = Tree->ConstructWidget<UListView>(UListView::StaticClass(), FName("ListView"));;
	};
#endif
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = ListViewDataTable)
	class UListView* ListView;

};
