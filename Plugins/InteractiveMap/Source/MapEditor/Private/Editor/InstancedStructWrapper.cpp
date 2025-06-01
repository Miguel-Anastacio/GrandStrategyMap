// Copyright 2024 An@stacioDev All rights reserved.
#include "Editor/InstancedStructWrapper.h"

#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "MapDataMode/MapEditorDataAppMode.h"

void UInstancedStructWrapper::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	// Block updates of changes in structInstance
	if(Previous.IsValid() && Previous.GetScriptStruct() != StructInstance.GetScriptStruct())
	{
		StructInstance = Previous;
	}
	else if(AppMode.IsValid())
	{
		Previous = StructInstance;
		AppMode.Pin().Get()->UpdateMap(StructInstance, ID);
	}
}

void UInstancedStructWrapper::SetAppMode(TWeakPtr<class FMapEditorDataAppMode> appMode)
{
	AppMode = appMode;
}

void UInstancedStructWrapper::SetStructInstance(const FInstancedStruct& Struct)
{
	StructInstance = Struct;
	Previous = StructInstance;
}
