// Fill out your copyright notice in the Description page of Project Settings.
#include "Editor/MapEditorPreset.h"

#include "MapEditor.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"

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
	if(TileDataStructType)
	{
		if(!UADStructUtilsFunctionLibrary::StructHasPropertyWithTypeCompatible<int32>(TileDataStructType, FName("ID")))
		{
			// THROW ERROR AT USER  FACE
			UE_LOG(LogInteractiveMapEditor, Error, TEXT("Struct type must have property with type ID"));
			TileDataStructType = nullptr;
			this->PostEditChange();
		}
	}
}

#endif}

