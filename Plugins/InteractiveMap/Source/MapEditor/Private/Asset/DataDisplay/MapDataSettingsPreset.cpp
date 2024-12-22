// Fill out your copyright notice in the Description page of Project Settings.
#include "Asset/DataDisplay/MapDataSettingsPreset.h"

#if WITH_EDITOR
void UMapDataSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	OnObjectChanged.Broadcast();
}
#endif}

