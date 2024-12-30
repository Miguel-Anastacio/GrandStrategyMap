// Fill out your copyright notice in the Description page of Project Settings.
#include "Editor/MapEditorPreset.h"

#if WITH_EDITOR
UMapEditorPreset::UMapEditorPreset()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/Script/Engine.Material'/InteractiveMap/GSMap/InteractiveMap/Material/Material/M_PoliticalMap.M_PoliticalMap'"));
	if(MaterialFinder.Succeeded())
	{
		Material = MaterialFinder.Object;
	}
}

void UMapEditorPreset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	// OnObjectChanged.Broadcast();
}

#endif}

