// Fill out your copyright notice in the Description page of Project Settings.

#include "MapObject.h"
// #include "MapEditor.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#if WITH_EDITOR
#include "BlueprintLibrary/FilePickerFunctionLibrary.h"
#endif

#if WITH_EDITOR
void UMapObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if(!PropertyChangedEvent.Property)
		return;
	
	const FName PropertyName =  PropertyChangedEvent.Property->GetFName();
	
	if(PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, LookupTexture))
	{
		LookupTextureData = UTextureUtilsFunctionLibrary::ReadTextureToArray(LookupTexture);
	}
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, StructType))
	{
		if(!StructType->IsChildOf(FBaseMapStruct::StaticStruct()))
		{
			// THROW ERROR AT USER  FACE
			UE_LOG(LogTemp, Error, TEXT("Struct type must inherit from FBaseMapStruct"));
			StructType = nullptr;
			this->PostEditChange();
		}
	}

	if(PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, LookupFilePath))
	{
		LoadLookupMap(LookupFilePath);
	}
	OnObjectChanged.Broadcast();
}
#endif



void UMapObject::LogMapData() const
{
	for(const auto& Data : MapData)
	{
		UE_LOG(LogTemp, Display, TEXT("Struct Name %s"), *Data.GetScriptStruct()->GetName());
		UADStructUtilsFunctionLibrary::ForEachProperty(Data, [&](const FProperty* Property)
		{
			const FString PropertyName = Property->GetAuthoredName();
			bool bOutResult = false;
			UE_LOG(LogTemp, Display, TEXT("Name: %s ; Value: %s"), *PropertyName,
				*UADStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(Data, PropertyName, bOutResult));
		});
	}
}

void UMapObject::LogLookupTable() const
{
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
		// UE_LOG(LogInteractiveMapEditor, Log, TEXT("Invalid Index on Update Tile Property '%s'"), *PropertyName);
		return;
	}

	UADStructUtilsFunctionLibrary::SetPropertyValueNestedInStructFromString(MapData[Index], PropertyName, NewValue);
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
#if WITH_EDITOR
	TArray<FString> FilesNames;
	UFilePickerFunctionLibrary::OpenFileDialogJson(FPaths::ProjectDir(), FilesNames);
	if(FilesNames.IsEmpty())
	{
		// UE_LOG(LogInteractiveMapEditor, Warning, TEXT("No file selected"));
		return;
	}
	if(!StructType)
	{
		// UE_LOG(LogInteractiveMapEditor, Error, TEXT("Struct Data Type not selected"));
		return;
	}
	
	SetMapDataFilePath(FilesNames[0]);
#endif
}

void UMapObject::SetMapData(const TArray<FInstancedStruct>& NewData)
{
	if(NewData.Num() > 0)
	{
		if(NewData[0].GetScriptStruct() != StructType)
		{
			// UE_LOG(LogInteractiveMapEditor, Error, TEXT("New data type is not of the same as StructType"));
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
		MapData = UDataManagerFunctionLibrary::LoadCustomDataFromJson(FilePathMapData, {StructType, OceanStructType});
	}
}

int UMapObject::GetIndexOfTileSelected(const FColor& Color)
{
	// UE_LOG(LogInteractiveMapEditor, Log, TEXT("FColor: %s"), *Color.ToString());
	// UE_LOG(LogInteractiveMapEditor, Log, TEXT("Lookup Table size: %d"), LookupTable.Num());
	if(const int32* ID = LookupTable.Find(Color))
	{
		// UE_LOG(LogInteractiveMapEditor, Log, TEXT("ID: %d"), *ID);
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
		// UE_LOG(LogInteractiveMapEditor, Error, TEXT("Color not in LookupTable"));
		return -1;
	}
	
	// UE_LOG(LogInteractiveMapEditor, Error, TEXT("Color in LookupTable but ID not in Map Data"));
	return -1;
}

FColor UMapObject::GetColorFromUv(const FVector2D& Uv) const 
{
	return UTextureUtilsFunctionLibrary::GetColorFromUV(LookupTexture, Uv, LookupTextureData);
}

void UMapObject::LoadLookupMap(const FString& FilePath)
{
	const TArray<FLookupEntry> Lookup = UDataManagerFunctionLibrary::LoadCustomDataFromJson<FLookupEntry>(FilePath);
	LookupTable.Empty();
	for(const auto& Entry : Lookup)
	{
		LookupTable.Emplace(UDataManagerFunctionLibrary::ConvertHexStringToRGB(Entry.Color), FCString::Atoi(*Entry.Name));
	}
}
