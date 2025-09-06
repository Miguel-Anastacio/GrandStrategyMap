// Fill out your copyright notice in the Description page of Project Settings.
#include "Editor/MapEditorPreset.h"
#include "MapEditor.h"
#include "MapObject.h"
#include "BlueprintLibrary/PropertyUtilityFunctionLibrary.h"

#if WITH_EDITOR
UMapEditorPreset::UMapEditorPreset()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/Script/Engine.Material'/InteractiveMap/GSMap/Editor/M_PoliticalMapEditor.M_PoliticalMapEditor'"));
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
		if(!UAtkStructUtilsFunctionLibrary::StructHasPropertyOfTypeWithName<int>(TileDataStructType, FName("ID")))
		{
			const FText Title = FText::FromString(TEXT("Error"));
			const FText Message = FText::FromString(TEXT("Struct type must have a numeric field named ID"));
			EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
			TileDataStructType = nullptr;
			this->PostEditChange();
		}
	}
}
#endif

