// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/ClickableMap.h"
#include "Map/MapUtils.h"
#include "Engine/Texture2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Map/DynamicTextureComponent.h"
#include "Game/MapPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Map/Visual/LayeredMapVisualComponent.h"
#include "Map/MapDataComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "InteractiveMap.h"
#include "MapObject.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"

AClickableMap::AClickableMap(const FObjectInitializer& ObjectInitializer)
	: AActor(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MapRoot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MapRoot"));
	RootComponent = MapRoot;

	MapDataComponent = CreateDefaultSubobject<UMapDataComponent>(TEXT("Map Data"));

	DynamicTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Dynamic Texture"));
}

void AClickableMap::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
void AClickableMap::InitializeMap_Implementation()
{
	// LOAD FROM MAP ASSET
	LoadMapAsset(MapAsset);
	if (!MapLookUpTexture)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Map Look up Texture not assigned"));
		return;
	}
	MapDataComponent->SetCountryProvinces();

	DynamicTextureComponent->InitializeTexture(MapLookUpTexture->GetSizeX(), MapLookUpTexture->GetSizeY());
	CreateMapModes();
	UDynamicTexture* DynamicTextureTest = *MapModesTextureComponents.Find("Country");
	DynamicTextureComponent->SetDynamicTexture(DynamicTextureTest);
	DynamicTextureComponent->UpdateTexture();
	// DynamicTextureComponent->InitializeTexture();
	
	DynamicTextureComponent->GetMaterialInstance()->SetTextureParameterValue("DynamicTexture", DynamicTextureComponent->GetTexture());
	// DynamicTextureComponent->GetMaterialInstance()->SetTextureParameterValue("DynamicTexture", MapLookUpTexture);
	DynamicTextureComponent->GetMaterialInstance()->SetTextureParameterValue("LookUpTexture", MapLookUpTexture);
	DynamicTextureComponent->GetMaterialInstance()->SetScalarParameterValue("TextureType", 1.0f);

	SetMapMode_Implementation(MapMode::POLITICAL);
	
	MapDataChangedDelegate.AddDynamic(this, &AClickableMap::UpdateMapTexture);

	// set referene in player class
	AMapPawn* player = Cast<AMapPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player)
	{
		player->SetInteractiveMap(this);
	}

	if (!TerrainMapMaterial)
		UE_LOG(LogInteractiveMap, Error, TEXT("Terrain Map Material is not valid"));

	// Terrain Material
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(TerrainMapMaterial, this);
	TerrainDynamicMaterial = DynMaterial;
	if (!TerrainTexture)
		UE_LOG(LogInteractiveMap, Error, TEXT("Terrain Texture is not valid"));
	if (TerrainDynamicMaterial)
	{
		TerrainDynamicMaterial->SetTextureParameterValue("TerrainTexture", TerrainTexture); 
		TerrainDynamicMaterial->SetScalarParameterValue("TextureType", 0.0f);
		TerrainDynamicMaterial->SetTextureParameterValue("LookUpTexture", MapLookUpTexture);
	}
}
// Called when the game starts or when spawned
void AClickableMap::BeginPlay()
{
	Super::BeginPlay();

	InitializeMap_Implementation();
}

void AClickableMap::SetPixelColorInt(int index, TArray<uint8>& pixelArray, const FColor& color)
{
	pixelArray[index ] = color.B;
	pixelArray[index + 1] = color.G;
	pixelArray[index + 2] = color.R;
	pixelArray[index + 3] = color.A;
}

void AClickableMap::CreateMapModes()
{
	TArray<FVisualPropertyType> Types;
	MapDataComponent->GetVisualPropertiesMap().GetKeys(Types);
	CreateDynamicTextures(Types);
	FillDynamicTextures(MapDataComponent->GetVisualPropertyNameMap(), MapAsset->GetLookupTextureData());
}
// #if WITH_EDITOR
void AClickableMap::CreateDynamicTextures( const TArray<FVisualPropertyType>& VisualPropertyTypes)
{
	MapLookUpTexture = MapAsset->LookupTexture;
	if(!MapLookUpTexture)
		return;
	
	const int32 Width = MapLookUpTexture->GetSizeX();
	const int32 Height = MapLookUpTexture->GetSizeY();
	for(const auto& VisualPropertyType : VisualPropertyTypes)
	{
		const FName ComponentName = FName(*FString::Printf(TEXT("DynamicTextureComponent_%s"), *VisualPropertyType.Type.ToString()));
		UDynamicTexture* DynamicTexture = NewObject<UDynamicTexture>();

		DynamicTexture->InitializeDynamicTexture(Width, Height);
		DynamicTexture->DynamicMaterial = UMaterialInstanceDynamic::Create(VisualPropertyType.MaterialInstance, this);
		DynamicTexture->DynamicMaterial->SetTextureParameterValue("LookUpTexture", MapLookUpTexture);
		// Use a regular texture not a virtual texture
		DynamicTexture->DynamicMaterial->SetScalarParameterValue("TextureType", 1.0f);
		
		MapModesTextureComponents.Emplace(VisualPropertyType.Type, DynamicTexture);
	}
}

