// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/InteractiveMap.h"
#include "Map/MapUtils.h"
#include "Engine/Texture2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Map/DynamicTextureComponent.h"
#include "Game/MapPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
// Sets default values
AInteractiveMap::AInteractiveMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MapRoot = CreateDefaultSubobject<UArrowComponent>(TEXT("Root"));
	RootComponent = MapRoot;

	MapSelectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map Selection"));
	MapSelectMesh->SetupAttachment(RootComponent);

	MapBorderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map Border"));
	MapBorderMesh->SetupAttachment(RootComponent);

	GameplayMapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gameplay Map"));
	GameplayMapMesh->SetupAttachment(RootComponent);


	TerrainMapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Terrain Map"));
	TerrainMapMesh->SetupAttachment(RootComponent);

	PoliticalMapTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Dynamic Texture"));
	ReligiousMapTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Religious Map Texture"));
	CultureMapTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Culture Map Texture"));
	
}

UE_DISABLE_OPTIMIZATION
void AInteractiveMap::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
// Called when the game starts or when spawned
void AInteractiveMap::BeginPlay()
{
	Super::BeginPlay();
	GameplayMapMesh->SetVisibility(true);
	PoliticalMapTextureComponent->InitializeTexture(MapLookUpTexture->GetSizeX(), MapLookUpTexture->GetSizeY());
	ReligiousMapTextureComponent->InitializeTexture(MapLookUpTexture->GetSizeX(), MapLookUpTexture->GetSizeY());
	CultureMapTextureComponent->InitializeTexture(MapLookUpTexture->GetSizeX(), MapLookUpTexture->GetSizeY());

	CreateLookUpTable();
	ReadProvinceDataTable();
	ReadCountryDataTable();
	SaveMapTextureData();

	//CreatePoliticalMapTexture();
	CreateMapTexture(PoliticalMapTextureComponent, PixelColorPoliticalTexture, PoliticalMapTexture);
	CreateMapTexture(ReligiousMapTextureComponent, PixelColorReligiousTexture, ReligiousMapTexture);
	CreateMapTexture(CultureMapTextureComponent, PixelColorCultureMapTexture, CultureMapTexture);

	//PoliticalMapTextureComponent-
	//if (PoliticalMapTexture)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Political map texture is valid"));
	//}
	//PoliticalMapTextureComponent->FillTexture(FLinearColor::Blue);
	//PoliticalMapTextureComponent->UpdateTexture();

	GameplayMapMesh->SetMaterial(0, PoliticalMapTextureComponent->DynamicMaterial);

	// set referene in player class
	AMapPawn* player = Cast<AMapPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player)
	{
		player->SetInteractiveMap(this);
	}
	UMaterialInstanceDynamic* borderMaterialInstance = UMaterialInstanceDynamic::Create(BorderMaterial, this);
	//BorderMaterialRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 1024, 1024);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), BorderMaterialRenderTarget, borderMaterialInstance);

	UMaterialInstanceDynamic* filteredBorderMaterialInstance = UMaterialInstanceDynamic::Create(HQXFilterMaterial, this);
	filteredBorderMaterialInstance->SetTextureParameterValue("BorderTexture", BorderMaterialRenderTarget);
	MapBorderMesh->SetMaterial(0, filteredBorderMaterialInstance);



}

void AInteractiveMap::SetPixelColor(int index, TArray<float>& pixelArray, uint8 R, uint8 G, uint8 B, uint8 A)
{
	pixelArray[index] = R;
	pixelArray[index + 1] = G;
	pixelArray[index + 2] = B;
	pixelArray[index + 3] = A;
}
void AInteractiveMap::SetPixelColor(int index, TArray<float>& pixelArray, const FColor& color)
{
	pixelArray[index] = color.R;
	pixelArray[index + 1] = color.G;
	pixelArray[index + 2] = color.B;
	pixelArray[index + 3] = color.A;
}
bool AInteractiveMap::GetCountryColor(const FVector& color, FColor& out_countryColor)
{
	FName* id = LookUpTable.Find(color);
	if (id)
	{
		FProvinceData* province = ProvinceDataMap.Find(*id);
		if (!province)
			return false;
		FCountryData* country = CountryData.Find(province->Owner);
		if (!country)
			return false;

		out_countryColor = country->Color;
		return true;
	}
	else
	{
		//SetPixelColor(index, PixelColorPoliticalTexture, 0, 0, 0, 0);
		return false;
	}
	return true;
}

