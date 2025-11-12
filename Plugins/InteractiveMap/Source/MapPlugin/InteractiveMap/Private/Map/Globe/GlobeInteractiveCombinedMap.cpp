//  Copyright 2024 An@stacioDev All rights reserved.
#include "Map/Globe/GlobeInteractiveCombinedMap.h"
#include "Map/Visual/CombinedMapVisualComponent.h"
#include "Map/DynamicTextureComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "InteractiveMap.h"
#include "MapObject.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"

AGlobeInteractiveCombinedMap::AGlobeInteractiveCombinedMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCombinedMapVisualComponent>(TEXT("Map Visual")))
{
}

void AGlobeInteractiveCombinedMap::InitializeMap_Implementation()
{
	Super::InitializeMap_Implementation();

	if (!bUseBorderMesh)
	{
		MapInitializationDelegate.Broadcast(this);
		return;
	}

	if (!BorderMaterial)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Use border set to true but border material is null"));
	}
	else
	{
		BorderDynamicMaterial = UMaterialInstanceDynamic::Create(BorderMaterial, this);
		BorderDynamicMaterial->SetTextureParameterValue("LookUpTexture", MapAsset->GetLookupTexture().Get());

		BorderMaterialRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 2048, 1024, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor::White);
		UpdateBorder(BorderDynamicMaterial, BorderMaterialRenderTarget);
	}
	
	MapInitializationDelegate.Broadcast(this);
}