void AClickableMap::FillDynamicTextures(const TMap<FName, FArrayOfVisualProperties>& VisualProperties, const TArray<uint8>& LookupTextureData)
{
	const int32 Width = MapLookUpTexture->GetSizeX();
	const int32 Height = MapLookUpTexture->GetSizeY();
	if(LookupTextureData.Num() != Width*Height*4)
		return;
	
	// Read color of each pixel
	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			const int32 Index = (Y * Width + X) * 4; // 4 bytes per pixel (RGBA)
			const uint8 B = LookupTextureData[Index];
			const uint8 G = LookupTextureData[Index + 1];
			const uint8 R = LookupTextureData[Index + 2];
			const int* ID = MapDataComponent->FindId(FColor(R, G, B));
			if (ID)
			{
				FInstancedStruct* InstancedStruct = GetProvinceData(*ID);
				if (!InstancedStruct)
				{
					UE_LOG(LogInteractiveMap, Error, TEXT("Error province present in look up table but not in province map data"));
					return;
				}
				
				for (TFieldIterator<FProperty> It(MapAsset->StructType); It; ++It)
				{
					const FProperty* Property = *It;
					if (!Property)
					{
						continue;
					}
					// check if property is visual
					const FName PropertyTypeName = Property->GetFName();
					auto* AllPropertiesOfType = VisualProperties.Find(PropertyTypeName);
					if(!AllPropertiesOfType)
					{
						// UE_LOG(LogInteractiveMap, Error, TEXT("Property %s is visual but type is not convertible to string"), *PropertyTypeName.ToString())
						continue;
					}
					
					bool bResult = false;
					const FString Tag = UADStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(*InstancedStruct, PropertyTypeName.ToString(), bResult);
					if(!bResult)
					{
						UE_LOG(LogInteractiveMap, Error, TEXT("Property %s is visual but type is not convertible to string"), *PropertyTypeName.ToString())
						continue;
					}
					
					for(auto& VisualProperty : AllPropertiesOfType->VisualProperties)
					{
						if(Tag != VisualProperty.Tag)
						{
							continue;
						}
						// finally set pixels of texture component that matches this visual property type
						if(UDynamicTexture** TextureComponent = MapModesTextureComponents.Find(PropertyTypeName))
						{
							(*TextureComponent)->SetPixelValue(X, Y, VisualProperty.Color);
						}
					}
				}
			}
		}
	}
}
// #endif

//TODO - TO BE REMOVED
void AClickableMap::SaveMapTextureData()
{
	if (!IsValid(MapLookUpTexture))
		return;

	// Create Dynamic Texture Components
	
	
	
	// Get the dimensions of the texture
	const int32 Width = MapLookUpTexture->GetSizeX();
	const int32 Height = MapLookUpTexture->GetSizeY();
	if(MapColorCodeTextureData.Num() != Width*Height*4)
		return;
	
	// Read color of each pixel
	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			const int32 Index = (Y * Width + X) * 4; // 4 bytes per pixel (RGBA)
			const uint8 B = MapColorCodeTextureData[Index];
			const uint8 G = MapColorCodeTextureData[Index + 1];
			const uint8 R = MapColorCodeTextureData[Index + 2];
			
			// const FName* ID = MapDataComponent->LookUpTable.Find(FVector(R, G, B));
			const int* ID = MapDataComponent->FindId(FColor(R, G, B));
			if (ID)
			{
				FInstancedStruct* province = GetProvinceData(*ID);
				if (!province)
				{
					UE_LOG(LogInteractiveMap, Error, TEXT("Error province present in look up table but not in province map data"));
					return;
				}

			}
			else
			{

			}

		}
	}
}

