// Copyright 2024 An@stacioDev All rights reserved.
#include "CollectionViewWidgets/MutableCollectionViewDataTable.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "ContainerWrappers/ManagerStructsArray.h"

void UWPropGenMutableCollectionViewDataTable::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	const TArray<FInstancedStruct> DataTableItems = UAtkDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable);
	OnInit(DataTableItems);
}

void UWPropGenMutableCollectionViewDataTable::SetDataTable_Implementation(UDataTable* NewDataTable)
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

void UWPropGenMutableCollectionViewDataTable::SetManager(UTkManagerStructsArray* ManagerStructsArray)
{
	// If the List is based on a data table, then add dataTable entries to Struct Manager
	if(ManagerStructsArray)
	{
		ManagerStructsArray->AddMultiple_BP(GetDataTableEntries());
	}
	Super::SetManager(ManagerStructsArray);
}

TArray<FInstancedStruct> UWPropGenMutableCollectionViewDataTable::GetDataTableEntries() const
{
	return UAtkDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable);
}
