//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Flat/FlatInteractiveCombinedMap.h"
#include "Map/Visual/CombinedMapVisualComponent.h"
#include "Map/DynamicTextureComponent.h"
#include "Map/Flat/MapLimitComponent.h"
AFlatInteractiveCombinedMap::AFlatInteractiveCombinedMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCombinedMapVisualComponent>(TEXT("Map Visual")))
{

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
