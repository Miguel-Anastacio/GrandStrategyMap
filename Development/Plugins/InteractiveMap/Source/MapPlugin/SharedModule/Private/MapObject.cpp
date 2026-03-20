// Copyright 2024 An@stacioDev All rights reserved.

#include "MapObject.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "BlueprintLibrary/MiscFunctionLibrary.h"
#include "SharedModule.h"
#if WITH_EDITOR
#include "UObject/ObjectSaveContext.h"
#endif

UMapObject::UMapObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
#if WITH_EDITOR
	MapGenSaved = MakeShareable(new MapGenerator::Map(1024, 1024, "MapGenLog.txt"));
#endif
}

#if WITH_EDITOR

void UMapObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if(!PropertyChangedEvent.Property)
		return;
	
	const FName PropertyName =  PropertyChangedEvent.Property->GetFName();
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, StructType))
	{
		bool bSuccess = false;
		if(ValidateStructChange(StructType, StructTypePrevious))
		{
			bSuccess = ProcessStructChange(StructType, StructTypePrevious);
		}
		
		if(bSuccess)
		{
			StructTypePrevious = StructType;
		}
		else
		{
			StructType = StructTypePrevious;
			this->PostEditChange();
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, OceanStructType))
	{
		bool bSuccess = false;
		if(ValidateStructChange(OceanStructType, OceanStructTypePrevious))
		{
			bSuccess = ProcessStructChange(OceanStructType, OceanStructTypePrevious);
		}
		
		if(bSuccess)
		{
			OceanStructTypePrevious = OceanStructType;
		}
		else
		{
			OceanStructType = OceanStructTypePrevious;
			this->PostEditChange();
		}
	}
}
TSharedPtr<MapGenerator::Map> UMapObject::GetMapGen() const
{
	return MapGenSaved;
}

void UMapObject::SetMapGen(TSharedPtr<MapGenerator::Map> MapGen)
{
	MapGenSaved = MapGen;
}

TSharedPtr<MapGenerator::Map> UMapObject::GetLastMapGen() const
{
	return LastMapGen;
}

void UMapObject::SetLastMapGen(const TSharedPtr<MapGenerator::Map>& MapGen)
{
	LastMapGen = MapGen;
}

UTexture2D* UMapObject::GetRootTexture() const
{
	if (!OriginTexture.Get())
	{
		return OriginTexture.LoadSynchronous();
	}
	return OriginTexture.Get();
}

void UMapObject::SetRootTexture(UTexture2D* texture)
{
	OriginTexture = texture;
}

FMapGenParams UMapObject::GetLastParamsUsed() const
{
	return  LastParamsUsedGen;
}

void UMapObject::SetLastParamsUsed(const FMapGenParams& Params)
{
	LastParamsUsedGen = Params;
}

bool UMapObject::IsMapSaved() const
{
	return bMapSaved;
}

void UMapObject::SetMapSaved(bool Saved)
{
	bMapSaved = Saved;
	if(Saved)
	{
		Counter = 0;
	}
}

void UMapObject::IncrementCounter()
{
	Counter++;
}

#endif
void UMapObject::PreSave(FObjectPreSaveContext SaveContext)
{
	UObject::PreSave(SaveContext);
#if WITH_EDITOR
	SaveData();
#endif
}

void UMapObject::PostLoad()
{
	UObject::PostLoad();
#if WITH_EDITOR
	LoadLookupMap(LookupFilePath);
	SetMapDataFilePath(FilePathMapData);
	ReconstructMapGenSavedFromData();
	bMapSaved = true;
	StructTypePrevious = StructType;
	OceanStructTypePrevious = OceanStructType;
#endif
}


TArray<TObjectPtr<UVisualProperty>> UMapObject::GetVisualProperties()
{
	VisualPropertyInstances.Empty();
	for(const TSubclassOf<UVisualProperty> VisualPropertyClass : VisualProperties)
	{
		if (UVisualProperty* Instance = NewObject<UVisualProperty>(this, VisualPropertyClass))
		{
			VisualPropertyInstances.Add(Instance);
		}
	}
	return VisualPropertyInstances;
}