// Called every frame
void AClickableMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClickableMap::CreateMapTexture(UDynamicTextureComponent* textureCompoment)
{
	textureCompoment->UpdateTexture();
	textureCompoment->GetMaterialInstance()->SetTextureParameterValue("DynamicTexture", textureCompoment->GetTexture());
	textureCompoment->GetMaterialInstance()->SetTextureParameterValue("LookUpTexture", MapLookUpTexture);
	textureCompoment->GetMaterialInstance()->SetScalarParameterValue("TextureType", 1.0f);
}

void AClickableMap::RefreshDynamicTextureDataBuffer(UDynamicTextureComponent* textureCompoment, MapMode mode)
{
	int32 Width = MapLookUpTexture->GetSizeX();
	int32 Height = MapLookUpTexture->GetSizeY();

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			int32 Index = (Y * Width + X) * 4; // 4 bytes per pixel (RGBA)
			uint8 B = MapColorCodeTextureData[Index];
			uint8 G = MapColorCodeTextureData[Index + 1];
			uint8 R = MapColorCodeTextureData[Index + 2];
			uint8 A = MapColorCodeTextureData[Index + 3];


			FColor pixelColor = FColor(0, 0, 0, 0);
			bool bResult = false;
			const int ID = GetProvinceID(FColor(R,G,B), bResult);
			if (bResult)
			{
				FInstancedStruct* province = GetProvinceData(ID);
				if (!province)
				{
					UE_LOG(LogInteractiveMap, Error, TEXT("Error province present in look up table but not in province map data"));
					return;
				}

				// switch (mode)
				// {
				// case MapMode::POLITICAL:
				// 	textureCompoment->SetPixelValue(X, Y, MapDataComponent->GetCountryColor(province));
				// 	break;
				// case MapMode::RELIGIOUS:
				// 	textureCompoment->SetPixelValue(X, Y, MapDataComponent->GetReligionColor(province));
				// 	break;
				// case MapMode::CULTURAL:
				// 	textureCompoment->SetPixelValue(X, Y, MapDataComponent->GetCultureColor(province));
				// 	break;
				// case MapMode::TERRAIN:
				// 	break;
				// default:
				// 	break;
				// }
			}
			else
			{
				textureCompoment->SetPixelValue(X, Y, pixelColor);
			}

		}
	}
}

