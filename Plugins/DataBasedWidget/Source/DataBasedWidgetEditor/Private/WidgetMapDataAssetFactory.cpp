
#include "WidgetMapDataAssetFactory.h"
#include "GenericWidgetDataMap.h"
#include "Styling/SlateStyle.h"

UWidgetMapDataAssetFactory::UWidgetMapDataAssetFactory(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	SupportedClass = UWidgetMapDataAsset::StaticClass();
	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UWidgetMapDataAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UWidgetMapDataAsset>(InParent, Class, Name, Flags | RF_Transactional);
}

