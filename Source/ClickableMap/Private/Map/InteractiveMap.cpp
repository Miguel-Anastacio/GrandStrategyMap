// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/InteractiveMap.h"
#include "Map/MapUtils.h"
#include "Engine/Texture2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Map/DynamicTextureComponent.h"
// Sets default values
AInteractiveMap::AInteractiveMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MapSelectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map Selection"));
	MapSelectMesh->SetupAttachment(RootComponent);

	MapBorderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map Border"));
	MapBorderMesh->SetupAttachment(RootComponent);

	GameplayMapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gameplay Map"));
	GameplayMapMesh->SetupAttachment(RootComponent);


	PoliticalMapTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Dynamic Texture"));
	ReligiousMapTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Religious Map Texture"));
	CultureMapTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Culture Map Texture"));
	
}

UE_DISABLE_OPTIMIZATION
void AInteractiveMap::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//DynamicTextureComponent->InitializeTexture(MapLookUpTexture->GetSizeX(), MapLookUpTexture->GetSizeY());
	//UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(GameplayMapMaterial, this);
	////set paramater with Set***ParamaterValue
	//DynMaterial->SetTextureParameterValue("PoliticalMapTexture", PoliticalMapTexture);
	//GameplayMapMesh->SetMaterial(0, DynMaterial);

	//UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), TestPoliticalMapRenderTarget, DynMaterial);

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

	//if (PoliticalMapTexture)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Political map texture is valid"));
	//}

	GameplayMapMesh->SetMaterial(0, PoliticalMapTextureComponent->DynamicMaterial);



	////set paramater with Set***ParamaterValue
	//DynMaterial->SetTextureParameterValue("Texture", PoliticalMapTexture);

	//UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), TestPoliticalMapRenderTarget, DynMaterial);

	
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
	FTexture2DMipMap& Mip = MapLookUpTexture->PlatformData->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);

	// Get the dimensions of the texture
	int32 Width = MapLookUpTexture->GetSizeX();
	int32 Height = MapLookUpTexture->GetSizeY();

	// Assume texture format is RGBA
	const uint8* Data = static_cast<const uint8*>(TextureData);

	PixelColorPoliticalTexture.AddDefaulted(Width * Height * 4);
	PixelColorReligiousTexture.AddDefaulted(Width * Height * 4);
	PixelColorCultureMapTexture.AddDefaulted(Width * Height * 4);

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

			FColor pixelColor = FColor(0, 0, 0, 0);
			FName* id = LookUpTable.Find(FVector(R, G, B));
			if (id)
			{
				FProvinceData* province = GetProvinceData(*id);

				SetPixelColor(Index, PixelColorPoliticalTexture, GetCountryColor(province));
				SetPixelColor(Index, PixelColorReligiousTexture, GetReligionColor(province));
				SetPixelColor(Index, PixelColorCultureMapTexture, GetCultureColor(province));

			}
			else
			{
				SetPixelColor(Index, PixelColorPoliticalTexture, pixelColor);
				SetPixelColor(Index, PixelColorReligiousTexture, pixelColor);
				SetPixelColor(Index, PixelColorCultureMapTexture, pixelColor);
			}


			MapColorCodeTextureData.Add(FColor(R, G, B, A));
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
UE_ENABLE_OPTIMIZATION

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

// to be able to use in BP
void AInteractiveMap::GetProvinceData(FName name, FProvinceData& out_data)
{
	FProvinceData* data = ProvinceDataMap.Find(name);
	if (data)
	{
		out_data = (*data);
	}

	//return nullptr;
}

void AInteractiveMap::SetMapMode(MapMode mode)
{
	switch (mode)
	{
	case MapMode::POLITICAL:
		GameplayMapMesh->SetMaterial(0, PoliticalMapTextureComponent->DynamicMaterial);
		break;
	case MapMode::RELIGIOUS:
		GameplayMapMesh->SetMaterial(0, ReligiousMapTextureComponent->DynamicMaterial);
		break;
	case MapMode::CULTURAL:
		GameplayMapMesh->SetMaterial(0, CultureMapTextureComponent->DynamicMaterial);
		break;
	case MapMode::Terrain:
		break;
	default:
		break;
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

