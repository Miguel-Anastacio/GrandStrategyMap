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
#include "ShadersModule/Public/ReplaceColorComputeShader/ReplaceColorComputeShader.h"

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
#if WITH_EDITOR
void AClickableMap::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(MapDataComponent && MapAsset)
	{
		MapDataComponent->ReadDataTables(MapAsset->VisualPropertiesDT, MapAsset->VisualPropertyTypesDT);
	}
}
#endif

void AClickableMap::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MapTileChangedDelegate.AddDynamic(this, &AClickableMap::UpdateDynamicTextures);
}

void AClickableMap::SetMapTiles(const TArray<FTilePair>& NewData)
{
	TArray<int> IDs;
	IDs.Reserve(NewData.Num());
	for(const auto& [ID, Data] : NewData)
	{
		if(MapDataComponent->SetProvinceData(Data, ID))
		{
			IDs.Emplace(ID);
		}
	}
	MapTileChangedDelegate.Broadcast(IDs);
}

void AClickableMap::UpdateDynamicTextures(const TArray<int>& IDs)
{
	if(IDs.IsEmpty())
		return;
	
	for(const auto& [PropertyName, DynamicTexture] : MapModeDynamicTextures)
	{
		bool bResult = false;
		TMap<FColor, TArray<int>> BatchesOfProvinces;
		for(const auto& ID : IDs)
		{
			if(const FInstancedStruct* CurrentData = MapDataComponent->GetProvinceData(ID))
			{
				const FColor Color = MapDataComponent->GetPropertyColorFromInstancedStruct(*CurrentData, PropertyName, bResult);
				if(!bResult)
					break;
				
				if(TArray<int>* IdsToEdit = BatchesOfProvinces.Find(Color))
				{
					IdsToEdit->Emplace(ID);
				}
				else
				{
					BatchesOfProvinces.Emplace(FColor(Color), {ID});
				}
			}	
		}
		if(BatchesOfProvinces.IsEmpty())
			continue;

		// Mark all Pixels that are to be edited by the compute shader
		TArray<uint8> DataBuffer = DynamicTexture->GetTextureDataCopy();
		TArray<FColorReplace> ColorReplaces;
		ColorReplaces.Reserve(BatchesOfProvinces.Num());
		uint8 Marker = 0;
		UE_LOG(LogInteractiveMap, Warning, TEXT("Property: %s"), *PropertyName.ToString())
		for (const auto& [Key, BatchIDs] : BatchesOfProvinces)
		{
			MarkPixelsToEdit(DataBuffer, BatchIDs, Marker);
			ColorReplaces.Emplace(FColorReplace{FColor::White, Key});
			Marker++;
		}

		// dispatch all changes to this dynamic texture
		const FReplaceColorComputeShaderDispatchParams Params(DataBuffer, ColorReplaces);
		FReplaceColorComputeShaderInterface::Dispatch(Params, [this, PropertyName](TArray<uint32> OutputVal)
		{
			if(	UDynamicTexture** TextureDynData = MapModeDynamicTextures.Find(PropertyName))
			{
				(*TextureDynData)->SetTextureData(OutputVal);
			}
			this->DynamicTextureComponent->UpdateTexture();
		});
	}
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
	DynamicTextureComponent->InitializeTexture(MapLookUpTexture->GetSizeX(), MapLookUpTexture->GetSizeY());
	CreateMapModes();
	
	// set Current texture in Component
	UDynamicTexture* DynamicTextureTest = *MapModeDynamicTextures.Find("Landscape");
	DynamicTextureComponent->SetDynamicTexture(DynamicTextureTest);
	DynamicTextureComponent->UpdateTexture();
	DynamicTextureComponent->GetMaterialInstance()->SetTextureParameterValue("DynamicTexture", DynamicTextureComponent->GetTexture());

	SetMapMode_Implementation(FName("Landscape"));
	
	// set referene in player class
	AMapPawn* player = Cast<AMapPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player)
	{
		player->SetInteractiveMap(this);
	}
}
// Called when the game starts or when spawned
void AClickableMap::BeginPlay()
{
	Super::BeginPlay();
	InitializeMap_Implementation();
}

void AClickableMap::CreateMapModes()
{
	TArray<FVisualPropertyType> Types;
	MapDataComponent->GetVisualPropertiesMap().GetKeys(Types);
	CreateDynamicTextures(Types);
	FillDynamicTextures(MapDataComponent->GetVisualPropertyNameMap(), MapAsset->GetLookupTextureData());
}

void AClickableMap::UpdateTileData(const FInstancedStruct& Data, int ID)
{
	MapDataComponent->SetProvinceData(Data, ID);
	MapTileChangedDelegate.Broadcast({ID});
}

