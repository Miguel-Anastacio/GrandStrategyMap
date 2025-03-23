// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "MutableCollectionViewDataTable.h"
#include "Types/SlateEnums.h"
#include "Components/TileView.h"
#include "GridViewWidgetsDataTable.generated.h"

UCLASS(Blueprintable, BlueprintType, DisplayName=MutableGridViewDataTable)
class DATABASEDWIDGET_API UWPropGenGridViewDataTable : public UWPropGenMutableCollectionViewDataTable
{
	GENERATED_BODY()

public:
	virtual UWidget* GetCollectionContainer_Implementation() override
	{
		return TileView;
	};
protected:
	virtual void SetRootWidget(UWidgetTree* Tree) override
	{
		TileView = Tree->ConstructWidget<UTileView>(UTileView::StaticClass(), FName("TileView"));;
	};
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = ListViewDataTable)
	UTileView* TileView;
};
