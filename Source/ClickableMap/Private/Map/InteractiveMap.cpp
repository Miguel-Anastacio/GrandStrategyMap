// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/InteractiveMap.h"
#include "Map/MapUtils.h"
#include "Engine/Texture2D.h"
#include "Kismet/KismetRenderingLibrary.h"
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
}

UE_DISABLE_OPTIMIZATION
// Called when the game starts or when spawned
void AInteractiveMap::BeginPlay()
{
	Super::BeginPlay();

	CreateLookUpTable();
	SaveMapTextureData();
	CreatePoliticalMapTexture();

	//create dynamic material anywhere u like, Constructor or anywhere .
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(GameplayMapMaterial, this);
	//set paramater with Set***ParamaterValue
	DynMaterial->SetTextureParameterValue("PoliticalMapTexture", PoliticalMapTexture);
	GameplayMapMesh->SetMaterial(0, DynMaterial);

	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), TestPoliticalMapRenderTarget, DynMaterial);
	//UKismetRenderingLibrary::rendertarger

	
}

void AInteractiveMap::SaveMapTextureData()
{
	if (!IsValid(MapLookUpTexture))
		return;


	TextureCompressionSettings OldCompressionSettings = MapLookUpTexture->CompressionSettings;
	TextureMipGenSettings OldMipGenSettings = MapLookUpTexture->MipGenSettings;
	bool OldSRGB = MapLookUpTexture->SRGB;

	MapLookUpTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	MapLookUpTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	MapLookUpTexture->SRGB = false;
	////MapTexture->UpdateResource();
	//FTexture2DMipMap* MipMap = &MapTexture->PlatformData->Mips[0];
	//FByteBulkData* ImageData = &MipMap->BulkData;
	//uint8* RawImageData = (uint8*)ImageData->Lock(LOCK_READ_ONLY);
	////const uint8* FormatedImageData =  (uint8*)(MapTexture->GetPlatformData()->Mips[0].BulkData.LockReadOnly());
	//auto xSize = MapTexture->GetSizeX();
	//auto ySize = MapTexture->GetSizeY();

	//MapColorCodeTextureData.AddDefaulted(MapTexture->GetSizeX() * MapTexture->GetSizeY());
	//int i = 0;
	//for (int32 Y = 0; Y < MapTexture->GetSizeY(); Y++)
	//{
	//	for (int32 X = 0; X < MapTexture->GetSizeX(); X+=4)
	//	{
	//		int index = Y * MapTexture->GetSizeX() + X; 
	//		uint8 b = RawImageData[index];
	//		uint8 r = RawImageData[index + 1];
	//		uint8 g = RawImageData[index + 2];
	//		uint8 a = RawImageData[index + 3];

	//		// Do the job with the pixel
	//		i++;
	//		if (i == xSize / 2 * ySize / 2)
	//		{
	//			int test = 2;
	//		}
	//	}
	//}
	////FMemory::Memcpy(MapColorCodeTextureData.GetData(), &FormatedImageData, sizeof(FColor) * MapColorCodeTextureData.Num());



	//MapTexture->GetPlatformData()->Mips[0].BulkData.Unlock();


	// Lock the texture for reading
	FTexture2DMipMap& Mip = MapLookUpTexture->PlatformData->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);

	// Get the dimensions of the texture
	int32 Width = MapLookUpTexture->GetSizeX();
	int32 Height = MapLookUpTexture->GetSizeY();

	// Assume texture format is RGBA
	const uint8* Data = static_cast<const uint8*>(TextureData);

	// Read color of each pixel
	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			int32 Index = (Y * Width + X) * 4; // 4 bytes per pixel (RGBA)
			uint8 R = Data[Index];
			uint8 G = Data[Index + 1];
			uint8 B = Data[Index + 2];
			uint8 A = Data[Index + 3];

			MapColorCodeTextureData.Add(FColor(R, G, B, A));
			if (MapColorCodeTextureData.Last() != FColor(0, 0, 0, 0))
			{
				int t = 0;
			}
			// Do something with the color (R, G, B, A)
		}
	}

	// Unlock the texture
	Mip.BulkData.Unlock();
	MapLookUpTexture->CompressionSettings = OldCompressionSettings;
	MapLookUpTexture->MipGenSettings = OldMipGenSettings;
	MapLookUpTexture->SRGB = OldSRGB;
	MapLookUpTexture->UpdateResource();
}

