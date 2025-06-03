// Fill out your copyright notice in the Description page of Project Settings.

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
	Map = MakeShareable(new MapGenerator::Map(1024, 1024));
}

#if WITH_EDITOR

void UMapObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if(!PropertyChangedEvent.Property)
		return;
	
	const FName PropertyName =  PropertyChangedEvent.Property->GetFName();
	
	if(PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, LookupTexture))
	{
		LookupTextureData = UAtkTextureUtilsFunctionLibrary::ReadTextureToArray(LookupTexture);
		LoadLookupMap(LookupFilePath);
	}
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, StructType))
	{
		if(!UAtkStructUtilsFunctionLibrary::StructHasPropertyOfTypeWithName<int>(StructType, FName("ID")))
		{
			const FText Title = FText::FromString(TEXT("Error"));
			const FText Message = FText::FromString(TEXT("Struct type must have a numeric field named ID"));
			EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
			// this should keep the previous values
			StructType = nullptr;
			this->PostEditChange();
		}
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, OceanStructType))
	{
		if(!UAtkStructUtilsFunctionLibrary::StructHasPropertyOfTypeWithName<int>(OceanStructType, FName("ID")))
		{
			const FText Title = FText::FromString(TEXT("Error"));
			const FText Message = FText::FromString(TEXT("OceanStructType must have a numeric field named ID"));
			EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
			// this should keep the previous value
			OceanStructType = nullptr;
			this->PostEditChange();
		}
	}

	if(PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, LookupFilePath))
	{
		LoadLookupMap(LookupFilePath);
	}
	if(PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, VisualPropertiesDT))
	{
		if(VisualPropertyTypesDT)
		{
			ReadDataTables();
		}
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, VisualPropertyTypesDT))
	{
		if(VisualPropertiesDT)
		{
			ReadDataTables();
		}
	}
	
	OnObjectChanged.Broadcast();
	// SetMapDataFilePath(FilePathMapData);	
}
TSharedPtr<MapGenerator::Map> UMapObject::GetMapGen() const
{
	return Map;
}

void UMapObject::SetMapGen(TSharedPtr<MapGenerator::Map> MapGen)
{
	Map = MapGen;
}

UTexture2D* UMapObject::GetRootTexture() const
{
	return OriginTexture;
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
}

