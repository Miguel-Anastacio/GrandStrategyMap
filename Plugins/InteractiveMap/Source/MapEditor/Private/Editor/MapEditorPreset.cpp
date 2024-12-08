// Fill out your copyright notice in the Description page of Project Settings.
#include "Editor/MapEditorPreset.h"

#if WITH_EDITOR
void UMapEditorPreset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	OnObjectChanged.Broadcast();
}
#endif}