void AClickableMap::UpdateMapTexturePerProvince(MapMode mode, FName provinceID, const FColor& newColor)
{
	FColor oldColor;
	UDynamicTextureComponent* textureComponent = nullptr;
	switch (mode)
	{
	case MapMode::POLITICAL:
	
		// oldColor = MapDataComponent->GetCountryColor(MapDataComponent->ProvinceDataMap.Find(provinceID));
		if (oldColor == FColor(0, 0, 0, 0))
		{
			UE_LOG(LogInteractiveMap, Error, TEXT("Province or Country data not found"));
			return;
		}

		// textureComponent = PoliticalMapTextureComponent;

		break;
	case MapMode::RELIGIOUS:

		// oldColor = MapDataComponent->GetReligionColor(MapDataComponent->ProvinceDataMap.Find(provinceID));
		if (oldColor == FColor(0, 0, 0, 0))
		{
			UE_LOG(LogInteractiveMap, Error, TEXT("Province data not found"));
			return;
		}

		// textureComponent = ReligiousMapTextureComponent;

		break;
	case MapMode::CULTURAL:
		// oldColor = MapDataComponent->GetCultureColor(MapDataComponent->ProvinceDataMap.Find(provinceID));
		if (oldColor == FColor(0, 0, 0, 0))
		{
			UE_LOG(LogInteractiveMap, Error, TEXT("Province data not found"));
			return;
		}

		// textureComponent = CultureMapTextureComponent;

		break;
	case MapMode::TERRAIN:

		break;
	default:
		break;
	}

	if (!textureComponent)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Invalid Map Mode"));
		return;
	}

	//UpdatePixelArray(*textureComponent->GetTextureData(), oldColor, newColor, textureComponent->GetTexture(), {provinceID});
	RefreshDynamicTextureDataBuffer(textureComponent, mode);
	textureComponent->UpdateTexture();
}
void AClickableMap::UpdateMapTexture(MapMode mode)
{
	UDynamicTextureComponent* textureComponent = nullptr;
	switch (mode)
	{
	case MapMode::POLITICAL:
		// textureComponent = PoliticalMapTextureComponent;
		break;
	case MapMode::RELIGIOUS:
		// textureComponent = ReligiousMapTextureComponent;
		break;
	case MapMode::CULTURAL:
		// textureComponent = CultureMapTextureComponent;
		break;
	case MapMode::TERRAIN:
		break;
	default:
		break;
	}

	if (!textureComponent)
		return;

	RefreshDynamicTextureDataBuffer(textureComponent, mode);
	textureComponent->UpdateTexture();
}
void AClickableMap::UpdatePixelArray(TArray<uint8>& pixelArray, const FColor& oldColor, const FColor& newColor, const UTexture2D* texture, const TArray<FName>& provinceIDs)
{
	int32 Width = texture->GetSizeX();
	int32 Height = texture->GetSizeY();

	if (pixelArray.Num() != Width * Height * 4)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Mismatch size between texture and pixel array when updating texture"));
		return;
	}

	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++)
		{
			int32 Index = (y * Width + x) * 4;
			
			//FColor(30, )
			FColor color = FColor(pixelArray[Index + 2],
				pixelArray[Index + 1],
				pixelArray[Index],
				pixelArray[Index + 3]);

			if (color != oldColor)
				continue;

			// get province id from the array that holds the original look up texture pixel data
			// FName* id = MapDataComponent->NewLookupTable.Find(FVector(MapColorCodeTextureData[Index+2],
			// 								MapColorCodeTextureData[Index + 1],
			// 								MapColorCodeTextureData[Index]));
			// if (!id)
			// 	continue;
			//
			// bool found = false;
			// // cycle through provinces to update 
			// // if it matches the id of this pixel then update the color
			// for (auto& idToUpdate : provinceIDs)
			// {
			// 	if ((*id) == idToUpdate)
			// 	{
			// 		pixelArray[Index + 2] = newColor.R;
			// 		pixelArray[Index + 1] = newColor.G;
			// 		pixelArray[Index + 0] = newColor.B;
			// 		pixelArray[Index + 3] = newColor.A;
			// 		found = true;
			// 	}
			//
			// 	if (found)
			// 		break;
			//
			// }
		}
	}
}

TMap<FColor, int> AClickableMap::GetLookUpTable() const
{
	return MapDataComponent->GetLookUpTable();
}

TMap<int, FInstancedStruct>* AClickableMap::GetProvinceDataMap() const
{
	return MapDataComponent->GetProvinceDataMap();
}
TMap<FName, FCountryData>* AClickableMap::GetCountryDataMap()
{
	return  MapDataComponent->GetCountryDataMap();
}

TMap<FName, FColoredData>* AClickableMap::GetVisualPropertiesDataMap()
{
	return MapDataComponent->GetVisualPropertiesDataMap();
}

int AClickableMap::GetProvinceID(const FColor& Color, bool& bOutResult) const
{
	return MapDataComponent->GetProvinceID(Color, bOutResult);
}

void AClickableMap::SetMapMode_Implementation(MapMode mode)
{
	UStaticMeshComponent* mesh = MapVisualComponent->GetMapGameplayMeshComponent();
	SetMeshMaterial(mode, mesh);
	MapModeChangedDelegate.Broadcast(CurrentMapMode, mode);
	CurrentMapMode = mode;
}

void AClickableMap::SetMeshMaterial(MapMode mode, UStaticMeshComponent* mesh)
{
	if(mesh)
	{
		mesh->SetMaterial(0, DynamicTextureComponent->GetMaterialInstance());
	}
	switch (mode)
	{
	case MapMode::POLITICAL:

		if (mesh)
		{
			// mesh->SetMaterial(0, PoliticalMapTextureComponent->DynamicMaterial);
		}
		break;
	case MapMode::RELIGIOUS:

		if (mesh)
		{
			// mesh->SetMaterial(0, ReligiousMapTextureComponent->DynamicMaterial);
		}

		break;
	case MapMode::CULTURAL:

		if (mesh)
		{
			// mesh->SetMaterial(0, CultureMapTextureComponent->DynamicMaterial);
		}
		break;
	case MapMode::TERRAIN:

		break;
	default:
		break;
	}
}

// to be able to use in BP
void AClickableMap::GetProvinceData(int ID, FInstancedStruct& OutData) const 
{
	MapDataComponent->GetProvinceData(ID, OutData);
}