void UMapObject::SerializeMap(FArchive& Ar)
{
	int32 Version = 2;
	Ar << Version;

	if(Version >= 2)
	{
		unsigned width = Map->Width();  
		unsigned height = Map->Height();
		Ar << width;
		Ar << height;

		if(Ar.IsSaving())
		{
			std::vector<MapGenerator::Tile> tiles = Map->GetTiles();
			int32 count = static_cast<int32>(tiles.size());
			Ar << count;
			TArray<TArray<uint8>> ThreadBuffers;
			int32 NumThreads = 8;
			ThreadBuffers.SetNum(NumThreads);

			ParallelFor(NumThreads, [&](int32 Index)
			{
				TArray<uint8>& Buffer = ThreadBuffers[Index];
				FMemoryWriter Writer(Buffer);

				int32 ChunkSize = tiles.size() / NumThreads;
				int32 Start = Index * ChunkSize;
				int32 End = (Index == NumThreads - 1) ? tiles.size() : Start + ChunkSize;

				for (int32 i = Start; i < End; ++i)
				{
					SerializeTile(tiles[i], Writer);  // Manual serialize per field
				}
			});
			Ar << NumThreads;
			for (int32 i = 0; i < NumThreads; ++i)
			{
				int32 Size = ThreadBuffers[i].Num();
				Ar << Size;
				Ar.Serialize(ThreadBuffers[i].GetData(), Size);
			}
		}
		
		else
		{
			int count = 0;
			int NumThreads = 0;
			
			Ar << count;
			Ar << NumThreads;

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

			// Step 2: Deserialize in parallel
			ParallelFor(NumThreads, [&](int32 Index)
			{
				TArray<uint8>& Buffer = ThreadBuffers[Index];
				FMemoryReader Reader(Buffer);

				int32 ChunkSize = count / NumThreads;
				int32 Start = Index * ChunkSize;
				int32 End = (Index == NumThreads - 1) ? count : Start + ChunkSize;

				for (int32 i = Start; i < End; ++i)
				{
					SerializeTile(tiles[i], Reader);
				}
			});
			Map->SetSize(width, height);
			Map->SetLookupTileMap(tiles);
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
	// SaveData();
#endif
}

void UMapObject::PostLoad()
{
	UObject::PostLoad();
#if WITH_EDITOR
	LoadLookupMap(LookupFilePath);
	SetMapDataFilePath(FilePathMapData);
	bMapSaved = true;
#endif
}

void UMapObject::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
#if WITH_EDITOR
	SerializeMap(Ar);
#endif
}


const TMap<FVisualPropertyType, FArrayOfVisualProperties>& UMapObject::GetVisualPropertiesMap() const
{
	return VisualPropertiesMap;
}

TMap<FName, FArrayOfVisualProperties> UMapObject::GetVisualPropertyNameMap() const
{
	TMap<FName, FArrayOfVisualProperties> VisualPropertiesNameMap;
	for(auto& VpType : VisualPropertiesMap)
	{
		VisualPropertiesNameMap.Emplace(VpType.Key.Type, VpType.Value);		
	}
	return VisualPropertiesNameMap;
}

FColor UMapObject::GetPropertyColorFromInstancedStruct(const FInstancedStruct& InstancedStruct,
                                                       const FName& PropertyName, bool& OutResult) const
{
	const FString PropertyValue = UAtkStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(InstancedStruct, PropertyName.ToString(), OutResult);

	if (!OutResult)
		return FColor::Black;
		
	return GetVisualProperty(PropertyName, FName(*PropertyValue), OutResult).Color;
}

void UMapObject::ReadDataTables()
{
	if(!VisualPropertiesDT|| !VisualPropertyTypesDT)
	{
		// UE_LOG(LogInteractiveMap, Warning, TEXT("ReadDataTables() failed - null datatables"));
		return;
	}
	
	VisualPropertiesMap.Empty();	
	TArray<FVisualPropertyType*> AllTypes;
	if(UAtkDataManagerFunctionLibrary::ReadDataTableToArray(VisualPropertyTypesDT, AllTypes))
	{
	}
	TArray<FVisualProperty*> VisualProperties;
	UAtkDataManagerFunctionLibrary::ReadDataTableToArray(VisualPropertiesDT, VisualProperties);
	
	for(const auto& Type : AllTypes)
	{
		FArrayOfVisualProperties ArrayOf;
		for(const auto& Property : VisualProperties)
		{
			if(Type->Type == Property->Type)
			{
				ArrayOf.VisualProperties.Emplace(*Property);
			}
		}
		VisualPropertiesMap.Emplace(*Type, ArrayOf);
	}
}

FVisualProperty UMapObject::GetVisualProperty(const FName& Type, const FName& Tag, bool& OutResult) const
{
	OutResult = false;
	const FArrayOfVisualProperties* PropertiesOfType = VisualPropertiesMap.Find(FVisualPropertyType(Type));
	if(!PropertiesOfType)
	{
		return FVisualProperty();
	}
	
	for(const FVisualProperty& VisualProperty : PropertiesOfType->VisualProperties)
	{
		if(VisualProperty.Tag == Tag)
		{
			OutResult = true;
			return VisualProperty;
		}
	}

	return FVisualProperty();
}

FVisualProperty UMapObject::GetVisualProperty(const FVisualPropertyType& Type, const FName& Tag, bool& OutResult) const
{
	OutResult = false;
	const FArrayOfVisualProperties* PropertiesOfType = VisualPropertiesMap.Find(Type);
	if(!PropertiesOfType)
	{
		return FVisualProperty();
	}
	
	for(const FVisualProperty& VisualProperty : PropertiesOfType->VisualProperties)
	{
		if(VisualProperty.Tag == Tag)
		{
			OutResult = true;
			return VisualProperty;
		}
	}

	return FVisualProperty();
}

TArray<FName> UMapObject::GetVisualPropertiesNamesOfType(const FName& Type) const
{
	const FArrayOfVisualProperties* PropertiesOfType = VisualPropertiesMap.Find(Type);
	if(!PropertiesOfType)
	{
		return TArray<FName>();
	}
	
	TArray<FName> Names;
	Names.Reserve(PropertiesOfType->VisualProperties.Num());
	for(const FVisualProperty& VisualProperty : PropertiesOfType->VisualProperties)
	{
		Names.Emplace(VisualProperty.Tag);	
	}
	return Names;
}

TSet<FName> UMapObject::GetNamesOfVisualPropertiesInMapData() const
{
	TSet<FName> Names;
	Names.Reserve(VisualPropertiesMap.Num());
	for(const auto& [VpType, Properties] : VisualPropertiesMap)
	{
		if(const FProperty* Property = UAtkStructUtilsFunctionLibrary::FindPropertyByDisplayName({OceanStructType, StructType}, VpType.Type))
		{
			// check if property is not numeric
			if(!Property->IsA(FNumericProperty::StaticClass()))
			{
				Names.Emplace(VpType.Type);
			}
		}
	}
	return Names;
}

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
		UE_LOG(LogTemp, Display, TEXT("Key: %s ; ID: %d"), *Color.ToString(), ID);
	}
}

