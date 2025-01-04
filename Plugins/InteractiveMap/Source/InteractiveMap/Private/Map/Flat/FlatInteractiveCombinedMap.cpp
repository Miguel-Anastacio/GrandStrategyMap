//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Flat/FlatInteractiveCombinedMap.h"
#include "Map/Visual/CombinedMapVisualComponent.h"
#include "Map/DynamicTextureComponent.h"
#include "Map/Flat/MapLimitComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "InteractiveMap.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"

AFlatInteractiveCombinedMap::AFlatInteractiveCombinedMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCombinedMapVisualComponent>(TEXT("Map Visual")))
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Script/Engine.MaterialInstanceConstant'/InteractiveMap/GSMap/InteractiveMap/Material/Instances/MI_PoliticalMapWithTerrain.MI_PoliticalMapWithTerrain'"));
	if (MaterialFinder.Succeeded())
		GameplayMapMaterial = MaterialFinder.Object;
}

void AFlatInteractiveCombinedMap::SetMapMode_Implementation(MapMode mode)
{
	Super::SetMapMode_Implementation(mode);

	UStaticMeshComponent* mesh = MapVisualComponent->GetMapGameplayMeshComponent();
	switch (mode)
	{
	case MapMode::POLITICAL:
	
		break;
	case MapMode::RELIGIOUS:
		
		break;
	case MapMode::CULTURAL:
	
		break;
	case MapMode::TERRAIN:
		if (mesh)
		{
			mesh->SetMaterial(0, TerrainDynamicMaterial);
		}
		break;
	default:
		break;
	}

	UpdateLimits();
	
}

void AFlatInteractiveCombinedMap::InitializeMap_Implementation()
{
	Super::InitializeMap_Implementation();

	if (!bUseBorderMesh)
	{
	}

	if (!BorderMaterial)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Use border set to true but border material is null"));
	}
	else
	{
		BorderDynamicMaterial = UMaterialInstanceDynamic::Create(BorderMaterial, this);
		//BorderDynamicMaterial->SetTextureParameterValue("LookUpTexture", MapLookUpTexture);
		SetBorderLookUpTexture(BorderDynamicMaterial, PoliticalMapTextureComponent);

		BorderMaterialRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 2048, 1024, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor::White);
		UpdateBorder(BorderDynamicMaterial, BorderMaterialRenderTarget);

	}

	MapInitializationDelegate.Broadcast(this);
}

//void AFlatInteractiveCombinedMap::UpdateProvinceHovered(const FColor& color)
//{
//	Super::UpdateProvinceHovered(color);
//	if (CurrentMapMode == MapMode::TERRAIN)
//	{
//		TerrainDynamicMaterial->SetVectorParameterValue("ProvinceHighlighted", color);
//	}
//}
//
//void AFlatInteractiveCombinedMap::InitializeMap()
//{
//	Super::InitializeMap();
//	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(GameplayMapMaterial, this);
//	TerrainDynamicMaterial = DynMaterial;
//	TerrainDynamicMaterial->SetTextureParameterValue("DynamicTexture", TerrainTexture);
//	TerrainDynamicMaterial->SetTextureParameterValue("LookUpTexture", MapLookUpTexture);
//}
