// Copyright 2024 An@stacioDev All rights reserved.
#include "CollectionViewWidgets/MutableCollectionViewDataTable.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "ContainerWrappers/ManagerStructsArray.h"

void UWPropGenMutableCollectionViewDataTable::SetDataTable_Implementation(UDataTable* NewDataTable)
{
	const TArray<FInstancedStruct> DataTableItems = UAtkDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(NewDataTable);
	OnInit(DataTableItems);
	ClearManager();
}