// Called every frame
void AInteractiveMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AInteractiveMap::CreateLookUpTable()
{
	TArray<FName> RowNames = MapDataTable->GetRowNames();

	for (auto& name : RowNames)
	{
		FProvinceIDData* Item = MapDataTable->FindRow<FProvinceIDData>(name, "");
		if (Item)
		{
			FString temp = Item->Color;
			temp += FString("FF");
			FColor sRGBColor = Item->ConvertHexStringToRGB(temp);

			LookUpTable.Add(FVector(sRGBColor.R, sRGBColor.G, sRGBColor.B), Item->Name);

			// for now init provinces with test values
			int64 value = FMath::RandRange(1000, 5000);
			ProvinceDataMap.Emplace(Item->Name, FProvinceData(Item->Name, FString("Spain"), value));
			
		}
	}

	// Test Country
	CountryData.Emplace(TEXT("Spain"), FCountryData(FString("Spain"), FColor(1.0f, 0, 0, 1.0f)));
}
void AInteractiveMap::CreatePoliticalMapTexture()
{
	// Create custom texture and retrieve raw image data
	PoliticalMapTexture = UTexture2D::CreateTransient(MapLookUpTexture->GetSizeX(), MapLookUpTexture->GetSizeY());
	FTexture2DMipMap* MipMap = &PoliticalMapTexture->PlatformData->Mips[0];
	FByteBulkData* ImageData = &MipMap->BulkData;
	uint8* RawImageData = (uint8*)ImageData->Lock(LOCK_READ_WRITE);

	//RawImageData is formatted as such :
	//[Pixel1 B] [Pixel1 G] [Pixel1 R] [Pixel1 A] [Pixel2 B] [Pixel2 G] [Pixel2 R] [Pixel2 A] …
	for (int y = 0; y < PoliticalMapTexture->GetSizeY(); y++)
	{
		for (int x = 0; x < PoliticalMapTexture->GetSizeX(); x++)
		{
			int32 ogIndex = (y * PoliticalMapTexture->GetSizeX() + x);
			FColor originalColor = MapColorCodeTextureData[ogIndex];

			int32 index = (y * PoliticalMapTexture->GetSizeX() + x) * 4;
			
			FString* id = LookUpTable.Find(FVector(originalColor));
			if (id)
			{
				FProvinceData* province = ProvinceDataMap.Find(*id);
				if (!province)
					break;
				FCountryData* country = CountryData.Find(province->Owner);
				if (!country)
					break;

				// r
				RawImageData[index] = country->Color.R;
				// g
				RawImageData[index+1] = country->Color.G;
				// b
				RawImageData[index+2] = country->Color.B;
				// a
				RawImageData[index+3] = country->Color.A;
			}
			else
			{
				RawImageData[index] = 0;
					// g
				RawImageData[index + 1] = 0;
					// b
				RawImageData[index + 2] = 0;
					// a
				RawImageData[index + 3] = 0;
			}
		}
	}

	int a = 3;
}
UE_ENABLE_OPTIMIZATION

UTextureRenderTarget2D* AInteractiveMap::GetMapRenderTarget() const
{
	return MapRenderTarget;
}

TMap<FVector,FString> AInteractiveMap::GetLookUpTable() const
{
	return LookUpTable;
}

FString AInteractiveMap::GetProvinceID(const FVector& color) const
{
	const FString* ID = LookUpTable.Find(color);
	if (ID)
	{
		return (*ID);
	}

	return FString();
}
