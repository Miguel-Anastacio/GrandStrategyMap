
#include "Asset/MapObjectFactory.h"
#include "MapObject.h"
// #include "CustomObjectToolkit.h"
#include "Asset/MapObjectToolkit.h"
#include "Styling/SlateStyle.h"

UMapObjectFactory::UMapObjectFactory(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	SupportedClass = UMapObject::StaticClass();
	
	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UMapObjectFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMapObject>(InParent, Class, Name, Flags | RF_Transactional);
}

EAssetCommandResult UAssetDefinition_MapObject::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	for (UMapObject* EditingAsset : OpenArgs.LoadObjects<UMapObject>())
	{
		const TSharedRef<FMapObjectToolkit> CustomObjectToolkit(new FMapObjectToolkit());
		CustomObjectToolkit->InitEditor(OpenArgs.ToolkitHost, EditingAsset);
	}

	return EAssetCommandResult::Handled;
}