FColor UMapObject::GetPropertyColorFromInstancedStruct(const FInstancedStruct& InstancedStruct,
                                                       const FName& PropertyName, bool& OutResult) const
{
	for(const auto& Property : VisualPropertyInstances)
	{
		if(Property->Name == PropertyName)
		{
			OutResult = true;
			return Property->GetColorForProperty(InstancedStruct);
		}
	}
	OutResult = false;
	return FColor::Black;
}

# if WITH_EDITOR

void UMapObject::InitLandStructType(UScriptStruct* NewStruct)
{
	StructType = NewStruct;
	StructTypePrevious = StructType;
}

void UMapObject::InitOceanStructType(UScriptStruct* NewStruct)
{
	OceanStructType = NewStruct;
	OceanStructTypePrevious = OceanStructType;
}
#endif

bool UMapObject::IsTileOfType(int32 ID, const UScriptStruct* ScriptStruct) const
{
	if(const FInstancedStruct* Data = MapData.Find(ID))
	{
		return ScriptStruct == Data->GetScriptStruct();
	}
	return false;
}

bool UMapObject::IsTileWater(int32 ID) const
{
	return IsTileOfType(ID, StructType);
}

bool UMapObject::IsTileLand(int32 ID) const
{
	return IsTileOfType(ID, OceanStructType);
}

bool UMapObject::IsStructValid(const FInstancedStruct& Struct) const
{
	return IsStructValid(Struct.GetScriptStruct());
}
bool UMapObject::IsStructValid(const UScriptStruct* Struct) const
{
	if(!Struct)
		return false;
	return Struct == StructType || Struct == OceanStructType;
}

void UMapObject::LogLookupTable() const
{
	UE_LOG(LogMapSharedModule, Display, TEXT("Logging %s LookupTable"), *GetName());
	UE_LOG(LogMapSharedModule, Display, TEXT("LookupTable Entries %d"), LookupTable.Num());
	for(const auto& [Color, ID]: LookupTable)
	{
		UE_LOG(LogMapSharedModule, Display, TEXT("ID: %d Key: %s"), ID,  *Color.ToString());
	}
}

void UMapObject::LogMapData() const
{
	UE_LOG(LogMapSharedModule, Display, TEXT("Logging %s MapData"), *GetName());
	UE_LOG(LogMapSharedModule, Display, TEXT("MapData Entries %d"), MapData.Num());
	for(const auto& [ID, Data] : MapData)
	{
		UAtkStructUtilsFunctionLibrary::LogInstancedStruct(Data);
	}
}

#if WITH_EDITOR

void UMapObject::SaveData() const
{
	TArray<FInstancedStruct> Values;
	MapData.GenerateValueArray(Values);
	UAtkDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(FilePathMapData, Values);
}

void UMapObject::SetLookupTexture(UTexture2D* Texture2D)
{
	if(!Texture2D)
		return;

	LookupTexture = Texture2D;
	LoadLookupTextureData();
}

TWeakObjectPtr<UTexture2D> UMapObject::GetLookupTexture() const
{
	if (!LookupTexture.Get())
	{
		return LookupTexture.LoadSynchronous();
	}
	return LookupTexture.Get();
}

void UMapObject::SetMapDataFilePath(const FString& FilePath)
{
	FilePathMapData = FPaths::CreateStandardFilename(FilePath);
	const TArray<FInstancedStruct> StructData = UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson(FilePathMapData, {StructType, OceanStructType});
	SetMapData(StructData);
}

void UMapObject::SetFilePathMapData(const FString& FilePath)
{
	FilePathMapData = FilePath;
}

