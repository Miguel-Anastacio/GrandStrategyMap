// Copyright 2024 An@stacioDev All rights reserved.

#include "MapObject.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "BlueprintLibrary/MiscFunctionLibrary.h"
#if WITH_EDITOR
#include "BlueprintLibrary/FilePickerFunctionLibrary.h"
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
		if(ValidateStructChange(StructType, StructTypePrevious))
		{
			ProcessStructChange(StructType, StructTypePrevious);
		}
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, OceanStructType))
	{
		if(ValidateStructChange(OceanStructType, OceanStructTypePrevious))
		{
			ProcessStructChange(OceanStructType, OceanStructTypePrevious);
		}
	}
	
	// SetMapDataFilePath(FilePathMapData);	
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

// Lag probably due to this
void UMapObject::SerializeMap(FArchive& Ar)
{
	// Skip serialization during editor operations that don't need it
	if (Ar.IsTransacting() || Ar.IsPersistent() == false)
	{
		return; // Don't serialize during undo/redo or temporary operations
	}
	
	int32 Version = 2;
	Ar << Version;

	if(Version >= 2)
	{
		unsigned width = MapGenSaved->Width();  
		unsigned height = MapGenSaved->Height();
		Ar << width;
		Ar << height;
		int32 NumThreads = 8;
		Ar << NumThreads;

		if(Ar.IsSaving())
		{
			std::vector<MapGenerator::Tile> tiles = MapGenSaved->GetTiles();
			int32 count = static_cast<int32>(tiles.size());
			Ar << count;
			
			TArray<TArray<uint8>> ThreadBuffers;
			ThreadBuffers.SetNum(NumThreads);
			
			SerializeParallel<FMemoryWriter>(NumThreads, ThreadBuffers, tiles);
			for (int32 i = 0; i < NumThreads; ++i)
			{
				int32 Size = ThreadBuffers[i].Num();
				Ar << Size;
				Ar.Serialize(ThreadBuffers[i].GetData(), Size);
			}
		}
		else if(Ar.IsLoading())
		{
			int count = 0;
			Ar << count;

			std::vector<MapGenerator::Tile> tiles;
			tiles.resize(count);

			TArray<TArray<uint8>> ThreadBuffers;
			ThreadBuffers.SetNum(NumThreads);

			// Step 1: Read each chunk
			for (int32 i = 0; i < NumThreads; ++i)
			{
				int32 Size = 0;
				Ar << Size;
				ThreadBuffers[i].SetNumUninitialized(Size);
				Ar.Serialize(ThreadBuffers[i].GetData(), Size);
			}
			SerializeParallel<FMemoryReader>(NumThreads, ThreadBuffers, tiles);
			
			MapGenSaved->SetSize(width, height);
			MapGenSaved->SetLookupTileMap(tiles);
		}
	}
}

void SerializeTile(MapGenerator::Tile& Tile, FArchive& Ar)
{
	uint8 r, g, b, a;
	int32 cx, cy;
	int32 type = static_cast<int32>(Tile.type); // store enum as int

	if (Ar.IsSaving())
	{
		r = Tile.color.R;
		g = Tile.color.G;
		b = Tile.color.B;
		a = Tile.color.A;

		cx = Tile.centroid.x;
		cy = Tile.centroid.y;
	}

	Ar << r << g << b << a;
	Ar << Tile.visited;
	Ar << type;
	Ar << Tile.isBorder;
	Ar << cx << cy;

	if (Ar.IsLoading())
	{
		Tile.color = MapGenerator::Color(r, g, b, a);
		Tile.type = static_cast<MapGenerator::TileType>(type);
		Tile.centroid = mygal::Vector2<int>(cx, cy);
	}

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
	bMapSaved = true;
	StructTypePrevious = StructType;
	OceanStructTypePrevious = OceanStructType;
#endif
}

void UMapObject::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
#if WITH_EDITOR
	SerializeMap(Ar);
#endif
}


TArray<TObjectPtr<UVisualProperty>> UMapObject::GetVisualProperties()
{
	if(VisualPropertyInstances.IsEmpty())
	{
		for(const TSubclassOf<UVisualProperty> VisualPropertyClass : VisualProperties)
		{
			if (UVisualProperty* Instance = NewObject<UVisualProperty>(this, VisualPropertyClass))
			{
				VisualPropertyInstances.Add(Instance);
			}
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
	UE_LOG(LogTemp, Display, TEXT("Logging %s LookupTable"), *GetName());
	UE_LOG(LogTemp, Display, TEXT("LookupTable Entries %d"), LookupTable.Num());
	for(const auto& [Color, ID]: LookupTable)
	{
		UE_LOG(LogTemp, Display, TEXT("ID: %d Key: %s"), ID,  *Color.ToString());
	}
}

void UMapObject::LogMapData() const
{
	UE_LOG(LogTemp, Display, TEXT("Logging %s MapData"), *GetName());
	UE_LOG(LogTemp, Display, TEXT("MapData Entries %d"), MapData.Num());
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

void UMapObject::SetMapDataFilePath(const FString& FilePath, bool LoadFromFile)
{
	FilePathMapData = FPaths::CreateStandardFilename(FilePath);
	TArray<FInstancedStruct> StructData;
	if(LoadFromFile)
	{
		StructData = UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson(FilePathMapData, {StructType, OceanStructType});
	}
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
	const TArray<FLookupEntry> Lookup = UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson<FLookupEntry>(FilePath);
	LookupTable.Empty();
	for(const auto& Entry : Lookup)
	{
		LookupTable.Emplace(UAtkMiscFunctionLibrary::ConvertHexStringToRGB(Entry.Color), FCString::Atoi(*Entry.Name));
	}
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
	for(auto& [id, data] : MapData)
	{
		if(data.GetScriptStruct() == OldStruct)
		{
			data = FInstancedStruct(NewStruct);
			UAtkStructUtilsFunctionLibrary::SetPropertyValueInStruct(data, FString("ID"), id);
		}
	}
}

bool UMapObject::ValidateStructChange(const UScriptStruct* NewStruct, const UScriptStruct* OldStruct)
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

void UMapObject::ProcessStructChange(const UScriptStruct* NewStruct, const UScriptStruct* OldStruct)	
{
	// replace all data that used this struct type with the new data
	const FText Title = FText::FromString(TEXT("Struct Type Replacing"));
	const FText Message = FText::FromString(TEXT("This will replace all entries in MapData with the default for this new Struct. Do you wish to continue?"));
	EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo, Message, Title);
	if(Result == EAppReturnType::Yes)
	{
		ReplaceDataMap(NewStruct, OldStruct);
		OldStruct = NewStruct;
		OnMapDetailsChange.ExecuteIfBound();
	}
	else
	{
		NewStruct = OldStruct;
		this->PostEditChange();
	}
}

#endif
