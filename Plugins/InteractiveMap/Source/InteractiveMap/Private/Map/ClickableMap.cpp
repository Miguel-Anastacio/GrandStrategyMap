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
#include "DataManager/DataManagerFunctionLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
//#include "InteractiveMap.h"
// Sets default values
AClickableMap::AClickableMap(const FObjectInitializer& ObjectInitializer)
	: AActor(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MapRoot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MapRoot"));
	RootComponent = MapRoot;

	//MapVisualComponent = CreateDefaultSubobject<UMapVisualComponent>(TEXT("Map Visual"));
	//MapVisualComponent->SetupAttachment(RootComponent);
	//MapVisualComponent->AttachMeshes(RootComponent);

	MapDataComponent = CreateDefaultSubobject<UMapDataComponent>(TEXT("Map Data"));

	PoliticalMapTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Dynamic Texture"));
	ReligiousMapTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Religious Map Texture"));
	CultureMapTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Culture Map Texture"));

}

void AClickableMap::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
void AClickableMap::InitializeMap_Implementation()
{
	if (!MapLookUpTexture)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Map Look up Texture not assigned"));
		return;
	}
	//// Data
	MapDataComponent->CreateLookUpTable();
	MapDataComponent->ReadDataTables();
	MapDataComponent->SetCountryProvinces();

	// Visual
	int32 width = MapLookUpTexture->GetSizeX();
	int32 height = MapLookUpTexture->GetSizeY();
	PoliticalMapTextureComponent->InitializeTexture(width, height);
	ReligiousMapTextureComponent->InitializeTexture(width, height);
	CultureMapTextureComponent->InitializeTexture(width, height);

	// Visual Data
	SaveMapTextureData();
	CreateMapTexture(PoliticalMapTextureComponent);
	CreateMapTexture(ReligiousMapTextureComponent);
	CreateMapTexture(CultureMapTextureComponent);

	MapVisualComponent->GetMapGameplayMeshComponent()->SetMaterial(0, PoliticalMapTextureComponent->DynamicMaterial);
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

void AClickableMap::SaveMapTextureData()
{
	if (!IsValid(MapLookUpTexture))
		return;

	// Get the dimensions of the texture
	int32 Width = MapLookUpTexture->GetSizeX();
	int32 Height = MapLookUpTexture->GetSizeY();
	MapColorCodeTextureData.AddDefaulted(Width * Height * 4);

	// Lock the texture for reading
	FTexture2DMipMap& Mip = MapLookUpTexture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);
	
	const uint8* Data = static_cast<const uint8*>(TextureData);

	// Read color of each pixel
	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			int32 Index = (Y * Width + X) * 4; // 4 bytes per pixel (RGBA)
			uint8 B = Data[Index];
			uint8 G = Data[Index + 1];
			uint8 R = Data[Index + 2];
			uint8 A = Data[Index + 3];

			SetPixelColorInt(Index, MapColorCodeTextureData, FColor(R, G, B, A));

			FColor pixelColor = FColor(0, 0, 0, 0);
			FName* id = MapDataComponent->LookUpTable.Find(FVector(R, G, B));
			if (id)
			{
				FProvinceData* province = GetProvinceData(*id);
				if (!province)
				{
					UE_LOG(LogInteractiveMap, Error, TEXT("Error province present in look up table but not in province map data"));

					// Unlock the texture
					Mip.BulkData.Unlock();
					MapLookUpTexture->UpdateResource();
					return;
				}

				PoliticalMapTextureComponent->SetPixelValue(X, Y, MapDataComponent->GetCountryColor(province));
				ReligiousMapTextureComponent->SetPixelValue(X, Y, MapDataComponent->GetReligionColor(province));
				CultureMapTextureComponent->SetPixelValue(X, Y, MapDataComponent->GetCultureColor(province));

			}
			else
			{

				PoliticalMapTextureComponent->SetPixelValue(X, Y, pixelColor);
				ReligiousMapTextureComponent->SetPixelValue(X, Y, pixelColor);
				CultureMapTextureComponent->SetPixelValue(X, Y, pixelColor);
			}

		}
	}

	// Unlock the texture
	Mip.BulkData.Unlock();
	MapLookUpTexture->UpdateResource();
}

