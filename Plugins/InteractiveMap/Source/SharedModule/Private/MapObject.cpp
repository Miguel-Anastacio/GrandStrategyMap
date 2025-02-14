// Fill out your copyright notice in the Description page of Project Settings.

#include "MapObject.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#if WITH_EDITOR
#include "BlueprintLibrary/FilePickerFunctionLibrary.h"
#include "UObject/ObjectSaveContext.h"
#include "AssetToolsModule.h"
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
		LoadLookupMap(LookupFilePath);
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
	SetMapDataFilePath(FilePathMapData);	
}

#endif
void UMapObject::PreSave(FObjectPreSaveContext SaveContext)
{
	UObject::PreSave(SaveContext);
#if WITH_EDITOR
	SaveData();
#endif
}

void UMapObject::PostInitProperties()
{
	UObject::PostInitProperties();
	IAssetTools& AssetTools =FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
}

void UMapObject::PostLoad()
{
	UObject::PostLoad();
#if WITH_EDITOR
	LoadLookupMap(LookupFilePath);
	SetMapDataFilePath(FilePathMapData);
#endif
}

void UMapObject::LogMapData() const
{
	UE_LOG(LogTemp, Display, TEXT("Logging %s MapData"), *GetName());
	UE_LOG(LogTemp, Display, TEXT("MapData Entries %d"), MapData.Num());
	for(const auto& [ID, Data] : MapData)
	{
		UE_LOG(LogTemp, Display, TEXT("Struct Type %s"), *Data.GetScriptStruct()->GetName());
		UADStructUtilsFunctionLibrary::ForEachProperty(Data, [&](const FProperty* Property)
		{
			const FString PropertyName = Property->GetAuthoredName();
			bool bOutResult = false;
			UE_LOG(LogTemp, Display, TEXT("Name: %s ; Value: %s"), *PropertyName,
				*UADStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(Data, PropertyName, bOutResult));
		});
	}
}

bool UMapObject::IsTileOfType(int ID, const UScriptStruct* ScriptStruct) const
{
	for(const auto& [ItemID, Data] : MapData)
	{
		bool bOutResult = false;
		// const int LocalID = UADStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int>(Data, "ID", bOutResult);
		if(bOutResult && ID == ItemID && ScriptStruct == Data.GetScriptStruct())
		{
			return true;
		}
	}

	return false;
}

bool UMapObject::IsTileWater(int ID) const
{
	return IsTileOfType(ID, StructType);
}

bool UMapObject::IsTileLand(int ID) const
{
	return IsTileOfType(ID, OceanStructType);
}

void UMapObject::LogLookupTable() const
{
	UE_LOG(LogTemp, Display, TEXT("Logging %s LookupTable"), *GetName());
	UE_LOG(LogTemp, Display, TEXT("LookupTable Entries %d"), LookupTable.Num());
	for(const auto& [Color, ID]: LookupTable)
	{
		UE_LOG(LogTemp, Display, TEXT("Key: %s ; ID: %d"), *Color.ToString(), ID);
	}
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
	TArray<FInstancedStruct> Values;
	MapData.GenerateValueArray(Values);
	UDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(FilePathMapData, Values);
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
		// MapData = NewData;
	}
}

#if WITH_EDITOR
void UMapObject::SetLookupTexture(UTexture2D* Texture2D)
{
	if(!Texture2D)
		return;

	LookupTexture = Texture2D;
	LookupTextureData = UTextureUtilsFunctionLibrary::ReadTextureToArray(LookupTexture);
}
#endif

void UMapObject::SetMapDataFilePath(const FString& FilePath, bool LoadFromFile)
{
	FilePathMapData = FPaths::CreateStandardFilename(FilePath);
	TArray<FInstancedStruct> StructData;
	if(LoadFromFile)
	{
		StructData = UDataManagerFunctionLibrary::LoadCustomDataFromJson(FilePathMapData, {StructType, OceanStructType});
	}
	for(const auto& Data: StructData)
	{
		if(Data.IsValid())
		{
			bool bOutResult = false;
			int32 ID = UADStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int32>(Data, "ID", bOutResult);
			if(bOutResult)
			{
				MapData.Emplace(ID, Data);
			}
		}
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
		if(FInstancedStruct* Data = MapData.Find(*ID))
		{
			return *ID;
		}
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

void UMapObject::UpdateData(const FInstancedStruct& NewData)
{
	if(NewData.GetScriptStruct()->IsChildOf(FBaseMapStruct::StaticStruct()))
	{
		bool bResult = false;
		const int32 ID = UADStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int32>(NewData, "ID", bResult);
		if(bResult && ID < MapData.Num())
		{
			MapData[ID] = NewData;
			MarkPackageDirty();
			// SaveData();
		}
	}
}
