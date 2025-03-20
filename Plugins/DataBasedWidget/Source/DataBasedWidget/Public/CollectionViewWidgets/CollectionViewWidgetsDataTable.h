// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "CollectionViewWidgets.h"
#include "Types/SlateEnums.h"
#include "CollectionViewWidgetsDataTable.generated.h"

// Base Class for 
UCLASS(Abstract)
class DATABASEDWIDGET_API UWPropGenCollectionViewDataTable : public UWPropGenCollectionView
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable, Category=CollectionViewDataTable)
	virtual void SetDataTable_Implementation(UDataTable* NewDataTable) override;

protected:
	UPROPERTY(EditAnywhere, Category=CollectionViewDataTable, BlueprintReadWrite)
	TSoftObjectPtr<UDataTable> DataTable;
	
};
