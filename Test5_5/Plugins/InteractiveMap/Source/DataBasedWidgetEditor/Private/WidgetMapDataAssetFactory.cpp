// Copyright 2024 An@stacioDev All rights reserved.

#include "WidgetMapDataAssetFactory.h"
#include "Styling/SlateStyle.h"

UWidgetMapDataAssetFactory::UWidgetMapDataAssetFactory(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	SupportedClass = UPropGenWidgetMapDataAsset::StaticClass();
	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UWidgetMapDataAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UPropGenWidgetMapDataAsset>(InParent, Class, Name, Flags | RF_Transactional);
}

