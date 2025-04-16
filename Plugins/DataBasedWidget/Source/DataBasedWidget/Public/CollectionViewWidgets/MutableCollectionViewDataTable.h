// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "MutableCollectionStructsView.h"
#include "Types/SlateEnums.h"
#include "MutableCollectionViewDataTable.generated.h"

UCLASS(Abstract)
class DATABASEDWIDGET_API UWPropGenMutableCollectionViewDataTable : public UWPropGenMutableCollectionStructsView, public IPropGenWidgetDataTableInterface
{
	GENERATED_BODY()

public:
	// When calling this the Manager reference will be cleared
	// The list will just display thc contents of the DT
	virtual void SetDataTable_Implementation(UDataTable* NewDataTable) override;
};
