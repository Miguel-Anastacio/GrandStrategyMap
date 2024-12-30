// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/MapObject.h"
#include "MapEditor.h"
#include "TextureCompiler.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "BlueprintLibrary/FilePickerFunctionLibrary.h"

#if WITH_EDITOR
void UMapObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	// PropertyChangedEvent.Property
	// TODO - CHECK WHICH PROPERTY CHANGED
	if(LookupTexture)
	{
		LookupTextureData = UTextureUtilsFunctionLibrary::ReadTextureToArray(LookupTexture);
	}
	
	if(StructType)
	{
		if(!UADStructUtilsFunctionLibrary::StructHasPropertyWithTypeCompatible<int32>(StructType, FName("ID")))
		{
			// THROW ERROR AT USER  FACE
			UE_LOG(LogInteractiveMapEditor, Error, TEXT("Struct has no property with type ID"));
			StructType = nullptr;
			this->PostEditChange();
		}
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
	{
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("Invalid Index on Update Tile Property '%s'"), *PropertyName);
		return;
	}
	
	if(!UADStructUtilsFunctionLibrary::SetPropertyValueInStruct<FString>(MapData[Index], PropertyName, NewValue))
	{
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("No Matching Property with the compatible values: '%s'"), *PropertyName);
	}
	else
	{
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("Property does not exist in struct '%s'"), *PropertyName);
	}
}

void UMapObject::SaveData() const
{
	UDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(FilePathMapData, StructType, MapData);
	// for(auto& Data : MapData)
	// {
	// 	FProperty* Property = Data.GetScriptStruct()->FindPropertyByName("ID");
	// 	if(Property)
	// 	{
	// 		bool bResult = false;
	// 		int32 value = UADStructUtilsFunctionLibrary::GetPropertyValue<int32>(Property, Data.GetMemory(), bResult);
	// 		UE_LOG(LogInteractiveMapEditor, Log, TEXT("TEST ID: %d"), value);
	// 	}
	// 	
	// 	FProperty* PropertyName = Data.GetScriptStruct()->FindPropertyByName("Name");
	// 	if(PropertyName)
	// 	{
	// 		bool bResult = false;
	// 		FString name = UADStructUtilsFunctionLibrary::GetPropertyValue<FString>(PropertyName, Data.GetMemory(), bResult);
	// 		UE_LOG(LogInteractiveMapEditor, Log, TEXT("TEST Name: %s"), *name);
	// 		FTestBasic test = UADStructUtilsFunctionLibrary::GetPropertyValue<FTestBasic>(PropertyName, Data.GetMemory(), bResult);
	// 		if(!bResult)
	// 		{
	// 			UE_LOG(LogInteractiveMapEditor, Error, TEXT("Struct Property is not of type requested"));
	// 		}
	// 	}
	// }
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
	
	SetMapDataFilePath(FilesNames[0]);
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

void UMapObject::SetMapDataFilePath(const FString& FilePath, bool LoadFromFile)
{
	FilePathMapData = FPaths::CreateStandardFilename(FilePath);
	if(LoadFromFile)
	{
		MapData = UDataManagerFunctionLibrary::LoadCustomDataFromJson(FilePathMapData, StructType);
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
			const int32 StructId = UADStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int32>(Data, "ID", bOutResult);
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
	return UTextureUtilsFunctionLibrary::GetColorFromUV(LookupTexture, Uv, LookupTextureData);
}

void UMapObject::LoadLookupMap(const FString& FilePath)
{
	const TArray<FLookupEntry> Lookup = UDataManagerFunctionLibrary::LoadCustomDataFromJson<FLookupEntry>(LookupFilePath);
	LookupTable.Empty();
	for(const auto& Entry : Lookup)
	{
		LookupTable.Emplace(UDataManagerFunctionLibrary::ConvertHexStringToRGB(Entry.Color), FCString::Atoi(*Entry.Name));
	}
	
	// for(const auto& Entry : LookupTable)
	// {
	// 	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Color:%s, ID: %d"), *Entry.Key.ToString(), Entry.Value)
	// }

}
#endif