// Called every frame
void AClickableMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClickableMap::CreateMapTexture(UDynamicTextureComponent* textureCompoment)
{
	textureCompoment->UpdateTexture();

	if (!GameplayMapMaterial)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Gameplay Map Material not assigned"));
	}
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(GameplayMapMaterial, this);
	textureCompoment->DynamicMaterial = DynMaterial;
	textureCompoment->DynamicMaterial->SetTextureParameterValue("DynamicTexture", textureCompoment->GetTexture());
	textureCompoment->DynamicMaterial->SetTextureParameterValue("LookUpTexture", MapLookUpTexture);
	// Use a regular texture not a virtual texture
	textureCompoment->DynamicMaterial->SetScalarParameterValue("TextureType", 1.0f);
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
			FName* id = MapDataComponent->LookUpTable.Find(FVector(R, G, B));
			if (id)
			{
				FProvinceData* province = GetProvinceData(*id);
				if (!province)
				{
					UE_LOG(LogInteractiveMap, Error, TEXT("Error province present in look up table but not in province map data"));
					return;
				}

				switch (mode)
				{
				case MapMode::POLITICAL:
					textureCompoment->SetPixelValue(X, Y, MapDataComponent->GetCountryColor(province));
					break;
				case MapMode::RELIGIOUS:
					textureCompoment->SetPixelValue(X, Y, MapDataComponent->GetReligionColor(province));
					break;
				case MapMode::CULTURAL:
					textureCompoment->SetPixelValue(X, Y, MapDataComponent->GetCultureColor(province));
					break;
				case MapMode::TERRAIN:
					break;
				default:
					break;
				}
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
	
		oldColor = MapDataComponent->GetCountryColor(MapDataComponent->ProvinceDataMap.Find(provinceID));
		if (oldColor == FColor(0, 0, 0, 0))
		{
			UE_LOG(LogInteractiveMap, Error, TEXT("Province or Country data not found"));
			return;
		}

		textureComponent = PoliticalMapTextureComponent;

		break;
	case MapMode::RELIGIOUS:

		oldColor = MapDataComponent->GetReligionColor(MapDataComponent->ProvinceDataMap.Find(provinceID));
		if (oldColor == FColor(0, 0, 0, 0))
		{
			UE_LOG(LogInteractiveMap, Error, TEXT("Province data not found"));
			return;
		}

		textureComponent = ReligiousMapTextureComponent;

		break;
	case MapMode::CULTURAL:
		oldColor = MapDataComponent->GetCultureColor(MapDataComponent->ProvinceDataMap.Find(provinceID));
		if (oldColor == FColor(0, 0, 0, 0))
		{
			UE_LOG(LogInteractiveMap, Error, TEXT("Province data not found"));
			return;
		}

		textureComponent = CultureMapTextureComponent;

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
		textureComponent = PoliticalMapTextureComponent;
		break;
	case MapMode::RELIGIOUS:
		textureComponent = ReligiousMapTextureComponent;
		break;
	case MapMode::CULTURAL:
		textureComponent = CultureMapTextureComponent;
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
			FName* id = MapDataComponent->LookUpTable.Find(FVector(MapColorCodeTextureData[Index+2],
											MapColorCodeTextureData[Index + 1],
											MapColorCodeTextureData[Index]));
			if (!id)
				continue;

			bool found = false;
			// cycle through provinces to update 
			// if it matches the id of this pixel then update the color
			for (auto& idToUpdate : provinceIDs)
			{
				if ((*id) == idToUpdate)
				{
					pixelArray[Index + 2] = newColor.R;
					pixelArray[Index + 1] = newColor.G;
					pixelArray[Index + 0] = newColor.B;
					pixelArray[Index + 3] = newColor.A;
					found = true;
				}

				if (found)
					break;

			}
		}
	}
}

TMap<FVector, FName> AClickableMap::GetLookUpTable() const
{
	return MapDataComponent->GetLookUpTable();
}

TMap<FName, FProvinceData>* AClickableMap::GetProvinceDataMap()
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

