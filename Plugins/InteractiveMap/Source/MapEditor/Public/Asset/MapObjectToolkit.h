#pragma once
#include "MapEditor.h"
#include "DataDisplay/MapDataSettingsPreset.h"
#include "FileIO/DataManagerFunctionLibrary.h"
#include "FileIO/FilePickerFunctionLibrary.h"

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
	
private:
	void OpenFiles()
	{
		TArray<FString> FilesNames;
		UFilePickerFunctionLibrary::OpenFileDialogJson(FPaths::ProjectDir(), FilesNames);

		UStruct* Struct = DataSettingsPreset->MapDataSettings.TileStructClass;

		UDataManagerFunctionLibrary::ReadJsonFileArray()
		// TArray<Struct->get> SelectedFiles;
	}

	
	TWeakObjectPtr<UMapObject> CustomObject = nullptr;
	TObjectPtr<UMapDataSettings> DataSettingsPreset = nullptr;
	FTestAdvanced AdvancedStruct;
	// TObjectPtr<UMapObject> MapObject = nullptr;
};