FColor AInteractiveMap::GetCountryColor(const FProvinceData* data)
{
	FColor color = FColor(0, 0, 0, 0);
	if (data)
	{
		FCountryData* country = CountryData.Find(data->Owner);
		if (!country)
			return color;

		return country->Color;
	}
	return color;
}

FColor AInteractiveMap::GetReligionColor(const FProvinceData* data)
{
	FColor color = FColor(0, 0, 0, 0);

	if (data)
	{
		return data->Religion.Color;
	}
	return FColor(0, 0, 0, 0);
}
FColor AInteractiveMap::GetCultureColor(const FProvinceData* data)
{
	FColor color = FColor(0, 0, 0, 0);
	if (data)
	{
		return data->Culture.Color;
	}
	return FColor(0, 0, 0, 0);
}
void AInteractiveMap::SaveMapTextureData()
{
	if (!IsValid(MapLookUpTexture))
		return;

	// Lock the texture for reading
	FTexture2DMipMap& Mip = MapLookUpTexture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);

	// Get the dimensions of the texture
	int32 Width = MapLookUpTexture->GetSizeX();
	int32 Height = MapLookUpTexture->GetSizeY();

	// Assume texture format is RGBA
	const uint8* Data = static_cast<const uint8*>(TextureData);

	PixelColorPoliticalTexture.AddDefaulted(Width * Height * 4);
	PixelColorReligiousTexture.AddDefaulted(Width * Height * 4);
	PixelColorCultureMapTexture.AddDefaulted(Width * Height * 4);

	MapColorCodeTextureData.PixelData.AddDefaulted(Width * Height * 4);

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

			SetPixelColor(Index, MapColorCodeTextureData.PixelData, FColor(R, G, B, A));

			FColor pixelColor = FColor(0, 0, 0, 0);
			FName* id = LookUpTable.Find(FVector(R, G, B));
			if (id)
			{
				FProvinceData* province = GetProvinceData(*id);
				if (!province)
				{
					UE_LOG(LogTemp, Error, TEXT("Error province present in look up table but not in province map data"));
					continue;
				}

				SetPixelColor(Index, PixelColorPoliticalTexture, GetCountryColor(province));
				SetPixelColor(Index, PixelColorReligiousTexture, GetReligionColor(province));
				SetPixelColor(Index, PixelColorCultureMapTexture, GetCultureColor(province));

				// this pixel belongs to a province so safe its indexes on the map
				//FName* indexPixel = MapColorCodeTextureData.PixedIndexID.Find((*id));
				
				MapColorCodeTextureData.PixedIndexID.Add(Index, (*id));

				if (!Religions.Contains(province->Religion.DataName))
				{
					Religions.Add(province->Religion.DataName, province->Religion.Color);
				}

				if (!Cultures.Contains(province->Culture.DataName))
				{
					Cultures.Add(province->Culture.DataName, province->Culture.Color);
				}
				
					
			}
			else
			{
				SetPixelColor(Index, PixelColorPoliticalTexture, pixelColor);
				SetPixelColor(Index, PixelColorReligiousTexture, pixelColor);
				SetPixelColor(Index, PixelColorCultureMapTexture, pixelColor);
			}

		}
	}

	// Unlock the texture
	Mip.BulkData.Unlock();
	/*MapLookUpTexture->CompressionSettings = OldCompressionSettings;
	MapLookUpTexture->MipGenSettings = OldMipGenSettings;
	MapLookUpTexture->SRGB = OldSRGB;*/
	MapLookUpTexture->UpdateResource();
}

