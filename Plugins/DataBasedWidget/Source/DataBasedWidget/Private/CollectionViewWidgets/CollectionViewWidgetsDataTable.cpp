// Copyright 2024 An@stacioDev All rights reserved.
#include "CollectionViewWidgets/CollectionViewWidgetsDataTable.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"

void UWPropGenCollectionViewDataTable::NativeConstruct()
{
	const TArray<FInstancedStruct> DataTableItems = UDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable);
	Execute_InitFromStructs(this, DataTableItems);
	Super::NativeConstruct();
}

void UWPropGenCollectionViewDataTable::SetDataTable_Implementation(UDataTable* NewDataTable)
{
	if (DataTable.Get() == NewDataTable)
	{
		return;
	}
	
	DataTable = NewDataTable;

#if WITH_EDITOR
	return;
#else
	// At runtime load data table and re init ListView
	const TArray<FInstancedStruct> DataTableItems = UDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable);
	InitFromStructs(DataTableItems);
#endif
}
