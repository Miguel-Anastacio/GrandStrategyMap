// Copyright 2024 An@stacioDev All rights reserved.
#include "Editor/InstancedStructWrapper.h"

#include "MapDataMode/MapEditorDataAppMode.h"

void UInstancedStructWrapper::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	if(AppMode.IsValid())
	{
		AppMode.Pin().Get()->UpdateMap(StructInstance, ID);
	}
}

void UInstancedStructWrapper::SetAppMode(TWeakPtr<class FMapEditorDataAppMode> appMode)
{
	AppMode = appMode;
}