// Called every frame
void AInteractiveMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AInteractiveMap::CreateLookUpTable()
{
	if (!MapDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Map table not loaded"));
		return;
	}

	TArray<FName> RowNames = MapDataTable->GetRowNames();
	TMap<TCHAR, int32> HexMap; 
	InitHexMap(HexMap);
	for (auto& name : RowNames)
	{
		FProvinceIDData* Item = MapDataTable->FindRow<FProvinceIDData>(name, "");
		if (Item)
		{
			FString temp = Item->Color;
			temp += FString("FF");
			FColor sRGBColor = Item->ConvertHexStringToRGB(temp, HexMap);

			LookUpTable.Add(FVector(sRGBColor.R, sRGBColor.G, sRGBColor.B), name);
			
		}
	}
}
void AInteractiveMap::ReadProvinceDataTable()
{
	if (!ProvinceDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Province Data not loaded"));
		return;
	}
	TArray<FName> RowNames = ProvinceDataTable->GetRowNames();
	for (auto& name : RowNames)
	{
		FProvinceData* Item = ProvinceDataTable->FindRow<FProvinceData>(name, "");
		if (Item)
		{
			ProvinceDataMap.Emplace(name, (*Item));
		}
	}

}
void AInteractiveMap::ReadCountryDataTable()
{
	if (!CountryDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Country Data not loaded"));
		return;
	}
	TArray<FName> RowNames = CountryDataTable->GetRowNames();
	for (auto& name : RowNames)
	{
		FCountryData* Item = CountryDataTable->FindRow<FCountryData>(name, "");
		if (Item)
		{
			CountryData.Emplace(FName(*Item->CountryName), (*Item));
		}
	}
}
void AInteractiveMap::CreatePoliticalMapTexture()
{
	PoliticalMapTextureComponent->DrawFromDataBuffer(0, 0, MapLookUpTexture, PixelColorPoliticalTexture);
	PoliticalMapTextureComponent->UpdateTexture();
	PoliticalMapTexture = PoliticalMapTextureComponent->GetTexture();

	// Set material
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(GameplayMapMaterial, this);
	PoliticalMapTextureComponent->DynamicMaterial = DynMaterial;
	PoliticalMapTextureComponent->DynamicMaterial->SetTextureParameterValue("DynamicTexture", PoliticalMapTexture);
}
void AInteractiveMap::CreateMapTexture(UDynamicTextureComponent* textureCompoment, const TArray<float>& pixelArray, UTexture2D* texture)
{
	textureCompoment->DrawFromDataBuffer(0, 0, MapLookUpTexture, pixelArray);
	textureCompoment->UpdateTexture();
	texture = textureCompoment->GetTexture();

	// Set material
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(GameplayMapMaterial, this);
	textureCompoment->DynamicMaterial = DynMaterial;
	textureCompoment->DynamicMaterial->SetTextureParameterValue("DynamicTexture", texture);
}

