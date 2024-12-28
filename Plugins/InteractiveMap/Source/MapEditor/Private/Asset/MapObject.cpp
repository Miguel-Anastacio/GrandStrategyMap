// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/MapObject.h"
#include "MapEditor.h"
#include "TextureCompiler.h"
#include "VisualizeTexture.h"
#include "Assets/AssetCreatorFunctionLibrary.h"
#include "FileIO/DataManagerFunctionLibrary.h"
#include "FileIO/FilePickerFunctionLibrary.h"
#include "Log/LogFunctionLibrary.h"

#if WITH_EDITOR
void UMapObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	if(LookupTexture)
	{
		FTextureCompilingManager::Get().FinishCompilation({LookupTexture});
		auto rawBuffer = UAssetCreatorFunctionLibrary::ReadTextureToBuffer(LookupTexture);
		LookupTextureData = UAssetCreatorFunctionLibrary::ReadTextureToArray(LookupTexture);
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("Size of buffer: %llu"), sizeof(*rawBuffer))
	}
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
	UDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(FilePathMapData, StructType, MapData);
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
	FilePathMapData = FilesNames[0];

	// Load lookup
	auto Lookup = UDataManagerFunctionLibrary::LoadCustomDataFromJson<FLookupEntry>(LookupFilePath);
	for(const auto& Entry : Lookup)
	{
		if(FCString::Atoi(*Entry.Name) == 24)
		{
			int a = 0;
		}
			LookupTable.Add(UDataManagerFunctionLibrary::ConvertHexStringToRGB(Entry.Color), FCString::Atoi(*Entry.Name));
	}
	for(const auto& Entry : LookupTable)
	{
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("Color:%s, ID: %d"), *Entry.Key.ToString(), Entry.Value)
	}
	
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

int UMapObject::GetIndexOfTileSelected(const FColor& Color)
{
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("FColor: %s"), *Color.ToString());
	// for(const auto& Entry : LookupTable)
	// {
	// 	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Color:%s, ID: %d"), *Entry.Key.ToString(), Entry.Value)
	// }
	if(const int* ID = LookupTable.Find(Color))
	{
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("ID: %d"), *ID);
		int Index = -1;
		for(const auto& Data : MapData)
		{
			bool OutResult = false;
			const int StructId = UDataManagerFunctionLibrary::GetPropertyValueFromStruct<int>(StructType, Data, "ID", OutResult);
			Index++;
			if(!OutResult)
			{
				continue;
			}
			if(StructId == *ID)
			{
				return Index;
			}
		}
	}
	else
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Color not in LookupTable"));
	}
	
	UE_LOG(LogInteractiveMapEditor, Error, TEXT("Color in LookupTable but ID not in Map Data"));
	return -1;
}

FColor UMapObject::GetColorFromUv(const FVector2D& uv) const 
{
	FTextureCompilingManager::Get().FinishCompilation({LookupTexture});
	const uint8* rawBuffer = UAssetCreatorFunctionLibrary::ReadTextureToBuffer(LookupTexture);
	return UAssetCreatorFunctionLibrary::GetColorFromUV(LookupTexture, uv, rawBuffer);
}
#endif
