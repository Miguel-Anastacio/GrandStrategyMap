// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/MapObject.h"
#include "MapEditor.h"
#include "FileIO/DataManagerFunctionLibrary.h"
#include "FileIO/FilePickerFunctionLibrary.h"
#include "Log/LogFunctionLibrary.h"

#if WITH_EDITOR
void UMapObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	OnObjectChanged.Broadcast();
}

void UMapObject::UpdateTile(int Index, const FInstancedStruct& NewData)
{
	if(Index < 0 || Index > MapData.Num() - 1)
		return;
	MapData[Index] = NewData;
}

void UMapObject::UpdateTileProperty(int Index, const FString& PropertyName,const FString& NewValue)
{
	if(Index < 0 || Index > MapData.Num() - 1)
		return;
	void* StructMemory = MapData[Index].GetMutableMemory();
	
	if(FProperty* Property = StructType->FindPropertyByName(FName(*PropertyName)))
	{
		if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
		{
			int32 Value = FCString::Atoi(*NewValue);
			IntProperty->SetPropertyValue_InContainer(StructMemory, Value);
		}
		else if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
		{
			StrProperty->SetPropertyValue_InContainer(StructMemory, NewValue);
		}
		//NO MATCHING PROPERTY
		else
		{
			UE_LOG(LogInteractiveMapEditor, Log, TEXT("No Matching Property with the compatible values: '%s'"), *PropertyName);
		}
	}
	else
	{
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("Property does not exist in struct '%s'"), *PropertyName);
	}
}

void UMapObject::SaveData() const
{
	UDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(FilePath, StructType, MapData);
}

void UMapObject::LoadDataFromFile()
{
	TArray<FString> FilesNames;
	UFilePickerFunctionLibrary::OpenFileDialogJson(FPaths::ProjectDir(), FilesNames);
	if(FilesNames.IsEmpty())
	{
		UE_LOG(LogInteractiveMapEditor, Warning, TEXT("No file selected"));
		return;
	}
	if(!StructType)
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Struct Data Type not selected"));
		return;
	}
	
	MapData = UDataManagerFunctionLibrary::LoadCustomDataFromJson(FilesNames[0], StructType);
	FilePath = FilesNames[0];
	// ULogFunctionLibrary::LogArrayInstancedStructs(MapData, "Log Map Data Structs", StructType);
}

void UMapObject::SetMapData(const TArray<FInstancedStruct>& NewData)
{
	if(NewData.Num() > 0)
	{
		if(NewData[0].GetScriptStruct() != StructType)
		{
			UE_LOG(LogInteractiveMapEditor, Error, TEXT("New data type is not of the same as StructType"));
			return;
		}
		MapData = NewData;
	}
}
#endif