void AInteractiveMap::UpdateMapTexture(MapMode mode, FName provinceID, const FColor& newColor)
{
	FColor oldColor;
	UDynamicTextureComponent* textureComponent = nullptr;
	TArray<float>* dataBuffer = nullptr;
	switch (mode)
	{
	case MapMode::POLITICAL:
	
		oldColor = GetCountryColor(ProvinceDataMap.Find(provinceID));
		if (oldColor == FColor(0, 0, 0, 0))
		{
			UE_LOG(LogTemp, Error, TEXT("Province or Country data not found"));
			return;
		}

		textureComponent = PoliticalMapTextureComponent;
		dataBuffer = &PixelColorPoliticalTexture;

		break;
	case MapMode::RELIGIOUS:

		oldColor = GetReligionColor(ProvinceDataMap.Find(provinceID));
		if (oldColor == FColor(0, 0, 0, 0))
		{
			UE_LOG(LogTemp, Error, TEXT("Province data not found"));
			return;
		}

		textureComponent = ReligiousMapTextureComponent;
		dataBuffer = &PixelColorReligiousTexture;

		break;
	case MapMode::CULTURAL:
		oldColor = GetCultureColor(ProvinceDataMap.Find(provinceID));
		if (oldColor == FColor(0, 0, 0, 0))
		{
			UE_LOG(LogTemp, Error, TEXT("Province data not found"));
			return;
		}

		textureComponent = CultureMapTextureComponent;
		dataBuffer = &PixelColorCultureMapTexture;

		break;
	case MapMode::TERRAIN:

		break;
	default:
		break;
	}

	if (!textureComponent || !dataBuffer)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Map Mode"));
		return;
	}

	UpdatePixelArray(*dataBuffer, oldColor, newColor, textureComponent->GetTexture(), {provinceID});
	textureComponent->DrawFromDataBuffer(0, 0, textureComponent->GetTexture(), *dataBuffer);
	textureComponent->UpdateTexture();
}
void AInteractiveMap::UpdatePixelArray(TArray<float>& pixelArray, const FColor& oldColor, const FColor& newColor, const UTexture2D* texture, const TArray<FName>& provinceIDs)
{
	int32 Width = texture->GetSizeX();
	int32 Height = texture->GetSizeY();

	if (pixelArray.Num() != Width * Height * 4)
	{
		UE_LOG(LogTemp, Error, TEXT("Mismatch size between texture and pixel array when updating texture"));
		return;
	}

	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++)
		{
			int32 Index = (y * Width + x) * 4;
			
			//FColor(30, )
			FColor color = FColor(pixelArray[Index],
				pixelArray[Index + 1],
				pixelArray[Index + 2],
				pixelArray[Index + 3]);

			if (color != oldColor)
				continue;

			// get province id from the array that holds the original look up texture pixel data
			FName* id = LookUpTable.Find(FVector(MapColorCodeTextureData.PixelData[Index],
											MapColorCodeTextureData.PixelData[Index + 1],
											MapColorCodeTextureData.PixelData[Index + 2]));
			if (!id)
				continue;

			bool found = false;
			// cycle through provinces to update 
			// if it matches the id of this pixel then update the color
			for (auto& idToUpdate : provinceIDs)
			{
				if ((*id) == idToUpdate)
				{
					pixelArray[Index] = newColor.R;
					pixelArray[Index + 1] = newColor.G;
					pixelArray[Index + 2] = newColor.B;
					pixelArray[Index + 3] = newColor.A;
					found = true;
				}

				if (found)
					break;

			}
		}
	}
}

UTextureRenderTarget2D* AInteractiveMap::GetMapRenderTarget() const
{
	return MapRenderTarget;
}

TMap<FVector, FName> AInteractiveMap::GetLookUpTable() const
{
	return LookUpTable;
}

FName AInteractiveMap::GetProvinceID(const FVector& color) const
{
	const FName* ID = LookUpTable.Find(color);
	if (ID)
	{
		return (*ID);
	}

	return FName();
}


void AInteractiveMap::SetMapMode(MapMode mode)
{

	switch (mode)
	{
	case MapMode::POLITICAL:
		GameplayMapMesh->SetVisibility(true);
		GameplayMapMesh->SetMaterial(0, PoliticalMapTextureComponent->DynamicMaterial);
		break;
	case MapMode::RELIGIOUS:
		GameplayMapMesh->SetVisibility(true);
		GameplayMapMesh->SetMaterial(0, ReligiousMapTextureComponent->DynamicMaterial);
		break;
	case MapMode::CULTURAL:
		GameplayMapMesh->SetVisibility(true);
		GameplayMapMesh->SetMaterial(0, CultureMapTextureComponent->DynamicMaterial);
		break;
	case MapMode::TERRAIN:
		TerrainMapMesh->SetVisibility(true);
		GameplayMapMesh->SetVisibility(false);
		break;
	default:
		break;
	}
	CurrentMapMode = mode;
}

// to be able to use in BP
void AInteractiveMap::GetProvinceData(FName name, FProvinceData& out_data)
{
	FProvinceData* data = ProvinceDataMap.Find(name);
	if (data)
	{
		out_data = (*data);
	}
}