void UMapObject::LogMapData() const
{
	UE_LOG(LogTemp, Display, TEXT("Logging %s MapData"), *GetName());
	UE_LOG(LogTemp, Display, TEXT("MapData Entries %d"), MapData.Num());
	for(const auto& [ID, Data] : MapData)
	{
		UE_LOG(LogTemp, Display, TEXT("Struct Type %s"), *Data.GetScriptStruct()->GetName());
		UAtkStructUtilsFunctionLibrary::ForEachProperty(Data, [&](const FProperty* Property)
		{
			const FString PropertyName = Property->GetAuthoredName();
			bool bOutResult = false;
			UE_LOG(LogTemp, Display, TEXT("Name: %s ; Value: %s"), *PropertyName,
				*UAtkStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(Data, PropertyName, bOutResult));
		});
	}
}

void UMapObject::LogVisualProperties() const
{
	for(const auto& [PropertyType, Properties] : VisualPropertiesMap)
	{
		UE_LOG(LogTemp, Display, TEXT("Property Type %s"), *PropertyType.Type.ToString());
		for(const auto& Property : Properties.VisualProperties)
		{
			UE_LOG(LogTemp, Display, TEXT("%s"), *Property.Tag.ToString());
		}
	}
}


#if WITH_EDITOR

void UMapObject::SaveData() const
{
	TArray<FInstancedStruct> Values;
	MapData.GenerateValueArray(Values);
	UAtkDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(FilePathMapData, Values);
}

void UMapObject::LoadDataFromFile()
{
	TArray<FString> FilesNames;
	UAtkFilePickerFunctionLibrary::OpenFileDialogJson(FPaths::ProjectDir(), FilesNames);
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
}
void UMapObject::SetLookupTexture(UTexture2D* Texture2D)
{
	if(!Texture2D)
		return;

	LookupTexture = Texture2D;
	LookupTextureData = UAtkTextureUtilsFunctionLibrary::ReadTextureToArray(LookupTexture);
}

void UMapObject::SetMapDataFilePath(const FString& FilePath, bool LoadFromFile)
{
	FilePathMapData = FPaths::CreateStandardFilename(FilePath);
	TArray<FInstancedStruct> StructData;
	if(LoadFromFile)
	{
		StructData = UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson(FilePathMapData, {StructType, OceanStructType});
	}
	TMap<int, FInstancedStruct> DataMap;
	for(const auto& Data: StructData)
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

FColor UMapObject::GetColorFromUv(const FVector2D& Uv) const 
{
	return UAtkTextureUtilsFunctionLibrary::GetColorFromUV(LookupTexture, Uv, LookupTextureData);
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
void UMapObject::UpdateDataInEditor(const FInstancedStruct& NewData)
{
	if(NewData.GetScriptStruct()->IsChildOf(FBaseMapStruct::StaticStruct()))
	{
		bool bResult = false;
		const int32 ID = UAtkStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int32>(NewData, "ID", bResult);
		if(bResult)
		{
			MapData[ID] = NewData;
			MarkPackageDirty();
		}
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

UDataTable* UMapObject::GetVisualPropertyTypes() const
{
	return VisualPropertyTypesDT;
}

#endif
