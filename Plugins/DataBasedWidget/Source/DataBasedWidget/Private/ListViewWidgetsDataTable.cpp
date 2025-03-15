// Copyright 2024 An@stacioDev All rights reserved.

#include "ListViewWidgetsDataTable.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "Components/ListView.h"


void UWPropGenListViewDataTable::NativeConstruct()
{
	const TArray<FInstancedStruct> DataTableItems = UDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable);
	InitFromStructs(DataTableItems);
	Super::NativeConstruct();
}

void UWPropGenListViewDataTable::SetDataTable(UDataTable* NewDataTable)
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
	if (const UDataTable* LoadedDataTable = DataTable.LoadSynchronous())
	{
		// UpdateListView Items
		const TArray<FInstancedStruct> DataTableItems = UDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable);
		InitFromStructs(DataTableItems);
	}
#endif
	
}