FProvinceData* AInteractiveMap::GetProvinceData(FName name)
{
	FProvinceData* data = ProvinceDataMap.Find(name);
	if (data)
	{
		return data;
	}

	return nullptr;
}

void AInteractiveMap::SetBorderVisibility(bool status)
{
	/*if (!IsValid(MapBorderMesh))
	{
		UE_LOG(LogTemp, Error, "Border Mesh not Created")
	}*/
	MapBorderMesh->SetVisibility(status);
}

void AInteractiveMap::UpdateProvinceData(const FProvinceData& data, FName id)
{
	FProvinceData* province = ProvinceDataMap.Find(id);
	if (province)
	{
		if (province->Owner != data.Owner)
		{
			FCountryData* newOwnerCountry = CountryData.Find(data.Owner);
			if (!newOwnerCountry)
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid owner - update not possible"));
				return;
			}

			//newOwnerCountry->Provinces.ad
			UpdateMapTexture(MapMode::POLITICAL, id, newOwnerCountry->Color);
			(*province) = data;
		}
		else if (province->Religion.DataName != data.Religion.DataName)
		{
			FColor* newColor = Religions.Find(data.Religion.DataName);
			if (!newColor)
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid religion - update not possible"));
				return;
			}

			//newOwnerCountry->Provinces.ad
			UpdateMapTexture(MapMode::RELIGIOUS, id, *newColor);
			(*province) = data;
			province->Religion.Color = *newColor;
		}
		else if (province->Culture.DataName != data.Culture.DataName)
		{
			FColor* newColor = Cultures.Find(data.Culture.DataName);
			if (!newColor)
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid culture - update not possible"));
				return;
			}

			//newOwnerCountry->Provinces.ad
			UpdateMapTexture(MapMode::CULTURAL, id, *newColor);
			(*province) = data;
			province->Culture.Color = *newColor;
		}
		else
		{
			(*province) = data;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid province id - update not possible"));
	}
	
}

FColor AInteractiveMap::GetColorFromLookUpTexture(FVector2D uv)
{
	return GetColorFromUV(MapLookUpTexture, uv);
}

void AInteractiveMap::UpdateProvinceHovered(const FColor& color)
{
	switch (CurrentMapMode)
	{
	case MapMode::POLITICAL:
		//color /= 255.0f;
		PoliticalMapTextureComponent->DynamicMaterial->SetVectorParameterValue("ProvinceHiglighted", color);
		break;
	case MapMode::RELIGIOUS:
		ReligiousMapTextureComponent->DynamicMaterial->SetVectorParameterValue("ProvinceHiglighted", color);
		break;
	case MapMode::CULTURAL:
		CultureMapTextureComponent->DynamicMaterial->SetVectorParameterValue("ProvinceHiglighted", color);
		break;
	case MapMode::TERRAIN:
		break;
	default:
		break;
	}
}

FColor AInteractiveMap::GetColorFromUV(UTexture2D* texture, FVector2D uv)
{
	if(!texture)
		return FColor();

	int32 width = texture->GetSizeX();
	int32 height = texture->GetSizeY();

	//int32 Index = (y * Width + x) * 4;
	int32 y = uv.Y * height;
	int32 x = uv.X * width;

	int32 index = (y * width + x) * 4;
	//int32 index = uv.X * width /** MapSelectMesh->GetComponentScale().X*/ * uv.Y * height/* * MapSelectMesh->GetComponentScale().X*/;
	//index = uv.X * width * uv.Y * height * 4;
	
	index = index % 4 + index;

	if (index < 0 || index > width * height * 4 - 4)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid index from uvs"));
		return FColor();
	}

	FColor color = FColor(MapColorCodeTextureData.PixelData[index],
		MapColorCodeTextureData.PixelData[index + 1],
		MapColorCodeTextureData.PixelData[index + 2],
		MapColorCodeTextureData.PixelData[index + 3]);

	return color;
}

UE_ENABLE_OPTIMIZATION
