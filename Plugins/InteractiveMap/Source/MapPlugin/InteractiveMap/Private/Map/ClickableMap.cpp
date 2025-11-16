// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/ClickableMap.h"
#include "VisualProperties.h"
#include "Engine/Texture2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Map/DynamicTextureComponent.h"
#include "Game/MapPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Map/MapDataComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "InteractiveMap.h"
#include "MapObject.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "Map/MapVisualComponent.h"
#include "ReplaceColorComputeShader/ReplaceColorComputeShader.h"

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
	MapTileChangedDelegate.AddDynamic(this, &AClickableMap::UpdateDynamicTextures);
}

void AClickableMap::SetMapTiles(const TArray<FTilePair>& NewData)
{
	TArray<int> IDs;
	IDs.Reserve(NewData.Num());
	for(const auto& [ID, Data] : NewData)
	{
		if(MapDataComponent->SetTileData(Data, ID))
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
			if(const FInstancedStruct* CurrentData = MapDataComponent->GetTileData(ID))
			{
				const FColor Color = MapAsset->GetPropertyColorFromInstancedStruct(*CurrentData, PropertyName, bResult);
				if(!bResult)
					continue;
				
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
	LoadMapAsset(MapAsset);
	if (!GetLookupTexture())
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Map Look up Texture not assigned"));
		return;
	}
	DynamicTextureComponent->InitializeTexture(GetLookupTexture()->GetSizeX(), GetLookupTexture()->GetSizeY());
	CreateMapModes();
	
	// set Current texture in Component
	if(UDynamicTexture** DynamicTextureTest = MapModeDynamicTextures.Find(StartMapMode))
	{
		DynamicTextureComponent->SetDynamicTexture(*DynamicTextureTest);
		DynamicTextureComponent->UpdateTexture();
		DynamicTextureComponent->GetMaterialInstance()->SetTextureParameterValue("DynamicTexture", DynamicTextureComponent->GetTexture());
	}

	SetMapMode_Implementation(StartMapMode);
	
	// set reference in player class
	if (AMapPawn* Player = Cast<AMapPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
	{
		Player->SetInteractiveMap(this);
	}
}

UMapObject* AClickableMap::GetMapObject() const
{
	return MapAsset;
}

// Called when the game starts or when spawned
void AClickableMap::BeginPlay()
{
	Super::BeginPlay();
	InitializeMap_Implementation();
}

void AClickableMap::CreateMapModes()
{
	CreateDynamicTextures(MapAsset->GetVisualProperties());
	FillDynamicTextures(GetLookupTextureData());
}

void AClickableMap::UpdateTileData(const FInstancedStruct& Data, int ID) const
{
	if(MapDataComponent->SetTileData(Data, ID))
	{
		MapTileChangedDelegate.Broadcast({ID});
	}
}

void AClickableMap::CreateDynamicTextures(const TArray<TObjectPtr<UVisualProperty>>& VisualPropertyTypes)
{
	if(!GetLookupTexture())
		return;
	const int32 Width = GetLookupTexture()->GetSizeX();
	const int32 Height = GetLookupTexture()->GetSizeY();
	
	for(const auto& VisualPropertyType : VisualPropertyTypes)
	{
		UDynamicTexture* DynamicTexture = NewObject<UDynamicTexture>();
		DynamicTexture->InitializeDynamicTexture(Width, Height);
		DynamicTexture->InitMaterial(UMaterialInstanceDynamic::Create(VisualPropertyType->GetMaterial(), this),
										GetLookupTexture(), 1.0f);
		MapModeDynamicTextures.Emplace(VisualPropertyType->Name, DynamicTexture);
	}
}

void AClickableMap::FillDynamicTextures(const TArray<uint8>& LookupTextureData)
{
	FillPixelMap();
	const int32 Width = GetLookupTexture()->GetSizeX();
	const int32 Height = GetLookupTexture()->GetSizeY();
	if(LookupTextureData.Num() != Width*Height*4)
		return;

	if(PixelMap.Num() != MapAsset->GetLookupTable().Num())
	{
			UE_LOG(LogInteractiveMap, Error, TEXT("Pixel Map does not match lookup"))
			UE_LOG(LogInteractiveMap, Error, TEXT("Pixel Map size %d"), PixelMap.Num());
			UE_LOG(LogInteractiveMap, Error, TEXT("Lookup size %d"), MapAsset->GetLookupTable().Num());
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

		const int32 Width = GetLookupTexture()->GetSizeX();
		for(const auto& Pos : Positions->PosArray)
		{
			const int32 Index = (Pos.Y * Width + Pos.X) * 4;
			PixelBuffer[Index + 3] = MarkerValue;
		}
	}
}

UTexture2D* AClickableMap::GetLookupTexture() const
{
	return MapAsset->GetLookupTexture().Get();
}

TArray<uint8> AClickableMap::GetLookupTextureData() const
{
	return MapAsset->GetLookupTextureData();
}

TMap<FColor, int> AClickableMap::GetLookUpTable() const
{
	return MapDataComponent->GetLookUpTable();
}

TMap<int, FInstancedStruct>* AClickableMap::GetProvinceDataMap() const
{
	return MapDataComponent->GetProvinceDataMap();
}

int AClickableMap::GetProvinceID(const FColor& Color, bool& bOutResult) const
{
	return MapDataComponent->GetTileID(Color, bOutResult);
}

void AClickableMap::SetMapMode_Implementation(const FName& Mode)
{
#if WITH_EDITOR
	if(Mode == "Debug")
	{
		DynamicTextureComponent->GetMaterialInstance()->SetTextureParameterValue("DynamicTexture", GetLookupTexture());
		if(UStaticMeshComponent* StaticMeshComponent = MapVisualComponent->GetMapGameplayMeshComponent())
		{
			StaticMeshComponent->SetMaterial(0, DynamicTextureComponent->GetMaterialInstance());
			MapModeChangedDelegate.Broadcast(CurrentMapMode, Mode);
			CurrentMapMode = Mode;
		}
		return;
	}
#endif
	
	if(UDynamicTexture** CurrentDyntTexture = MapModeDynamicTextures.Find(Mode))
	{
		DynamicTextureComponent->SetDynamicTexture(*CurrentDyntTexture);
		DynamicTextureComponent->UpdateTexture();
		DynamicTextureComponent->GetMaterialInstance()->SetTextureParameterValue("DynamicTexture", DynamicTextureComponent->GetTexture());
		if(UStaticMeshComponent* StaticMeshComponent = MapVisualComponent->GetMapGameplayMeshComponent())
		{
			StaticMeshComponent->SetMaterial(0, DynamicTextureComponent->GetMaterialInstance());
			MapModeChangedDelegate.Broadcast(CurrentMapMode, Mode);
			CurrentMapMode = Mode;
		}
	}
}

// to be able to use in BP
void AClickableMap::GetProvinceData(const int ID, FInstancedStruct& OutData) const 
{
	MapDataComponent->GetTileData(ID, OutData);
}

FInstancedStruct* AClickableMap::GetProvinceData(const int ID) const
{
	return MapDataComponent->GetTileData(ID);
}

void AClickableMap::FillPixelMap()
{
	PixelMap.Empty();
	const int32 Width = GetLookupTexture()->GetSizeX();
	const int32 Height = GetLookupTexture()->GetSizeY();
	auto LookupData = GetLookupTextureData();
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

FColor AClickableMap::GetColorFromLookUpTexture(const FVector2D& Uv) const
{
	return UAtkTextureUtilsFunctionLibrary::GetColorFromUV(GetLookupTexture(), Uv, GetLookupTextureData());
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
	if(!MapObject)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Map Object is NULL, please provide a Map Object"));
		return;
	}
	// Load Data
	MapDataComponent->SetMapObject(MapObject);
	
}