void AClickableMap::CreateDynamicTextures( const TArray<FVisualPropertyType>& VisualPropertyTypes)
{
	MapLookUpTexture = MapAsset->LookupTexture;
	if(!MapLookUpTexture)
		return;
	
	const int32 Width = MapLookUpTexture->GetSizeX();
	const int32 Height = MapLookUpTexture->GetSizeY();
	for(const auto& VisualPropertyType : VisualPropertyTypes)
	{
		UDynamicTexture* DynamicTexture = NewObject<UDynamicTexture>();
		DynamicTexture->InitializeDynamicTexture(Width, Height);
		DynamicTexture->InitMaterial(UMaterialInstanceDynamic::Create(VisualPropertyType.MaterialInstance, this),
										MapLookUpTexture, 1.0f);
		MapModeDynamicTextures.Emplace(VisualPropertyType.Type, DynamicTexture);
	}
}

void AClickableMap::FillDynamicTextures(const TMap<FName, FArrayOfVisualProperties>& VisualProperties, const TArray<uint8>& LookupTextureData)
{
	FillPixelMap();
	const int32 Width = MapLookUpTexture->GetSizeX();
	const int32 Height = MapLookUpTexture->GetSizeY();
	if(LookupTextureData.Num() != Width*Height*4)
		return;

	if(PixelMap.Num() != MapAsset->GetLookupTable().Num())
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Pixel Map does not match lookup"))
	}
	
	TArray<int> IDs;
	IDs.Reserve(MapDataComponent->GetLookUpTable().Num());
	MapAsset->GetLookupTable().GenerateValueArray(IDs);
	UpdateDynamicTextures(IDs);
}

void AClickableMap::MarkPixelsToEdit(TArray<uint8>& PixelBuffer, const TArray<int>& IDs, uint8 MarkerValue) const
{
	for(const auto& ID : IDs)
	{
		FColor Color = MapDataComponent->GetColor(ID);
		
		const FPositions* Positions = PixelMap.Find(Color);
		if(!Positions)
		{
			UE_LOG(LogInteractiveMap, Error, TEXT("Color not in PixelMap %s"), *Color.ToString());
			continue;
		}

		const int32 Width = MapLookUpTexture->GetSizeX();
		for(const auto& Pos : Positions->PosArray)
		{
			const int32 Index = (Pos.Y * Width + Pos.X) * 4;
			PixelBuffer[Index + 3] = MarkerValue;
		}
	}
}

// Called every frame
void AClickableMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	return  nullptr;
}

TMap<FName, FColoredData>* AClickableMap::GetVisualPropertiesDataMap()
{
	return nullptr;
}

int AClickableMap::GetProvinceID(const FColor& Color, bool& bOutResult) const
{
	return MapDataComponent->GetProvinceID(Color, bOutResult);
}


void AClickableMap::SetMapMode_Implementation(const FName& Mode)
{
	if(UDynamicTexture** CurrentDyntTexture = MapModeDynamicTextures.Find(Mode))
	{
		DynamicTextureComponent->SetDynamicTexture(*CurrentDyntTexture);
		DynamicTextureComponent->UpdateTexture();
		DynamicTextureComponent->GetMaterialInstance()->SetTextureParameterValue("DynamicTexture", DynamicTextureComponent->GetTexture());
		if(UStaticMeshComponent* StaticMeshComponent = MapVisualComponent->GetMapGameplayMeshComponent())
		{
			StaticMeshComponent->SetMaterial(0, DynamicTextureComponent->GetMaterialInstance());
		}
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

void AClickableMap::FillPixelMap()
{
	PixelMap.Empty();
	const int32 Width = MapAsset->LookupTexture->GetSizeX();
	const int32 Height = MapAsset->LookupTexture->GetSizeY();
	auto LookupData = MapAsset->GetLookupTextureData();
	if(LookupData.Num() != Width*Height*4)
		return;
	
	// Read color of each pixel
	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			const int32 Index = (Y * Width + X) * 4; // 4 bytes per pixel (RGBA)
			const uint8 B = LookupData[Index];
			const uint8 G = LookupData[Index + 1];
			const uint8 R = LookupData[Index + 2];
			const uint8 A = LookupData[Index + 3];
			FColor Color(R, G, B, A);
			if(FPositions* Positions = PixelMap.Find(Color))
			{
				Positions->PosArray.Emplace(FVector2D(X, Y));
			}
			else
			{
				FPositions NewPosition;
				NewPosition.PosArray.Emplace(FVector2D(X, Y));
				PixelMap.Emplace(Color, NewPosition);
			}
		}
	}
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

void AClickableMap::UpdateProvinceHovered(const FColor& Color)
{
	// possible improve, just add dynMaterial ref to DynamicTextureComponent
	//  so then just update the current 
	for(const auto& DynTextures : MapModeDynamicTextures)
	{
		DynTextures.Value->DynamicMaterial->SetVectorParameterValue("ProvinceHighlighted", Color);
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