FInstancedStruct* AClickableMap::GetProvinceData(int ID) 
{
	return MapDataComponent->GetProvinceData(ID);
}

void AClickableMap::SetBorderVisibility(bool status)
{
	if(bUseBorderMesh)
		MapVisualComponent->GetMapBorderMeshComponent()->SetVisibility(status);
}

bool AClickableMap::UpdateProvinceData(const FInstancedStruct& Data, int ID)
{
	FInstancedStruct* province = MapDataComponent->ProvinceDataMap.Find(ID);
	MapMode mapToUpdate = MapMode::TERRAIN;
	FColor color = FColor();

	// bool dataChanged = MapDataComponent->UpdateProvinceData(data, id, mapToUpdate, color);
	if (mapToUpdate != MapMode::TERRAIN)
	{
		MapDataChangedDelegate.Broadcast(mapToUpdate);
		return true;
	}
	
	// return dataChanged;
	return false;
}

bool AClickableMap::UpdateCountryData(const FCountryData& data, FName id)
{
	return MapDataComponent->UpdateCountryData(data, id);
}

bool AClickableMap::UpdateCountryColor(const FLinearColor& color, FName id)
{
	FCountryData* country = MapDataComponent->CountryDataMap.Find(id);
	
	if (!country)
		return false;

	// UpdatePixelArray(*PoliticalMapTextureComponent->GetTextureData(), country->Color, color.ToFColor(true), PoliticalMapTextureComponent->GetTexture(), country->Provinces);
	country->Color = color.ToFColor(true);

	// PoliticalMapTextureComponent->UpdateTexture();
	return true;

}

void AClickableMap::UpdateBorder(UMaterialInstanceDynamic* material, UTextureRenderTarget2D* renderTarget)
{
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), renderTarget, material);
	// PoliticalMapTextureComponent->DynamicMaterial->SetTextureParameterValue("BorderTexture", renderTarget);
	// ReligiousMapTextureComponent->DynamicMaterial->SetTextureParameterValue("BorderTexture", renderTarget);
	// CultureMapTextureComponent->DynamicMaterial->SetTextureParameterValue("BorderTexture", renderTarget);
	TerrainDynamicMaterial->SetTextureParameterValue("BorderTexture", renderTarget);
}

FColor AClickableMap::GetColorFromLookUpTexture(FVector2D uv) const
{
	// return GetColorFromUV(MapLookUpTexture, uv);
	return UTextureUtilsFunctionLibrary::GetColorFromUV(MapLookUpTexture, uv, MapColorCodeTextureData);
}

void AClickableMap::SetBorderLookUpTexture(UMaterialInstanceDynamic* borderMat, UDynamicTextureComponent* textureComponent)
{
	if (!textureComponent)
	{
		borderMat->SetTextureParameterValue("LookUpTexture", MapLookUpTexture);
		return;
	}

	borderMat->SetTextureParameterValue("LookUpTexture", textureComponent->GetTexture());
}

void AClickableMap::UpdateProvinceHovered(const FColor& color)
{
	switch (CurrentMapMode)
	{
	case MapMode::POLITICAL:
		// if(PoliticalMapTextureComponent->DynamicMaterial)
			// PoliticalMapTextureComponent->DynamicMaterial->SetVectorParameterValue("ProvinceHighlighted", color);
		break;
	case MapMode::RELIGIOUS:
		// if(ReligiousMapTextureComponent->DynamicMaterial)
			// ReligiousMapTextureComponent->DynamicMaterial->SetVectorParameterValue("ProvinceHighlighted", color);
		break;
	case MapMode::CULTURAL:
		// if(CultureMapTextureComponent->DynamicMaterial)
			// CultureMapTextureComponent->DynamicMaterial->SetVectorParameterValue("ProvinceHighlighted", color);
		break;
	case MapMode::TERRAIN:
		if(TerrainDynamicMaterial)
			TerrainDynamicMaterial->SetVectorParameterValue("ProvinceHighlighted", color);
		break;
	default:
		break;
	}
}

void AClickableMap::LoadMapAsset(UMapObject* MapObject)
{
	// Load Texture
	if(!MapObject)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Map Object is NULL, please provide a Map Object"));
		return;
	}
	MapLookUpTexture = MapObject->LookupTexture;
	MapColorCodeTextureData = MapObject->GetLookupTextureData();
	

	// Load Data
	MapDataComponent->LoadFromMapObject(MapObject);
	
}
