// Fill out your copyright notice in the Description page of Project Settings.
#include "Editor/MapEditorPreset.h"
#include "MapEditor.h"
#include "MapObject.h"

#if WITH_EDITOR
UMapEditorPreset::UMapEditorPreset()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/Script/Engine.Material'/InteractiveMap/GSMap/InteractiveMap/Material/Material/M_PoliticalMap.M_PoliticalMap'"));
	if(MaterialFinder.Succeeded())
	{
		Material = MaterialFinder.Object;
	}
	OceanTileDataType = FBaseMapStruct::StaticStruct();
}

void UMapEditorPreset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	if(TileDataStructType)
	{
		if(!TileDataStructType->IsChildOf(FBaseMapStruct::StaticStruct()))
		{
			// THROW ERROR AT USER  FACE
            UE_LOG(LogInteractiveMapEditor, Error, TEXT("Struct type must inherit from FBaseMapStruct"));
            TileDataStructType = nullptr;
            this->PostEditChange();
		}
	}
}
#endif}

