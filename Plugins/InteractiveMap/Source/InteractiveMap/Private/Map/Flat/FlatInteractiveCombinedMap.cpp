//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Flat/FlatInteractiveCombinedMap.h"
#include "Map/Visual/CombinedMapVisualComponent.h"
#include "Map/DynamicTextureComponent.h"
#include "Map/Flat/MapLimitComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "InteractiveMap.h"
AFlatInteractiveCombinedMap::AFlatInteractiveCombinedMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCombinedMapVisualComponent>(TEXT("Map Visual")))
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Script/Engine.MaterialInstanceConstant'/InteractiveMap/GSMap/InteractiveMap/Material/Instances/MI_PoliticalMapWithTerrain.MI_PoliticalMapWithTerrain'"));
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

void AFlatInteractiveCombinedMap::InitializeMap()
{
	Super::InitializeMap();
	if (!bUseBorderMesh)
	{
		return;
	}

	if (!BorderMaterial)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Use border set to true but border material is null"));
		return;
	}
	if (!BorderMaterialRenderTarget)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Use border set to true but border render target is null"));
		return;
	}

	UMaterialInstanceDynamic* material = UMaterialInstanceDynamic::Create(BorderMaterial, this);
	material->SetTextureParameterValue("LookUpTexture", MapLookUpTexture);
	
	UTextureRenderTarget2D* renderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 1024, 1024);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), BorderMaterialRenderTarget, material);

	//UTextureRenderTarget2D

	PoliticalMapTextureComponent->DynamicMaterial->SetTextureParameterValue("BorderTexture", BorderMaterialRenderTarget);
	ReligiousMapTextureComponent->DynamicMaterial->SetTextureParameterValue("BorderTexture", BorderMaterialRenderTarget);
	CultureMapTextureComponent->DynamicMaterial->SetTextureParameterValue("BorderTexture", BorderMaterialRenderTarget);
	TerrainDynamicMaterial->SetTextureParameterValue("BorderTexture", BorderMaterialRenderTarget);
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