void UMapObject::SetMapData(const TArray<FInstancedStruct>& NewData)
{
	TMap<int32, FInstancedStruct> DataMap;
	for(const auto& Data: NewData)
	{
		if(Data.IsValid())
		{
			bool bOutResult = false;
			int32 ID = UAtkStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int32>(Data, "ID", bOutResult);
			if(bOutResult)
			{
				DataMap.Emplace(ID, Data);
			}
		}
	}
	MapData = DataMap;
	OnMapDataChanged.ExecuteIfBound();
}
#endif

int32 UMapObject::FindID(const FColor& Color)
{
	if(const int32* ID = FindIDPtr(Color))
	{
		return *ID;
	}
	return -1;
}

int32* UMapObject::FindIDPtr(const FColor& Color)
{
	return LookupTable.Find(Color);
}

FColor UMapObject::GetColor(int32 ID) const
{
	for(const auto& [Color, Id] : LookupTable)
	{
		if(ID == Id)
		{
			return Color;
		}
	}
	return FColor(0, 0, 0, 0);
}

bool UMapObject::SetTileData(const FInstancedStruct& NewData, int32 ID)
{
	if(FInstancedStruct* Data = GetTileData(ID))
	{
		*Data = NewData;
		return true;
	}
	return false;
}


FInstancedStruct* UMapObject::GetTileData(int32 ID)
{
	return  MapData.Find(ID);
}

TArray<uint8> UMapObject::GetLookupTextureData()
{
	if(LookupTextureData.IsEmpty())
	{
		LoadLookupTextureData();
	}
	return LookupTextureData;
}

void UMapObject::LoadLookupTextureData()
{
	LookupTextureData = UAtkTextureUtilsFunctionLibrary::ReadTextureToArray(GetLookupTexture().Get());
}

void UMapObject::LoadLookupMap(const FString& FilePath)
{
	bool bOutResult = false;
	const TArray<FLookupEntry> Lookup = UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson<FLookupEntry>(FilePath, bOutResult);
	if (!bOutResult)
	{
		UE_LOG(LogMapSharedModule, Error, TEXT("Failed to load lookup map from %s"), *FilePath);
		return;
	}
	SetLookupTableFromEntries(Lookup);
}

#if WITH_EDITOR
bool UMapObject::UpdateDataInEditor(const FInstancedStruct& NewData, const int32 ID)
{
	if(FInstancedStruct* Data = GetTileData(ID))
	{
		if(Data->GetScriptStruct() == NewData.GetScriptStruct())
		{
			*Data = NewData;
			UAtkStructUtilsFunctionLibrary::SetPropertyValueInStruct(*Data, "ID", ID);
			return true;
		}
	}
	return false;
}

void UMapObject::ReconstructMapGenSavedFromData()
{
	const TArray<uint8> TextureData = UAtkTextureUtilsFunctionLibrary::ReadTextureToArray(LookupTexture.Get());
	if (!LookupTexture)
		return;
	
	MapGenSaved->SetSize(LookupTexture->GetSizeX(), LookupTexture->GetSizeY());
	MapGenSaved->CreateLookupMap();
	MapGenSaved->forEachTileInLookUpMap([&](MapGenerator::Tile& tile, unsigned bufferIndex)
	{
		const FColor Color = UAtkTextureUtilsFunctionLibrary::GetColorFromIndex(bufferIndex, TextureData);
		const int32 ID = FindID(Color);
		const FInstancedStruct* Data = GetTileData(ID);
		if(!Data)
			return;
		
		tile.color = MapGenerator::Color(Color.R, Color.G, Color.B, Color.A);
		
		if(Data->GetScriptStruct() == StructType)
		{
			tile.type = MapGenerator::TileType::LAND;
		}
		else if (Data->GetScriptStruct() == OceanStructType)
		{
			tile.type = MapGenerator::TileType::WATER;
		}
	});
	
	OnMapDataChanged.ExecuteIfBound();
}

