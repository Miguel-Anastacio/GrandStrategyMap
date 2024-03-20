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


	DynamicTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Dynamic Texture"));
}

UE_DISABLE_OPTIMIZATION
void AInteractiveMap::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	DynamicTextureComponent->InitializeTexture(MapLookUpTexture->GetSizeX(), MapLookUpTexture->GetSizeY());
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

	CreateLookUpTable();
	SaveMapTextureData();

	//CreatePoliticalMapTexture();


	//if (PoliticalMapTexture)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Political map texture is valid"));
	//}
	CreatePoliticalMapTexture();

	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(GameplayMapMaterial, this);
	GameplayMapMesh->SetMaterial(0, DynMaterial);
	DynamicTextureComponent->DynamicMaterial = DynMaterial;
	DynamicTextureComponent->DynamicMaterial->SetTextureParameterValue("DynamicTexture", PoliticalMapTexture);


	////set paramater with Set***ParamaterValue
	//DynMaterial->SetTextureParameterValue("Texture", PoliticalMapTexture);

	//UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), TestPoliticalMapRenderTarget, DynMaterial);

	
}


void AInteractiveMap::SaveMapTextureData()
{
	if (!IsValid(MapLookUpTexture))
		return;


	//TextureCompressionSettings OldCompressionSettings = MapLookUpTexture->CompressionSettings;
	//TextureMipGenSettings OldMipGenSettings = MapLookUpTexture->MipGenSettings;
	//bool OldSRGB = MapLookUpTexture->SRGB;

	//MapLookUpTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	//MapLookUpTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	//MapLookUpTexture->SRGB = false;
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

	PixelColorPoliticalTexture.AddDefaulted(Width * Height * 4);
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



			FString* id = LookUpTable.Find(FVector(R, G, B));
			if (id)
			{
				FProvinceData* province = ProvinceDataMap.Find(*id);
				if (!province)
					break;
				FCountryData* country = CountryData.Find(province->Owner);
				if (!country)
					break;

				// r
				PixelColorPoliticalTexture[Index] = country->Color.R;
				// g
				PixelColorPoliticalTexture[Index + 1] = country->Color.G;
				// b
				PixelColorPoliticalTexture[Index + 2] = country->Color.B;
				// a
				PixelColorPoliticalTexture[Index + 3] = country->Color.A;
			}
			else
			{
				PixelColorPoliticalTexture[Index] = 0;
				// g
				PixelColorPoliticalTexture[Index + 1] = 0;
				// b
				PixelColorPoliticalTexture[Index + 2] = 0;
				// a
				PixelColorPoliticalTexture[Index + 3] = 0;
			}



			MapColorCodeTextureData.Add(FColor(R, G, B, A));
			/*if (FColor(R, G, B, 255) == FColor(95, 70, 144, 255))
			{
				int t = 0;
			}
			else if(FColor(R, G, B, 0) != FColor(0, 0, 0, 0))
			{
				int abd = 4;
			}*/
			// Do something with the color (R, G, B, A)
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
	CountryData.Emplace(TEXT("Spain"), FCountryData(FString("Spain"), FColor(255, 0, 0, 255)));
}
void AInteractiveMap::CreatePoliticalMapTexture()
{
	int32 width = MapLookUpTexture->GetSizeX();
	int32 height = MapLookUpTexture->GetSizeY();
	uint32 texDataSize = width * height * 4;

	uint8* politicalMapData = new uint8[texDataSize];
	//RawImageData is formatted as such :
	//[Pixel1 B] [Pixel1 G] [Pixel1 R] [Pixel1 A] [Pixel2 B] [Pixel2 G] [Pixel2 R] [Pixel2 A] …
	//for (int y = 0; y < height; y++)
	//{
	//	for (int x = 0; x < height; x++)
	//	{
	//		int32 ogIndex = (y * width+ x);
	//		FColor originalColor = MapColorCodeTextureData[ogIndex];

	//		int32 index = (y * width + x) * 4;

	//		FString* id = LookUpTable.Find(FVector(originalColor.R, originalColor.G, originalColor.B));
	//		if (id)
	//		{
	//			FProvinceData* province = ProvinceDataMap.Find(*id);
	//			if (!province)
	//				break;
	//			FCountryData* country = CountryData.Find(province->Owner);
	//			if (!country)
	//				break;

	//			// r
	//			politicalMapData[index] = country->Color.R;
	//			// g
	//			politicalMapData[index+1] = country->Color.G;
	//			// b
	//			politicalMapData[index+2] = country->Color.B;
	//			// a
	//			politicalMapData[index+3] = country->Color.A;
	//		}
	//		else
	//		{
	//			politicalMapData[index] = 0;
	//				// g
	//			politicalMapData[index + 1] = 0;
	//				// b
	//			politicalMapData[index + 2] = 0;
	//				// a
	//			politicalMapData[index + 3] = 0;
	//		}
	//	}
	//}

	DynamicTextureComponent->DrawFromDataBuffer(0, 0, MapLookUpTexture, PixelColorPoliticalTexture);
	DynamicTextureComponent->UpdateTexture();
	PoliticalMapTexture = DynamicTextureComponent->GetTexture();

	FMemory::Free(politicalMapData);
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
