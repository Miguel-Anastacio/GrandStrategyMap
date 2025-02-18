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
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMapObject, OceanStructType))
	{
		if(!OceanStructType->IsChildOf(FBaseMapStruct::StaticStruct()))
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
	const FString PropertyValue = UADStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(InstancedStruct, PropertyName.ToString(), OutResult);

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
	if(UDataManagerFunctionLibrary::ReadDataTableToArray(VisualPropertyTypesDT, AllTypes))
	{
	}
	TArray<FVisualProperty*> VisualProperties;
	UDataManagerFunctionLibrary::ReadDataTableToArray(VisualPropertiesDT, VisualProperties);
	
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
		if(const FProperty* Property = UADStructUtilsFunctionLibrary::FindPropertyByDisplayName({OceanStructType, StructType}, VpType.Type))
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

// TMap<FName, TSet<FName>> UMapObject::VpMappedByType() const
// {
// 	TMap<FName, TSet<FName>> Map;
// 	Map.Reserve(VisualPropertiesMap.Num());
// 	for(const auto& [VpType, Property] : VisualPropertiesMap)
// 	{
// 		TSet<FName> Names;
// 		Names.Reserve(Property.VisualProperties.Num());
// 		for(const FVisualProperty& VisualProperty : Property.VisualProperties)
// 		{
// 			Names.Emplace(VisualProperty.Type);
// 		}
// 		Map.Emplace(VpType, Names);
// 	}
//
// 	return Map;
// }

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
		UADStructUtilsFunctionLibrary::ForEachProperty(Data, [&](const FProperty* Property)
		{
			const FString PropertyName = Property->GetAuthoredName();
			bool bOutResult = false;
			UE_LOG(LogTemp, Display, TEXT("Name: %s ; Value: %s"), *PropertyName,
				*UADStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(Data, PropertyName, bOutResult));
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
	UDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(FilePathMapData, Values);
}

void UMapObject::LoadDataFromFile()
{
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
}
void UMapObject::SetLookupTexture(UTexture2D* Texture2D)
{
	if(!Texture2D)
		return;

	LookupTexture = Texture2D;
	LookupTextureData = UTextureUtilsFunctionLibrary::ReadTextureToArray(LookupTexture);
}

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

#if WITH_EDITOR
void UMapObject::UpdateDataInEditor(const FInstancedStruct& NewData)
{
	if(NewData.GetScriptStruct()->IsChildOf(FBaseMapStruct::StaticStruct()))
	{
		bool bResult = false;
		const int32 ID = UADStructUtilsFunctionLibrary::GetPropertyValueFromStruct<int32>(NewData, "ID", bResult);
		if(bResult)
		{
			MapData[ID] = NewData;
			MarkPackageDirty();
		}
	}
}
#endif
