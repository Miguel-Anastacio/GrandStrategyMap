// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "ListViewWidgets.h"
#include "Types/SlateEnums.h"
#include "ListViewWidgetsDataTable.generated.h"

UCLASS(Abstract, BlueprintType)
class DATABASEDWIDGET_API UWPropGenListViewDataTable : public UWPropGenListView
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable, Category=ListViewDataTable)
	virtual void SetDataTable(UDataTable* NewDataTable);
	
protected:

	UPROPERTY(EditAnywhere, Category=ListViewDataTable, BlueprintReadWrite)
	TSoftObjectPtr<UDataTable> DataTable;
	
};
