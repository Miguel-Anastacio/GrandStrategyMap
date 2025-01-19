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

void AClickableMap::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(MapDataComponent && MapAsset)
	{
		MapDataComponent->ReadDataTables(MapAsset->VisualPropertiesDT, MapAsset->VisualPropertyTypesDT);
	}
}

void AClickableMap::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MapTileChangeDelegate.AddDynamic(this, &AClickableMap::OnMapTileChanged);
}

void AClickableMap::OnMapTileChanged(int ID, const FInstancedStruct& Data)
{
	if(FInstancedStruct* CurrentData = MapDataComponent->GetProvinceData(ID))
	{
		if(CurrentData->GetScriptStruct() == Data.GetScriptStruct())
		{
			*CurrentData = Data;
		}
	}

	UDynamicTexture* DynamicTexture = DynamicTextureComponent->GetDynamicTexture();
	bool bResult = false;
	FColor OldColor = MapDataComponent->GetVisualProperty(FName("Country"), FName("POR"), bResult).Color;
	FIntVector3 Vec(OldColor.R, OldColor.G, OldColor.B);
	TArray<FColorReplace> Colors;
	Colors.Emplace(FColorReplace{Vec, FIntVector3{0, 0, 255}});
	FReplaceColorComputeShaderDispatchParams Params(*DynamicTexture->GetTextureData(), Colors);
 
	// Executes the compute shader and calls the TFunction when complete.
	FReplaceColorComputeShaderInterface::Dispatch(Params, [this](TArray<uint32> OutputVal)
	{
		UDynamicTexture* CurrentTextureData = DynamicTextureComponent->GetDynamicTexture();
		CurrentTextureData->SetTextureData(OutputVal);
		// CurrentTextureData->SetTextureData(UTextureUtilsFunctionLibrary::PackUint8ToUint32(*CurrentTextureData->GetTextureData()));
		this->DynamicTextureComponent->UpdateTexture();
		
		DynamicTextureComponent->GetMaterialInstance()->SetTextureParameterValue("DynamicTexture", DynamicTextureComponent->GetTexture());
	});
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
	// Create Dynamic Textures
	// Fill them
	CreateMapModes();
	// set Current texture in Component
	UDynamicTexture* DynamicTextureTest = *MapModesTextureComponents.Find("Landscape");
	DynamicTextureComponent->SetDynamicTexture(DynamicTextureTest);
	DynamicTextureComponent->UpdateTexture();
	DynamicTextureComponent->GetMaterialInstance()->SetTextureParameterValue("DynamicTexture", DynamicTextureComponent->GetTexture());

	SetMapMode_Implementation(FName("Landscape"));
	
	MapDataChangedDelegate.AddDynamic(this, &AClickableMap::UpdateMapTexture);

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
		UDynamicTexture* DynamicTexture = NewObject<UDynamicTexture>();
		DynamicTexture->InitializeDynamicTexture(Width, Height);
		DynamicTexture->InitMaterial(UMaterialInstanceDynamic::Create(VisualPropertyType.MaterialInstance, this),
										MapLookUpTexture, 1.0f);
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
			if (!ID)
			{
				// UE_LOG(LogInteractiveMap, Error, TEXT("Error color not in lookup table %s"), *FColor(R, G, B).ToString());
				continue;
			}
			FInstancedStruct* InstancedStruct = GetProvinceData(*ID);
			if (!InstancedStruct)
			{
				UE_LOG(LogInteractiveMap, Error, TEXT("Error province present in look up table but not in province map data"));
				return;
			}

			for(const auto& Visual  : VisualProperties)
			{
				const FProperty* Property = InstancedStruct->GetScriptStruct()->FindPropertyByName(Visual.Key);
				if (!Property)
				{
					continue;
				}
				
				const FName PropertyTypeName = Property->GetFName();
				bool bResult = false;
				const FString Tag = UADStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(*InstancedStruct, PropertyTypeName.ToString(), bResult);
				if(!bResult)
				{
					UE_LOG(LogInteractiveMap, Error, TEXT("Property %s is visual but type is not convertible to string"), *PropertyTypeName.ToString())
					continue;
				}

				for(auto& VisualProperty : Visual.Value.VisualProperties)
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
	if(UDynamicTexture** CurrentDyntTexture = MapModesTextureComponents.Find(Mode))
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
	// FCountryData* country = MapDataComponent->CountryDataMap.Find(id);
	
	// if (!country)
	return false;

	// UpdatePixelArray(*PoliticalMapTextureComponent->GetTextureData(), country->Color, color.ToFColor(true), PoliticalMapTextureComponent->GetTexture(), country->Provinces);
	// country->Color = color.ToFColor(true);

	// PoliticalMapTextureComponent->UpdateTexture();

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
	// possible improve, just add dynMaterial ref to DynamicTextureComponent
	//  so then just update the current 
	for(const auto& DynTextures : MapModesTextureComponents)
	{
		DynTextures.Value->DynamicMaterial->SetVectorParameterValue("ProvinceHighlighted", color);
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
