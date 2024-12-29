// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/MapObject.h"
#include "MapEditor.h"
#include "TextureCompiler.h"
#include "Assets/AssetCreatorFunctionLibrary.h"
#include "FileIO/DataManagerFunctionLibrary.h"
#include "FileIO/FilePickerFunctionLibrary.h"

#if WITH_EDITOR
void UMapObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	if(LookupTexture)
	{
		FTextureCompilingManager::Get().FinishCompilation({LookupTexture});
		LookupTextureData = UAssetCreatorFunctionLibrary::ReadTextureToArray(LookupTexture);
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("Size of buffer: %d"), LookupTextureData.Num());
	}

	LoadLookupMap(LookupFilePath);
	

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
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Lookup Table size: %d"), LookupTable.Num());
	if(const int32* ID = LookupTable.Find(Color))
	{
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("ID: %d"), *ID);
		int32 Index = -1;
		for(const auto& Data : MapData)
		{
			Index++;
			bool bOutResult = false;
			const int32 StructId = UDataManagerFunctionLibrary::GetPropertyValueFromStruct<int32>(StructType, Data, "ID", bOutResult);
			if(!bOutResult)
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
		return -1;
	}
	
	UE_LOG(LogInteractiveMapEditor, Error, TEXT("Color in LookupTable but ID not in Map Data"));
	return -1;
}

FColor UMapObject::GetColorFromUv(const FVector2D& Uv) const 
{
	return UAssetCreatorFunctionLibrary::GetColorFromUV(LookupTexture, Uv, LookupTextureData);
}

void UMapObject::LoadLookupMap(const FString& FilePath)
{
	const TArray<FLookupEntry> Lookup = UDataManagerFunctionLibrary::LoadCustomDataFromJson<FLookupEntry>(LookupFilePath);
	LookupTable.Empty();
	for(const auto& Entry : Lookup)
	{
		LookupTable.Emplace(UDataManagerFunctionLibrary::ConvertHexStringToRGB(Entry.Color), FCString::Atoi(*Entry.Name));
	}
	for(const auto& Entry : LookupTable)
	{
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("Color:%s, ID: %d"), *Entry.Key.ToString(), Entry.Value)
	}

}
#endif
