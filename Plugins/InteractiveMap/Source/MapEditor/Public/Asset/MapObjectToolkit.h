#pragma once
#include "FileIO/DataManagerFunctionLibrary.h"

class UMapObject;
class FTabManager;

class MAPEDITOR_API FMapObjectToolkit : public FAssetEditorToolkit
{
public:
	void InitEditor(const TSharedPtr<IToolkitHost>& InitToolkitHost, UMapObject* Object);
	
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;

	virtual FName GetToolkitFName() const override { return FName("MapObjectEditor"); }
	virtual FText GetBaseToolkitName() const override { return INVTEXT("Map Object Editor"); }
	virtual FString GetWorldCentricTabPrefix() const override { return "MapObjectEditor"; }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }
	
private:
	TWeakObjectPtr<UMapObject> CustomObject = nullptr;
	FTestAdvanced AdvancedStruct;
	// TObjectPtr<UMapObject> MapObject = nullptr;
};
