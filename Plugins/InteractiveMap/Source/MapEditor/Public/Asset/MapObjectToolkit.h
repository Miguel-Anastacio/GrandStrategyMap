#pragma once
#include "MapObject.h"
class SInstancedStructList;
class SMapObjectViewport;
class UMapDataSettings;
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

	void UpdateTreeSelection(int32 Index) const;
	
private:
	void OnLoadFile() const;
	
	TWeakObjectPtr<UMapObject> CustomObject = nullptr;
	TSharedPtr<SInstancedStructList> EditableStructListDisplay = nullptr;
	TSharedPtr<SMapObjectViewport> MapViewport = nullptr;
	TArray<TSharedPtr<FInstancedStruct>> MyListItems;
	TArray<UScriptStruct*> StructTypes;
};