void UMapObject::SetLookupTableFromEntries(const TArray<FLookupEntry>& LookupEntries)
{
	LookupTable.Empty();
	for(const auto& [Color, Name] : LookupEntries)
	{
		LookupTable.Emplace(UAtkMiscFunctionLibrary::ConvertHexStringToRGB(Color), FCString::Atoi(*Name));
	}
}

void UMapObject::SetLookupFilePath(const FString& FilePath, const TArray<FLookupEntry>& LookupEntries)
{
	LookupFilePath = FilePath;
	SetLookupTableFromEntries(LookupEntries);
}

void UMapObject::UpdateDataInEditor(const FInstancedStruct& NewData, const TArray<int32>& IDs)
{
	bool update = false;
	for(const auto id : IDs)
	{
		update |= UpdateDataInEditor(NewData, id);
	}
	
	if(update)
	{
		OnMapDataChanged.ExecuteIfBound();
	}
}

void UMapObject::SetMaterialOverride(UMaterialInterface* MaterialInterface)
{
	MaterialOverride = MaterialInterface;
}

UMaterialInterface* UMapObject::GetMaterialOverride() const
{
	return MaterialOverride;
}


void UMapObject::AddTileSelected(int32 ID)
{
	SelectedTiles.Emplace(ID);
	OnTilesSelectedChanged.ExecuteIfBound(SelectedTiles);
}

void UMapObject::AddTilesSelected(const TArray<int32>& IDs)
{
	SelectedTiles.Append(IDs);
	OnTilesSelectedChanged.ExecuteIfBound(SelectedTiles);
}

void UMapObject::RemoveTileSelected(int32 ID)
{
	SelectedTiles.Remove(ID);
	OnTilesSelectedChanged.ExecuteIfBound(SelectedTiles);
}

void UMapObject::ClearTilesSelected(const bool triggerNotification)
{
	SelectedTiles.Empty();
	if(triggerNotification)
	{
		OnTilesSelectedChanged.ExecuteIfBound(SelectedTiles);
	}
}

const TArray<int32>& UMapObject::GetTilesSelected() const
{
	return SelectedTiles;
}

bool UMapObject::IsTileSelected(int32 ID) const
{
	return SelectedTiles.Find(ID) != INDEX_NONE;
}

void UMapObject::ReplaceDataMap(const UScriptStruct* NewStruct, const UScriptStruct* OldStruct)
{
	bool Update = false;
	for(auto& [id, data] : MapData)
	{
		if(data.GetScriptStruct() == OldStruct)
		{
			data = FInstancedStruct(NewStruct);
			UAtkStructUtilsFunctionLibrary::SetPropertyValueInStruct(data, FString("ID"), id);
			Update = true;
		}
	}
	if(Update)
	{
		OnMapDataChanged.ExecuteIfBound();
	}
}

bool UMapObject::ValidateStructChange(UScriptStruct* NewStruct, UScriptStruct* OldStruct)
{
	if(!UAtkStructUtilsFunctionLibrary::StructHasPropertyOfTypeWithName<int>(NewStruct, FName("ID")))
	{
		const FText Title = FText::FromString(TEXT("Error"));
		const FText Message = FText::FromString(TEXT("Struct type must have a numeric field named ID"));
		EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
		NewStruct = OldStruct;
		this->PostEditChange();
		return false;
	}
	return true;
}

bool UMapObject::ProcessStructChange(const UScriptStruct* NewStruct, const UScriptStruct* OldStruct)	
{
	// replace all data that used this struct type with the new data
	const FText Title = FText::FromString(TEXT("Struct Type Replacing"));
	const FText Message = FText::FromString(TEXT("This will replace all entries in MapData with the default for this new Struct. Do you wish to continue?"));
	EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo, Message, Title);
	if(Result == EAppReturnType::Yes)
	{
		ReplaceDataMap(NewStruct, OldStruct);
		return true;
	}
	else
	{
		return false;
	}
}

#endif
