// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "MutableCollectionStructsView.h"
#include "Types/SlateEnums.h"
#include "CollectionViewWidgetsDataTable.generated.h"

UCLASS(Abstract)
class DATABASEDWIDGET_API UWPropGenMutableCollectionViewDataTable : public UWPropGenMutableCollectionStructsView, public IPropGenWidgetDataTableInterface
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	UFUNCTION(BlueprintCallable, Category=CollectionViewDataTable)
	virtual void SetDataTable_Implementation(UDataTable* NewDataTable) override;
	virtual void SetManager(UTkManagerStructsArray* ManagerStructsArray) override;
	TArray<FInstancedStruct> GetDataTableEntries() const;
protected:
	UPROPERTY(EditAnywhere, Category=CollectionViewDataTable, BlueprintReadWrite)
	TSoftObjectPtr<UDataTable> DataTable;
	
};
