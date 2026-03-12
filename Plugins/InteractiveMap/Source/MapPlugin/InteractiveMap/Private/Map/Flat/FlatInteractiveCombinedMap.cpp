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
}

void AFlatInteractiveCombinedMap::SetMapMode_Implementation(const FName& mode)
{
	Super::SetMapMode_Implementation(mode);
	UpdateLimits();
}

void AFlatInteractiveCombinedMap::InitializeMap_Implementation()
{
	Super::InitializeMap_Implementation();
	MapInitializationDelegate.Broadcast(this);
}
