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
	DynamicTextureComponent->InitializeTexture(MapLookUpTexture->GetSizeX(), MapLookUpTexture->GetSizeY());

	CreateLookUpTable();
	ReadProvinceDataTable();
	ReadCountryDataTable();
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


			FName* id = LookUpTable.Find(FVector(R, G, B));
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
	DynamicTextureComponent->DrawFromDataBuffer(0, 0, MapLookUpTexture, PixelColorPoliticalTexture);
	DynamicTextureComponent->UpdateTexture();
	PoliticalMapTexture = DynamicTextureComponent->GetTexture();
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

FProvinceData* AInteractiveMap::GetProvinceData(FName name)
{
	FProvinceData* data = ProvinceDataMap.Find(name);
	if (data)
	{
		return data;
	}

	return nullptr;
}