FName AClickableMap::GetProvinceID(const FVector& color, bool& out_result) const
{
	return MapDataComponent->GetProvinceID(color, out_result);
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
	switch (mode)
	{
	case MapMode::POLITICAL:

		if (mesh)
		{
			mesh->SetMaterial(0, PoliticalMapTextureComponent->DynamicMaterial);
		}
		break;
	case MapMode::RELIGIOUS:

		if (mesh)
		{
			mesh->SetMaterial(0, ReligiousMapTextureComponent->DynamicMaterial);
		}

		break;
	case MapMode::CULTURAL:

		if (mesh)
		{
			mesh->SetMaterial(0, CultureMapTextureComponent->DynamicMaterial);
		}
		break;
	case MapMode::TERRAIN:

		break;
	default:
		break;
	}
}

// to be able to use in BP
void AClickableMap::GetProvinceData(FName name, FProvinceData& out_data) const 
{
	MapDataComponent->GetProvinceData(name, out_data);
}

FProvinceData* AClickableMap::GetProvinceData(FName name) 
{
	return MapDataComponent->GetProvinceData(name);
}

void AClickableMap::SetBorderVisibility(bool status)
{
	if(bUseBorderMesh)
		MapVisualComponent->GetMapBorderMeshComponent()->SetVisibility(status);
}

bool AClickableMap::UpdateProvinceData(const FProvinceData& data, FName id) 
{
	FProvinceData* province = MapDataComponent->ProvinceDataMap.Find(id);
	MapMode mapToUpdate = MapMode::TERRAIN;
	FColor color = FColor();

	bool dataChanged = MapDataComponent->UpdateProvinceData(data, id, mapToUpdate, color);
	if (mapToUpdate != MapMode::TERRAIN)
	{
		MapDataChangedDelegate.Broadcast(mapToUpdate);
		return true;
	}
	
	return dataChanged;
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

	UpdatePixelArray(*PoliticalMapTextureComponent->GetTextureData(), country->Color, color.ToFColor(true), PoliticalMapTextureComponent->GetTexture(), country->Provinces);
	country->Color = color.ToFColor(true);

	PoliticalMapTextureComponent->UpdateTexture();
	return true;

}

void AClickableMap::UpdateBorder(UMaterialInstanceDynamic* material, UTextureRenderTarget2D* renderTarget)
{
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), renderTarget, material);
	PoliticalMapTextureComponent->DynamicMaterial->SetTextureParameterValue("BorderTexture", renderTarget);
	ReligiousMapTextureComponent->DynamicMaterial->SetTextureParameterValue("BorderTexture", renderTarget);
	CultureMapTextureComponent->DynamicMaterial->SetTextureParameterValue("BorderTexture", renderTarget);
	TerrainDynamicMaterial->SetTextureParameterValue("BorderTexture", renderTarget);
}

FColor AClickableMap::GetColorFromLookUpTexture(FVector2D uv) const
{
	return GetColorFromUV(MapLookUpTexture, uv);
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
		if(PoliticalMapTextureComponent->DynamicMaterial)
			PoliticalMapTextureComponent->DynamicMaterial->SetVectorParameterValue("ProvinceHighlighted", color);
		break;
	case MapMode::RELIGIOUS:
		if(ReligiousMapTextureComponent->DynamicMaterial)
			ReligiousMapTextureComponent->DynamicMaterial->SetVectorParameterValue("ProvinceHighlighted", color);
		break;
	case MapMode::CULTURAL:
		if(CultureMapTextureComponent->DynamicMaterial)
			CultureMapTextureComponent->DynamicMaterial->SetVectorParameterValue("ProvinceHighlighted", color);
		break;
	case MapMode::TERRAIN:
		if(TerrainDynamicMaterial)
			TerrainDynamicMaterial->SetVectorParameterValue("ProvinceHighlighted", color);
		break;
	default:
		break;
	}
}

FColor AClickableMap::GetColorFromUV(UTexture2D* texture, FVector2D uv) const
{
	if(!texture)
		return FColor();

	int32 width = texture->GetSizeX();
	int32 height = texture->GetSizeY();

	//int32 Index = (y * Width + x) * 4;
	int32 y = uv.Y * height;
	int32 x = uv.X * width;

	int32 index = (y * width + x) * 4;
	
	index = index % 4 + index;

	if (index < 0 || index > width * height * 4 - 4)
	{
		UE_LOG(LogInteractiveMap, Warning, TEXT("Invalid index from uvs"));
		return FColor();
	}

	FColor color = FColor(MapColorCodeTextureData[index+2],
		MapColorCodeTextureData[index + 1],
		MapColorCodeTextureData[index],
		MapColorCodeTextureData[index + 3]);

	return color;
}