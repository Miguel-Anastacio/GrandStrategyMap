// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "CollectionViewWidgets.h"
#include "Types/SlateEnums.h"
#include "CollectionViewWidgetsDataTable.generated.h"

UCLASS(Abstract)
class DATABASEDWIDGET_API UWPropGenCollectionViewDataTable : public UWPropGenCollectionView
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
